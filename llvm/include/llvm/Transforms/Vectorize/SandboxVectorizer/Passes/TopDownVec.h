//===- TopDownVec.h ---------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A Top-Down Vectorizer pass.
//

#ifndef LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_TOPDOWNVEC_H
#define LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_TOPDOWNVEC_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/Legality.h"
#include "llvm/Transforms/Vectorize/SandboxVectorizer/Passes/VecPassBase.h"

namespace llvm::sandboxir {

class LLVM_ABI TopDownVec final : public VecPassBase {
  unsigned long TopDownInvocationCnt = 0;

  Action *vectorizeRec(ArrayRef<Value *> Bndl, unsigned Depth,
                       LegalityAnalysis &Legality);
  Value *emitVectors();
  bool tryVectorize(ArrayRef<Value *> Seeds, LegalityAnalysis &Legality);

public:
  TopDownVec() : VecPassBase("top-down-vec") {}
  bool runOnRegion(Region &Rgn, const Analyses &A) final;
};

} // namespace llvm::sandboxir

#endif // LLVM_TRANSFORMS_VECTORIZE_SANDBOXVECTORIZER_PASSES_TOPDOWNVEC_H
