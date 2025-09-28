//===- EarlyMemoryScheduler.cpp - Schedule memory ops early --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass moves memory instructions (loads and stores) to the earliest
// possible position in their basic blocks while respecting data dependencies.
// The goal is to allow memory operations to dispatch earlier, potentially
// hiding memory latency. This pass runs before register allocation.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "early-memory-scheduler"

STATISTIC(NumMemOpsScheduled, "Number of memory operations scheduled early");
STATISTIC(NumLoadsScheduled, "Number of loads scheduled early");
STATISTIC(NumStoresScheduled, "Number of stores scheduled early");

static cl::opt<bool>
    EnableEarlyMemScheduler("enable-early-mem-scheduler", cl::Hidden,
                           cl::init(true),
                           cl::desc("Enable early memory scheduling"));

namespace {

class EarlyMemoryScheduler : public MachineFunctionPass {
  const TargetInstrInfo *TII = nullptr;
  const TargetRegisterInfo *TRI = nullptr;
  MachineRegisterInfo *MRI = nullptr;
  MachineDominatorTree *MDT = nullptr;

public:
  static char ID;

  EarlyMemoryScheduler() : MachineFunctionPass(ID) {
    initializeEarlyMemorySchedulerPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<MachineDominatorTreeWrapperPass>();
    AU.addPreserved<MachineDominatorTreeWrapperPass>();
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  StringRef getPassName() const override {
    return "Early Memory Scheduler";
  }

private:
  bool processBasicBlock(MachineBasicBlock &MBB);
  bool canMoveMemOp(MachineInstr &MI, MachineBasicBlock::iterator InsertPos);
  void collectDependencies(MachineInstr &MI,
                           SmallVectorImpl<MachineInstr*> &Deps);
  bool hasMemoryDependency(MachineInstr &Earlier, MachineInstr &Later);
};

} // end anonymous namespace

char EarlyMemoryScheduler::ID = 0;
char &llvm::EarlyMemorySchedulerID = EarlyMemoryScheduler::ID;

INITIALIZE_PASS_BEGIN(EarlyMemoryScheduler, DEBUG_TYPE,
                      "Early Memory Scheduler", false, false)
INITIALIZE_PASS_DEPENDENCY(MachineDominatorTreeWrapperPass)
INITIALIZE_PASS_END(EarlyMemoryScheduler, DEBUG_TYPE,
                    "Early Memory Scheduler", false, false)

bool EarlyMemoryScheduler::runOnMachineFunction(MachineFunction &MF) {
  if (!EnableEarlyMemScheduler)
    return false;

  LLVM_DEBUG(dbgs() << "Running Early Memory Scheduler on function "
                    << MF.getName() << "\n");

  TII = MF.getSubtarget().getInstrInfo();
  TRI = MF.getSubtarget().getRegisterInfo();
  MRI = &MF.getRegInfo();
  MDT = &getAnalysis<MachineDominatorTreeWrapperPass>().getDomTree();

  bool Changed = false;

  // Process blocks in reverse post-order to handle dependencies properly
  ReversePostOrderTraversal<MachineFunction*> RPOT(&MF);
  for (MachineBasicBlock *MBB : RPOT) {
    Changed |= processBasicBlock(*MBB);
  }

  return Changed;
}

bool EarlyMemoryScheduler::processBasicBlock(MachineBasicBlock &MBB) {
  LLVM_DEBUG(dbgs() << "Processing BB#" << MBB.getNumber() << "\n");

  bool Changed = false;

  // Collect all memory operations in the block
  SmallVector<MachineInstr*, 16> MemOps;
  for (MachineInstr &MI : MBB) {
    if (MI.mayLoad() || MI.mayStore()) {
      // Skip volatile memory operations and atomic operations
      if (MI.hasOrderedMemoryRef())
        continue;

      // Skip instructions with side effects that we can't model
      if (MI.hasUnmodeledSideEffects())
        continue;

      MemOps.push_back(&MI);
    }
  }

  // Try to move each memory operation as early as possible
  for (MachineInstr *MI : MemOps) {
    // Find the earliest valid insertion point
    MachineBasicBlock::iterator InsertPos = MBB.begin();
    MachineBasicBlock::iterator CurrentPos = MI->getIterator();

    // Don't move PHI nodes or instructions that must be at block start
    while (InsertPos != CurrentPos && InsertPos->isPHI())
      ++InsertPos;

    // Check each position from the beginning to find the earliest valid spot
    MachineBasicBlock::iterator BestPos = CurrentPos;
    for (auto TestPos = InsertPos; TestPos != CurrentPos; ++TestPos) {
      if (canMoveMemOp(*MI, TestPos)) {
        BestPos = TestPos;
        break;
      }
    }

    // Move the instruction if we found a better position
    if (BestPos != CurrentPos) {
      LLVM_DEBUG(dbgs() << "Moving memory operation: " << *MI);
      MBB.splice(BestPos, &MBB, MI);
      Changed = true;

      if (MI->mayLoad()) {
        ++NumLoadsScheduled;
      } else {
        ++NumStoresScheduled;
      }
      ++NumMemOpsScheduled;
    }
  }

  return Changed;
}

bool EarlyMemoryScheduler::canMoveMemOp(MachineInstr &MI,
                                        MachineBasicBlock::iterator InsertPos) {
  // Check register dependencies
  SmallVector<Register, 4> UsedRegs;
  SmallVector<Register, 4> DefRegs;

  // Collect registers used and defined by the memory operation
  for (const MachineOperand &MO : MI.operands()) {
    if (!MO.isReg() || !MO.getReg())
      continue;

    if (MO.isUse())
      UsedRegs.push_back(MO.getReg());
    if (MO.isDef())
      DefRegs.push_back(MO.getReg());
  }

  // Check all instructions between InsertPos and MI's current position
  MachineBasicBlock::iterator CheckPos = InsertPos;
  MachineBasicBlock::iterator EndPos = MI.getIterator();

  while (CheckPos != EndPos) {
    MachineInstr &CheckMI = *CheckPos;

    // Check for register dependencies
    for (Register Reg : UsedRegs) {
      // The memory op uses Reg, so it can't move before an instruction
      // that defines Reg
      if (CheckMI.modifiesRegister(Reg, TRI))
        return false;
    }

    for (Register Reg : DefRegs) {
      // The memory op defines Reg, so it can't move before an instruction
      // that uses or defines Reg
      if (CheckMI.readsRegister(Reg, TRI) ||
          CheckMI.modifiesRegister(Reg, TRI))
        return false;
    }

    // Check for memory dependencies
    if (CheckMI.mayLoad() || CheckMI.mayStore()) {
      if (hasMemoryDependency(CheckMI, MI))
        return false;
    }

    // Don't move across instructions with side effects
    if (CheckMI.isCall() || CheckMI.isTerminator() ||
        CheckMI.hasUnmodeledSideEffects())
      return false;

    ++CheckPos;
  }

  return true;
}

bool EarlyMemoryScheduler::hasMemoryDependency(MachineInstr &Earlier,
                                               MachineInstr &Later) {
  // Conservative memory dependency checking
  // A store can't move before another store or load (unless proven independent)
  // A load can't move before a store (unless proven independent)

  if (Earlier.mayStore() && Later.mayLoad())
    return true; // Store-Load dependency

  if (Earlier.mayStore() && Later.mayStore())
    return true; // Store-Store dependency

  if (Earlier.mayLoad() && Later.mayStore())
    return true; // Load-Store dependency (less common but possible with volatile)

  // Two loads don't have dependencies unless they have side effects
  if (Earlier.mayLoad() && Later.mayLoad()) {
    if (Earlier.hasOrderedMemoryRef() || Later.hasOrderedMemoryRef())
      return true;
  }

  return false;
}

void EarlyMemoryScheduler::collectDependencies(MachineInstr &MI,
                                               SmallVectorImpl<MachineInstr*> &Deps) {
  // Collect all instructions that MI depends on
  for (const MachineOperand &MO : MI.operands()) {
    if (!MO.isReg() || !MO.getReg() || !MO.isUse())
      continue;

    Register Reg = MO.getReg();
    if (Register::isPhysicalRegister(Reg))
      continue;

    MachineInstr *DefMI = MRI->getVRegDef(Reg);
    if (DefMI && DefMI->getParent() == MI.getParent())
      Deps.push_back(DefMI);
  }
}

// Factory function for creating the pass
FunctionPass *llvm::createEarlyMemorySchedulerPass() {
  return new EarlyMemoryScheduler();
}