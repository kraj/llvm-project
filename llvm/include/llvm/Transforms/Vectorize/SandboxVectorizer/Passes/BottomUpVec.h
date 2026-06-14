//===- BottomUpVec.h --------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A Bottom-Up Vectorizer pass.
//

#ifndef LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_BOTTOMUPVEC_H
#define LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_BOTTOMUPVEC_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/SandboxIR/Constant.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/Legality.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/Passes/VecPassBase.h"

namespace llvm::sandboxir {

/// This is a simple bottom-up vectorizer Region pass.
/// It expects a "seed slice" as an input in the Region's Aux vector.
/// The "seed slice" is a vector of instructions that can be used as a starting
/// point for vectorization, like stores to consecutive memory addresses.
/// Starting from the seed instructions, it walks up the def-use chain looking
/// for more instructions that can be vectorized. This pass will generate vector
/// code if it can legally vectorize the code, regardless of whether it is
/// profitable or not. For now profitability is checked at the end of the region
/// pass pipeline by a dedicated pass that accepts or rejects the IR
/// transaction, depending on the cost.
class LLVM_ABI BottomUpVec final : public VecPassBase {
  /// Counter used for force-stopping the vectorizer after this many
  /// invocations. Used for debugging miscompiles.
  unsigned long BottomUpInvocationCnt = 0;

  /// Recursively try to vectorize \p Bndl and its operands. This populates the
  /// `Actions` vector.
  Action *vectorizeRec(ArrayRef<Value *> Bndl, ArrayRef<Value *> UserBndl,
                       unsigned Depth, LegalityAnalysis &Legality);
  /// Generate vector instructions based on `Actions` and return the last vector
  /// created.
  Value *emitVectors();
  /// Entry point for vectorization starting from \p Seeds.
  bool tryVectorize(ArrayRef<Value *> Seeds, LegalityAnalysis &Legality);

public:
  BottomUpVec() : VecPassBase("bottom-up-vec") {}
  bool runOnRegion(Region &Rgn, const Analyses &A) final;
};

} // namespace llvm::sandboxir

#endif // LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_BOTTOMUPVEC_H
