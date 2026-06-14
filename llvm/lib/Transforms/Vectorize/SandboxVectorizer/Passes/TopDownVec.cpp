//===- TopDownVec.cpp - A top-down vectorizer pass ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Vectorize/SandboxVectorizer/Passes/TopDownVec.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/SandboxIR/Function.h"
#include "llvm/SandboxIR/Instruction.h"
#include "llvm/SandboxIR/Module.h"
#include "llvm/SandboxIR/Region.h"
#include "llvm/SandboxIR/Utils.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/Debug.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/VecUtils.h"

namespace llvm {

#ifndef NDEBUG
static cl::opt<bool>
    AlwaysVerify("sbvec-topdown-always-verify", cl::init(false), cl::Hidden,
                 cl::desc("Helps find bugs by verifying the IR whenever we "
                          "emit new instructions (*very* expensive)."));
#endif // NDEBUG

static constexpr unsigned long StopAtDisabled =
    std::numeric_limits<unsigned long>::max();
static cl::opt<unsigned long>
    StopAt("sbvec-topdown-stop-at", cl::init(StopAtDisabled), cl::Hidden,
           cl::desc("Vectorize if the invocation count is < than this. 0 "
                    "disables vectorization."));

static constexpr unsigned long StopBundleDisabled =
    std::numeric_limits<unsigned long>::max();
static cl::opt<unsigned long>
    StopBundle("sbvec-topdown-stop-bndl", cl::init(StopBundleDisabled),
               cl::Hidden, cl::desc("Vectorize up to this many bundles."));

namespace sandboxir {

Action *TopDownVec::vectorizeRec(ArrayRef<Value *> Bndl, unsigned Depth,
                                 LegalityAnalysis &Legality) {
  LLVM_DEBUG(dbgs() << DEBUG_PREFIX << "canVectorize() Bundle:\n";
             VecUtils::dump(Bndl));
  const auto &LegalityRes =
      Legality.canVectorize(Bndl, /*SkipScheduling=*/true);
  LLVM_DEBUG(dbgs() << DEBUG_PREFIX << "Legality: " << LegalityRes << "\n");
  auto ActionPtr =
      std::make_unique<Action>(&LegalityRes, Bndl, ArrayRef<Value *>(), Depth);

  if (LegalityRes.getSubclassID() == LegalityResultID::Widen) {
    IMaps->registerVector(Bndl, ActionPtr.get());
  }

  // Pre-order push so defs are before uses.
  Action *Action = ActionPtr.get();
  Actions.push_back(std::move(ActionPtr));

  if (LegalityRes.getSubclassID() == LegalityResultID::Widen) {
    // Find users of Bndl to recurse down.
    // Group the first user of each element if they match.
    SmallVector<Value *, 4> UserBndl;
    bool CanFormUserBndl = true;
    for (Value *V : Bndl) {
      if (V->user_begin() == V->user_end()) {
        CanFormUserBndl = false;
        break;
      }
      UserBndl.push_back(*V->user_begin());
    }

    if (CanFormUserBndl) {
      auto *U0 = dyn_cast<Instruction>(UserBndl[0]);
      if (!U0 || IMaps->isVectorized(U0))
        CanFormUserBndl = false;
      else {
        for (Value *U : drop_begin(UserBndl)) {
          auto *UI = dyn_cast<Instruction>(U);
          if (!UI || UI->getOpcode() != U0->getOpcode() ||
              UI->getType() != U0->getType() || IMaps->isVectorized(UI)) {
            CanFormUserBndl = false;
            break;
          }
        }
      }
    }

    if (CanFormUserBndl) {
      vectorizeRec(UserBndl, Depth + 1, Legality);
    }
  }

  return Action;
}

Value *TopDownVec::emitVectors() {
  Value *NewVec = nullptr;
  for (const auto &ActionPtr : Actions) {
    ArrayRef<Value *> Bndl = ActionPtr->Bndl;
    const LegalityResult &LegalityRes = *ActionPtr->LegalityRes;
    unsigned Depth = ActionPtr->Depth;
    auto *UserBB = cast<Instruction>(Bndl[0])->getParent();

    switch (LegalityRes.getSubclassID()) {
    case LegalityResultID::Widen: {
      auto *I = cast<Instruction>(Bndl[0]);
      SmallVector<Value *, 2> VecOperands;
      switch (I->getOpcode()) {
      case Instruction::Opcode::Load:
        VecOperands.push_back(cast<LoadInst>(I)->getPointerOperand());
        break;
      case Instruction::Opcode::Store: {
        auto OpBndl = getOperand(Bndl, 0);
        if (Action *OpA = IMaps->getVectorForOrig(OpBndl[0])) {
          VecOperands.push_back(OpA->Vec);
        } else {
          Value *Packed = createPack(OpBndl, UserBB);
          VecOperands.push_back(Packed);
        }
        VecOperands.push_back(cast<StoreInst>(I)->getPointerOperand());
        break;
      }
      default:
        // Visit all operands and gather vectorized inputs.
        for (unsigned OpIdx = 0; OpIdx < I->getNumOperands(); ++OpIdx) {
          SmallVector<Value *, 4> OpBndl = getOperand(Bndl, OpIdx);
          if (Action *OpA = IMaps->getVectorForOrig(OpBndl[0])) {
            VecOperands.push_back(OpA->Vec);
          } else {
            Value *Packed = createPack(OpBndl, UserBB);
            VecOperands.push_back(Packed);
          }
        }
        break;
      }
      NewVec = createVectorInstr(ActionPtr->Bndl, VecOperands);
      LLVM_DEBUG(dbgs() << DEBUG_PREFIX << "New instr: " << *NewVec << "\n");
      if (NewVec != nullptr)
        collectPotentiallyDeadInstrs(Bndl);

      emitUnpacksForExternalUses(ActionPtr->Bndl, NewVec);
      break;
    }
    case LegalityResultID::DiamondReuse: {
      NewVec = cast<DiamondReuse>(LegalityRes).getVector()->Vec;
      break;
    }
    case LegalityResultID::DiamondReuseWithShuffle: {
      auto *VecOp = cast<DiamondReuseWithShuffle>(LegalityRes).getVector()->Vec;
      const ShuffleMask &Mask =
          cast<DiamondReuseWithShuffle>(LegalityRes).getMask();
      NewVec = createShuffle(VecOp, Mask, UserBB);
      break;
    }
    case LegalityResultID::DiamondReuseMultiInput: {
      const auto &Descr =
          cast<DiamondReuseMultiInput>(LegalityRes).getCollectDescr();
      Type *ResTy = VecUtils::getWideType(Bndl[0]->getType(), Bndl.size());

      SmallVector<Value *, 4> DescrInstrs;
      for (const auto &ElmDescr : Descr.getDescrs()) {
        auto *V = ElmDescr.needsExtract() ? ElmDescr.getValue()->Vec
                                          : ElmDescr.getScalar();
        if (auto *Inst = dyn_cast<Instruction>(V))
          DescrInstrs.push_back(Inst);
      }
      BasicBlock::iterator WhereIt =
          getInsertPointAfterInstrs(DescrInstrs, UserBB);

      Value *LastV = PoisonValue::get(ResTy);
      Context &Ctx = LastV->getContext();
      unsigned Lane = 0;
      for (const auto &ElmDescr : Descr.getDescrs()) {
        Value *VecOp = nullptr;
        Value *ValueToInsert;
        if (ElmDescr.needsExtract()) {
          VecOp = ElmDescr.getValue()->Vec;
          ConstantInt *IdxC =
              ConstantInt::get(Type::getInt32Ty(Ctx), ElmDescr.getExtractIdx());
          ValueToInsert = ExtractElementInst::create(
              VecOp, IdxC, WhereIt, VecOp->getContext(), "VExt");
        } else {
          ValueToInsert = ElmDescr.getScalar();
        }
        auto NumLanesToInsert = VecUtils::getNumLanes(ValueToInsert);
        if (NumLanesToInsert == 1) {
          ConstantInt *LaneC = ConstantInt::get(Type::getInt32Ty(Ctx), Lane);
          LastV = InsertElementInst::create(LastV, ValueToInsert, LaneC,
                                            WhereIt, Ctx, "VIns");
        } else {
          for (unsigned LnCnt = 0; LnCnt != NumLanesToInsert; ++LnCnt) {
            auto *ExtrIdxC = ConstantInt::get(Type::getInt32Ty(Ctx), LnCnt);
            auto *ExtrI = ExtractElementInst::create(ValueToInsert, ExtrIdxC,
                                                     WhereIt, Ctx, "VExt");
            unsigned InsLane = Lane + LnCnt;
            auto *InsLaneC = ConstantInt::get(Type::getInt32Ty(Ctx), InsLane);
            LastV = InsertElementInst::create(LastV, ExtrI, InsLaneC, WhereIt,
                                              Ctx, "VIns");
          }
        }
        Lane += NumLanesToInsert;
      }
      NewVec = LastV;
      break;
    }
    case LegalityResultID::Pack: {
      if (Depth == 0)
        return nullptr;
      NewVec = createPack(Bndl, UserBB);
      break;
    }
    }
    if (NewVec != nullptr) {
      Change = true;
      ActionPtr->Vec = NewVec;
    }
#ifndef NDEBUG
    if (AlwaysVerify) {
      Instruction *I0 = cast<Instruction>(Bndl[0]);
      assert(!Utils::verifyFunction(I0->getParent()->getParent(), dbgs()) &&
             "Broken function!");
    }
#endif // NDEBUG
  }
  return NewVec;
}

bool TopDownVec::tryVectorize(ArrayRef<Value *> Bndl,
                              LegalityAnalysis &Legality) {
  Change = false;
  if (LLVM_UNLIKELY(TopDownInvocationCnt++ >= StopAt &&
                    StopAt != StopAtDisabled))
    return false;
  DeadInstrCandidates.clear();
  Legality.clear();
  Actions.clear();
  DebugBndlCnt = 0;
  vectorizeRec(Bndl, /*Depth=*/0, Legality);
  LLVM_DEBUG(dbgs() << DEBUG_PREFIX << "TopDownVec: Vectorization Actions:\n";
             Actions.dump());
  emitVectors();
  tryEraseDeadInstrs(DEBUG_PREFIX);
  return Change;
}

bool TopDownVec::runOnRegion(Region &Rgn, const Analyses &A) {
  const auto &SeedSlice = Rgn.getAux();
  assert(SeedSlice.size() >= 2 && "Bad slice!");
  Function &F = *SeedSlice[0]->getParent()->getParent();
  IMaps = std::make_unique<InstrMaps>();
  LegalityAnalysis Legality(A.getAA(), A.getScalarEvolution(),
                            F.getParent()->getDataLayout(), F.getContext(),
                            *IMaps);

  SmallVector<Value *> SeedSliceVals(SeedSlice.begin(), SeedSlice.end());
  return tryVectorize(SeedSliceVals, Legality);
}

} // namespace sandboxir
} // namespace llvm
