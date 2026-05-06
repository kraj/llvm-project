//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Lower global variables with target extension type "amdgpu.named.barrier"
// that require specialized address assignment. It assigns a unique
// barrier identifier to each named-barrier variable and encodes
// this identifier within the !absolute_symbol metadata of that global.
//
//===----------------------------------------------------------------------===//

#include "AMDGPU.h"
#include "AMDGPUMemoryUtils.h"
#include "AMDGPUTargetMachine.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/ReplaceConstant.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

#include <algorithm>

#define DEBUG_TYPE "amdgpu-lower-exec-sync"

using namespace llvm;
using namespace AMDGPU;

namespace {

static bool isNamedBarrierToLower(const GlobalVariable &GV) {
  return isNamedBarrier(GV) && !GV.isAbsoluteSymbolRef();
}

// If GV is also used directly by other kernels, create a new GV
// used only by this kernel and its function.
static GlobalVariable *uniquifyGVPerKernel(Module &M, GlobalVariable *GV,
                                           Function *KF) {
  bool NeedsReplacement = false;
  for (Use &U : GV->uses()) {
    if (auto *I = dyn_cast<Instruction>(U.getUser())) {
      Function *F = I->getFunction();
      if (isKernel(*F) && F != KF) {
        NeedsReplacement = true;
        break;
      }
    }
  }
  if (!NeedsReplacement)
    return GV;
  // Create a new GV used only by this kernel and its function
  GlobalVariable *NewGV = new GlobalVariable(
      M, GV->getValueType(), GV->isConstant(), GV->getLinkage(),
      GV->getInitializer(), GV->getName() + "." + KF->getName(), nullptr,
      GV->getThreadLocalMode(), GV->getType()->getAddressSpace());
  NewGV->copyAttributesFrom(GV);
  for (Use &U : make_early_inc_range(GV->uses())) {
    if (auto *I = dyn_cast<Instruction>(U.getUser())) {
      Function *F = I->getFunction();
      if (!isKernel(*F) || F == KF) {
        U.getUser()->replaceUsesOfWith(GV, NewGV);
      }
    }
  }
  return NewGV;
}

// Write the specified address into metadata where it can be retrieved by
// the assembler. Format is a half open range, [Address Address+1)
static void recordAbsoluteAddress(Module *M, GlobalVariable *GV,
                                  uint32_t Address) {
  LLVMContext &Ctx = M->getContext();
  auto *IntTy = M->getDataLayout().getIntPtrType(Ctx, AMDGPUAS::LOCAL_ADDRESS);
  auto *MinC = ConstantAsMetadata::get(ConstantInt::get(IntTy, Address));
  auto *MaxC = ConstantAsMetadata::get(ConstantInt::get(IntTy, Address + 1));
  GV->setMetadata(LLVMContext::MD_absolute_symbol,
                  MDNode::get(Ctx, {MinC, MaxC}));
}

template <typename T> SmallVector<T> sortByName(SmallVector<T> &&V) {
  sort(V, [](const auto *L, const auto *R) {
    return L->getName() < R->getName();
  });
  return {std::move(V)};
}

static bool lowerExecSyncGlobalVariables(
    Module &M, GVUsesInfoTy &UsesInfo,
    VariableFunctionMap &GVsToKernelsThatNeedToAccessItIndirectly) {
  bool Changed = false;
  const DataLayout &DL = M.getDataLayout();
  // The 1st round: give module-absolute assignments
  int NumAbsolutes = 0;
  SmallVector<GlobalVariable *> OrderedGVs;
  for (auto &K : GVsToKernelsThatNeedToAccessItIndirectly) {
    GlobalVariable *GV = K.first;
    assert(isNamedBarrierToLower(*GV));

    // give a module-absolute assignment if it is indirectly accessed by
    // multiple kernels. This is not precise, but we don't want to duplicate
    // a function when it is called by multiple kernels.
    if (GVsToKernelsThatNeedToAccessItIndirectly[GV].size() > 1) {
      OrderedGVs.push_back(GV);
    } else {
      // leave it to the 2nd round, which will give a kernel-relative
      // assignment if it is only indirectly accessed by one kernel
      UsesInfo.DirectAccess[*K.second.begin()].insert(GV);
    }
    GVsToKernelsThatNeedToAccessItIndirectly.erase(GV);
  }
  OrderedGVs = sortByName(std::move(OrderedGVs));
  for (GlobalVariable *GV : OrderedGVs) {
    unsigned BarId = NumAbsolutes + 1;
    unsigned BarCnt = GV->getGlobalSize(DL) / 16;
    NumAbsolutes += BarCnt;
    recordAbsoluteAddress(&M, GV, BarId);
  }
  OrderedGVs.clear();

  // The 2nd round: give a kernel-relative assignment for GV that
  // either only indirectly accessed by single kernel or only directly
  // accessed by multiple kernels.
  SmallVector<Function *> OrderedKernels;
  for (auto &K : UsesInfo.DirectAccess) {
    Function *F = K.first;
    assert(isKernel(*F));
    OrderedKernels.push_back(F);
  }
  OrderedKernels = sortByName(std::move(OrderedKernels));

  DenseMap<Function *, uint32_t> Kernel2BarId;
  for (Function *F : OrderedKernels) {
    for (GlobalVariable *GV : UsesInfo.DirectAccess[F]) {
      assert(isNamedBarrierToLower(*GV));

      UsesInfo.DirectAccess[F].erase(GV);
      if (GV->isAbsoluteSymbolRef()) {
        // already assigned
        continue;
      }
      OrderedGVs.push_back(GV);
    }
    OrderedGVs = sortByName(std::move(OrderedGVs));
    for (GlobalVariable *GV : OrderedGVs) {
      // GV could also be used directly by other kernels. If so, we need to
      // create a new GV used only by this kernel and its function.
      auto *NewGV = uniquifyGVPerKernel(M, GV, F);
      Changed |= (NewGV != GV);
      unsigned BarId = Kernel2BarId[F];
      BarId += NumAbsolutes + 1;
      unsigned BarCnt = GV->getGlobalSize(DL) / 16;
      Kernel2BarId[F] += BarCnt;
      recordAbsoluteAddress(&M, NewGV, BarId);
    }
    OrderedGVs.clear();
  }
  // TODO: is this even necessary?
  // Also erase those special variables from indirect_access.
  for (auto &K : UsesInfo.IndirectAccess) {
    assert(isKernel(*K.first));
    for (GlobalVariable *GV : K.second) {
      if (isNamedBarrier(*GV))
        K.second.erase(GV);
    }
  }
  return Changed;
}

// With object linking, barrier ID assignment is deferred to the linker.
// Externalize named barrier globals and emit self-contained metadata so the
// AsmPrinter can generate the callgraph entries the linker needs.
static bool handleNamedBarriersForObjectLinking(Module &M) {
  DenseMap<GlobalVariable *, DenseSet<Function *>> BarrierToFuncs;
  for (GlobalVariable &GV : M.globals()) {
    if (!isNamedBarrier(GV) || GV.use_empty())
      continue;
    for (User *U : GV.users()) {
      if (auto *I = dyn_cast<Instruction>(U))
        BarrierToFuncs[&GV].insert(I->getFunction());
    }
  }
  if (BarrierToFuncs.empty())
    return false;

  LLVMContext &Ctx = M.getContext();
  NamedMDNode *BarMD = M.getOrInsertNamedMetadata("amdgpu.named_barrier.uses");

  std::string ModuleId;
  ModuleId = getUniqueModuleId(&M);
  assert(!ModuleId.empty() &&
         "modules with named barriers should have a unique ID");
  for (auto &[V, Funcs] : BarrierToFuncs) {
    if (V->hasLocalLinkage())
      V->setName("__amdgpu_named_barrier." + V->getName() + ModuleId);
    else if (!V->getName().starts_with("__amdgpu_named_barrier"))
      V->setName("__amdgpu_named_barrier." + V->getName());
    V->setInitializer(nullptr);
    V->setLinkage(GlobalValue::ExternalLinkage);

    SmallVector<Metadata *, 4> Ops;
    Ops.push_back(ValueAsMetadata::get(V));
    for (Function *F : Funcs)
      Ops.push_back(ValueAsMetadata::get(F));
    BarMD->addOperand(MDNode::get(Ctx, Ops));
  }
  return true;
}

static bool runLowerExecSyncGlobals(Module &M) {
  if (AMDGPUTargetMachine::EnableObjectLinking)
    return handleNamedBarriersForObjectLinking(M);

  CallGraph CG = CallGraph(M);
  bool Changed = false;
  Changed |=
      eliminateGVConstantExprUsesFromAllInstructions(M, isNamedBarrierToLower);

  // For each kernel, what variables does it access directly or through
  // callees
  GVUsesInfoTy BarrierUsesInfo =
      getTransitiveUsesOfGV(CG, M, isNamedBarrierToLower);

  // For each variable accessed through callees, which kernels access it
  VariableFunctionMap BarriersToKernelsThatNeedToAccessItIndirectly;
  for (auto &K : BarrierUsesInfo.IndirectAccess) {
    Function *F = K.first;
    assert(isKernel(*F));
    for (GlobalVariable *GV : K.second) {
      BarriersToKernelsThatNeedToAccessItIndirectly[GV].insert(F);
    }
  }

  Changed |= lowerExecSyncGlobalVariables(
      M, BarrierUsesInfo, BarriersToKernelsThatNeedToAccessItIndirectly);

  return Changed;
}

class AMDGPULowerExecSyncLegacy : public ModulePass {
public:
  static char ID;
  AMDGPULowerExecSyncLegacy() : ModulePass(ID) {}
  bool runOnModule(Module &M) override;
};

} // namespace

char AMDGPULowerExecSyncLegacy::ID = 0;
char &llvm::AMDGPULowerExecSyncLegacyPassID = AMDGPULowerExecSyncLegacy::ID;

INITIALIZE_PASS_BEGIN(AMDGPULowerExecSyncLegacy, DEBUG_TYPE,
                      "AMDGPU lowering of execution synchronization", false,
                      false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(AMDGPULowerExecSyncLegacy, DEBUG_TYPE,
                    "AMDGPU lowering of execution synchronization", false,
                    false)

bool AMDGPULowerExecSyncLegacy::runOnModule(Module &M) {
  return runLowerExecSyncGlobals(M);
}

ModulePass *llvm::createAMDGPULowerExecSyncLegacyPass() {
  return new AMDGPULowerExecSyncLegacy();
}

PreservedAnalyses AMDGPULowerExecSyncPass::run(Module &M,
                                               ModuleAnalysisManager &AM) {
  return runLowerExecSyncGlobals(M) ? PreservedAnalyses::none()
                                    : PreservedAnalyses::all();
}
