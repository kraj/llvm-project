//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a pass that replaces symbolic `cir.goto` operations
// with control-flow edges, so that the CFG and the region-branch interfaces
// describe every way into a block.
//
// A goto whose label is in the same region becomes a `cir.br`.
//
//===----------------------------------------------------------------------===//

#include "PassDetail.h"
#include "mlir/IR/PatternMatch.h"
#include "clang/CIR/Dialect/IR/CIRDialect.h"
#include "clang/CIR/Dialect/Passes.h"
#include "clang/CIR/Dialect/Transforms/CIRTransformUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"

using namespace mlir;

namespace mlir {
#define GEN_PASS_DEF_GOTOELIMINATION
#define GEN_PASS_DEF_ERASEUNUSEDLABELS
#include "clang/CIR/Dialect/Passes.h.inc"
} // namespace mlir

namespace {

struct GotoEliminationPass
    : public impl::GotoEliminationBase<GotoEliminationPass> {
  using GotoEliminationBase::GotoEliminationBase;

  void runOnOperation() override;
};

struct EraseUnusedLabelsPass
    : public impl::EraseUnusedLabelsBase<EraseUnusedLabelsPass> {
  using EraseUnusedLabelsBase::EraseUnusedLabelsBase;

  void runOnOperation() override;
};

} // namespace

void GotoEliminationPass::runOnOperation() {
  cir::FuncOp func = getOperation();
  IRRewriter rewriter(func.getContext());

  llvm::DenseMap<StringAttr, cir::LabelOp> labels;
  SmallVector<cir::GotoOp> gotos;
  func.getBody().walk([&](Operation *op) {
    if (auto label = dyn_cast<cir::LabelOp>(op))
      labels.try_emplace(label.getLabelAttr(), label);
    else if (auto gotoOp = dyn_cast<cir::GotoOp>(op))
      gotos.push_back(gotoOp);
  });

  for (cir::GotoOp gotoOp : gotos) {
    Block *dest = labels.lookup(gotoOp.getLabelAttr())->getBlock();
    // A goto this cannot route yet is left for GotoSolver.
    if (dest->getParent() != gotoOp->getParentRegion())
      continue;
    rewriter.setInsertionPoint(gotoOp);
    rewriter.replaceOpWithNewOp<cir::BrOp>(gotoOp, dest);
    ++numSameRegionGotos;
  }
}

void EraseUnusedLabelsPass::runOnOperation() {
  ModuleOp module = getOperation();
  cir::AddressTakenLabels attrLabels =
      cir::collectAddressTakenLabelsInAttributes(module);
  for (cir::FuncOp func : module.getOps<cir::FuncOp>()) {
    llvm::DenseSet<StringRef> named;
    if (auto it = attrLabels.find(func.getSymName()); it != attrLabels.end())
      named.insert_range(it->second);
    SmallVector<cir::LabelOp> labels;
    func.getBody().walk([&](Operation *op) {
      if (auto label = dyn_cast<cir::LabelOp>(op))
        labels.push_back(label);
      else if (auto gotoOp = dyn_cast<cir::GotoOp>(op))
        named.insert(gotoOp.getLabel());
      else if (auto blockAddr = dyn_cast<cir::BlockAddressOp>(op))
        named.insert(blockAddr.getBlockAddrInfo().getLabel());
    });
    for (cir::LabelOp label : labels) {
      if (named.contains(label.getLabel()))
        continue;
      label.erase();
      ++numErasedLabels;
    }
  }
}

std::unique_ptr<Pass> mlir::createGotoEliminationPass() {
  return std::make_unique<GotoEliminationPass>();
}

std::unique_ptr<Pass> mlir::createEraseUnusedLabelsPass() {
  return std::make_unique<EraseUnusedLabelsPass>();
}
