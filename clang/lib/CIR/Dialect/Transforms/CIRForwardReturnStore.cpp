//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a pass that returns the value stored to a function's
// return slot directly when that store dominates the only return, as classic
// codegen does in CodeGenFunction::EmitFunctionEpilog.
//
//===----------------------------------------------------------------------===//

#include "PassDetail.h"
#include "mlir/IR/Block.h"
#include "clang/CIR/Dialect/IR/CIRDialect.h"
#include "clang/CIR/Dialect/Passes.h"
#include "llvm/ADT/SmallPtrSet.h"

using namespace mlir;

namespace mlir {
#define GEN_PASS_DEF_CIRFORWARDRETURNSTORE
#include "clang/CIR/Dialect/Passes.h.inc"
} // namespace mlir

namespace {

struct CIRForwardReturnStorePass
    : public impl::CIRForwardReturnStoreBase<CIRForwardReturnStorePass> {
  using CIRForwardReturnStoreBase::CIRForwardReturnStoreBase;

  void runOnOperation() override;
};

/// Returns `op` as a store to `slot` if it is one that may be forwarded.
static cir::StoreOp getStoreToSlot(Operation *op, Value slot) {
  auto store = dyn_cast<cir::StoreOp>(op);
  if (!store || store.getAddr() != slot || store.isVolatileOrAtomic())
    return {};
  return store;
}

/// Finds the store whose value `load` (the load of the return slot feeding the
/// function's only return) can be replaced with. This mirrors
/// findDominatingStoreToReturnValue in clang/lib/CodeGen/CGCall.cpp, where the
/// load does not exist yet and the shared return block holds the insertion
/// point.
static cir::StoreOp findDominatingStoreToReturnSlot(cir::LoadOp load) {
  Value slot = load.getAddr();

  // If the slot has other uses than one store and the load, only look at the
  // operation right before the load, skipping casts and lifetime markers.
  if (!slot.hasNUses(2)) {
    for (Operation *op = load->getPrevNode(); op; op = op->getPrevNode()) {
      if (isa<cir::CastOp, cir::LifetimeEndOp>(op))
        continue;
      return getStoreToSlot(op, slot);
    }
    return {};
  }

  Operation *user = *llvm::find_if(slot.getUsers(), [&](Operation *user) {
    return user != load.getOperation();
  });
  cir::StoreOp store = getStoreToSlot(user, slot);
  if (!store)
    return {};

  // The store must dominate the load. As in classic codegen, only follow the
  // chain of single predecessors up from the load's block.
  Block *storeBlock = store->getBlock();
  if (load->getBlock() == storeBlock)
    return store->isBeforeInBlock(load) ? store : cir::StoreOp();
  llvm::SmallPtrSet<Block *, 4> seen;
  for (Block *block = load->getBlock(); block != storeBlock;) {
    if (!seen.insert(block).second)
      return {};
    block = block->getSinglePredecessor();
    if (!block)
      return {};
  }
  return store;
}

/// Classic codegen only stores scalars to the return value slot with a single
/// store; aggregates and complex values are copied or stored by parts.
static bool isScalarSlotType(Type type) {
  return !isa<cir::RecordType, cir::ArrayType, cir::ComplexType>(type);
}

static void forwardReturnStore(cir::FuncOp func) {
  // Classic codegen returns through a single return block, so only a function
  // with one return of a value is a candidate.
  cir::ReturnOp ret;
  WalkResult walk = func.walk([&](cir::ReturnOp op) {
    if (op.getInput().empty())
      return WalkResult::advance();
    if (ret)
      return WalkResult::interrupt();
    ret = op;
    return WalkResult::advance();
  });
  if (walk.wasInterrupted() || !ret)
    return;

  auto load = ret.getInput().front().getDefiningOp<cir::LoadOp>();
  if (!load || !load->hasOneUse() || load->getBlock() != ret->getBlock() ||
      load.isVolatileOrAtomic())
    return;
  auto slot = load.getAddr().getDefiningOp<cir::AllocaOp>();
  if (!slot || !slot.getReturnSlot() || !isScalarSlotType(slot.getAllocaType()))
    return;

  cir::StoreOp store = findDominatingStoreToReturnSlot(load);
  if (!store)
    return;

  // Return the stored value at the store's location and drop the store and
  // the load, then the slot if nothing else uses it.
  ret.getInputMutable().assign(store.getValue());
  ret->setLoc(store.getLoc());
  load.erase();
  store.erase();
  if (slot->use_empty())
    slot.erase();
}

void CIRForwardReturnStorePass::runOnOperation() {
  getOperation()->walk([](cir::FuncOp func) { forwardReturnStore(func); });
}

} // namespace

std::unique_ptr<Pass> mlir::createCIRForwardReturnStorePass() {
  return std::make_unique<CIRForwardReturnStorePass>();
}
