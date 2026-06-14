//===- VecPassBase.h --------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Abstract base class shared by the top-down and bottom-up vectorizer passes.
// Houses the common state (InstrMaps, dead-instruction bookkeeping, actions
// list) and the utility functions used by both vectorizer passes.
//

#ifndef LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_VECPASSBASE_H
#define LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_VECPASSBASE_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/SandboxIR/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/InstrMaps.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/Legality.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/VecUtils.h"

namespace llvm::sandboxir {

/// Abstract base class shared by TopDownVec and BottomUpVec.
/// It owns the state common to both vectorizers and implements the utility
/// functions used by both vectorizer passes.
class LLVM_ABI VecPassBase : public RegionPass {
protected:
  /// Set to true whenever the pass emits vector code in the current region.
  bool Change = false;
  /// Scalar instructions that may be dead after vectorization.
  DenseSet<Instruction *> DeadInstrCandidates;
  /// Maps scalar values/instructions to their vector counterparts.
  std::unique_ptr<InstrMaps> IMaps;
  /// Helper counter for debugging — counts bundles attempted in vectorizeRec.
  unsigned DebugBndlCnt = 0;

  /// Ordered list of vectorization decisions produced by vectorizeRec.
  class ActionsVector {
    SmallVector<std::unique_ptr<Action>, 16> Actions;

  public:
    auto begin() const { return Actions.begin(); }
    auto end() const { return Actions.end(); }
    auto rbegin() const { return Actions.rbegin(); }
    auto rend() const { return Actions.rend(); }
    void push_back(std::unique_ptr<Action> &&ActPtr) {
      ActPtr->Idx = Actions.size();
      Actions.push_back(std::move(ActPtr));
    }
    void clear() { Actions.clear(); }
#ifndef NDEBUG
    void print(raw_ostream &OS) const;
    LLVM_DUMP_METHOD void dump() const;
#endif // NDEBUG
  };
  ActionsVector Actions;

  explicit VecPassBase(StringRef Name) : RegionPass(Name) {}

  // -----------------------------------------------------------------------
  // Utility functions. These are owned by VecPassBase and accessible only
  // to subclasses (TopDownVec and BottomUpVec) through inheritance.
  // -----------------------------------------------------------------------

  /// \Returns the operand at \p OpIdx for each instruction in \p Bndl.
  static SmallVector<Value *, 4> getOperand(ArrayRef<Value *> Bndl,
                                            unsigned OpIdx) {
    SmallVector<Value *, 4> Operands;
    for (Value *BndlV : Bndl)
      Operands.push_back(cast<Instruction>(BndlV)->getOperand(OpIdx));
    return Operands;
  }

  /// \Returns the BB iterator after the lowest instruction in \p Vals, or the
  /// top of BB (after any PHIs) if no instruction is found in \p Vals.
  static BasicBlock::iterator getInsertPointAfterInstrs(ArrayRef<Value *> Vals,
                                                        BasicBlock *BB) {
    auto *BotI = VecUtils::getLastPHIOrSelf(VecUtils::getLowest(Vals, BB));
    if (BotI == nullptr)
      return BB->empty() ? BB->begin()
                         : std::next(VecUtils::getLastPHIOrSelf(&*BB->begin())
                                         ->getIterator());
    return std::next(BotI->getIterator());
  }

  /// Creates and returns a new vector instruction widening \p Bndl with
  /// \p Operands as the vector operands.
  static Value *createVectorInstr(ArrayRef<Value *> Bndl,
                                  ArrayRef<Value *> Operands);

  /// Creates a shuffle of \p VecOp according to \p Mask, inserted in \p UserBB.
  static Value *createShuffle(Value *VecOp, const ShuffleMask &Mask,
                              BasicBlock *UserBB);

  /// Packs all scalars/vectors in \p ToPack into a single vector, inserted in
  /// \p UserBB.
  static Value *createPack(ArrayRef<Value *> ToPack, BasicBlock *UserBB);

  /// Adds the instructions in \p Bndl (and pointer operands of loads/stores)
  /// to DeadInstrCandidates for later cleanup.
  void collectPotentiallyDeadInstrs(ArrayRef<Value *> Bndl);

  /// Erases all zero-use instructions from DeadInstrCandidates.
  /// \p PassName is used as the debug-output prefix.
  void tryEraseDeadInstrs(StringRef PassName);

  /// For each element of \p Bndl that has external uses (users not yet
  /// vectorized), emits extract instructions from \p Vec and replaces the
  /// scalar uses with them.
  void emitUnpacksForExternalUses(ArrayRef<Value *> Bndl, Value *Vec);
};

} // namespace llvm::sandboxir

#endif // LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_VECPASSBASE_H
