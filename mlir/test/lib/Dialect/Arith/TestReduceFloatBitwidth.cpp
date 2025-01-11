//===- TestReduceFloatBitwdith.cpp - Reduce Float Bitwidth  -*- c++ -----*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A pass that reduces the bitwidth of Arith floating-point IR.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

using namespace mlir;
using namespace mlir::arith;

namespace {
struct ReduceFloatOptions {
  FloatType sourceType;
  FloatType targetType;
};

class ConstantOpPattern : public OpRewritePattern<ConstantOp> {
public:
  ConstantOpPattern(MLIRContext *context, const ReduceFloatOptions &options,
                    PatternBenefit benefit = 1)
      : OpRewritePattern(context, benefit), options(options) {}

  LogicalResult matchAndRewrite(ConstantOp op,
                                PatternRewriter &rewriter) const override {
    if (op.getType() != options.sourceType)
      return rewriter.notifyMatchFailure(op, "does not match source type");
    auto attr = cast<FloatAttr>(op.getValue());
    double val = attr.getValueAsDouble();
    auto newAttr = FloatAttr::get(options.targetType, val);
    Value newConstant = rewriter.create<ConstantOp>(op.getLoc(), newAttr);
    rewriter.replaceOpWithNewOp<ExtFOp>(op, op.getType(), newConstant);
    return success();
  }

private:
  const ReduceFloatOptions &options;
};

class AddFOpPattern : public OpRewritePattern<AddFOp> {
public:
  AddFOpPattern(MLIRContext *context, const ReduceFloatOptions &options,
                PatternBenefit benefit = 1)
      : OpRewritePattern(context, benefit), options(options) {}

  LogicalResult matchAndRewrite(AddFOp op,
                                PatternRewriter &rewriter) const override {
    if (op.getType() != options.sourceType)
      return rewriter.notifyMatchFailure(op, "does not match source type");
    Value lhsTrunc =
        rewriter.create<TruncFOp>(op.getLoc(), options.targetType, op.getLhs());
    Value rhsTrunc =
        rewriter.create<TruncFOp>(op.getLoc(), options.targetType, op.getRhs());
    Value newAdd = rewriter.create<AddFOp>(op.getLoc(), lhsTrunc, rhsTrunc);
    rewriter.replaceOpWithNewOp<ExtFOp>(op, op.getType(), newAdd);
    return success();
  }

private:
  const ReduceFloatOptions &options;
};

class FuncOpPattern : public OpRewritePattern<func::FuncOp> {
public:
  FuncOpPattern(MLIRContext *context, const ReduceFloatOptions &options,
                PatternBenefit benefit = 1)
      : OpRewritePattern(context, benefit), options(options) {}

  LogicalResult matchAndRewrite(func::FuncOp op,
                                PatternRewriter &rewriter) const override {
    if (!llvm::hasSingleElement(op.getBody()))
      return rewriter.notifyMatchFailure(op, "0 or >1 blocks not supported");
    FunctionType type = op.getFunctionType();
    SmallVector<Type> newInputs;
    for (Type t : type.getInputs()) {
      if (t == options.sourceType) {
        newInputs.push_back(options.targetType);
      } else {
        newInputs.push_back(t);
      }
    }
    SmallVector<Type> newResults;
    for (Type t : type.getResults()) {
      if (t == options.sourceType) {
        newResults.push_back(options.targetType);
      } else {
        newResults.push_back(t);
      }
    }
    if (llvm::equal(type.getInputs(), newInputs) &&
        llvm::equal(type.getResults(), newResults))
      return rewriter.notifyMatchFailure(op, "no types to convert");
    auto newFuncOp = rewriter.create<func::FuncOp>(
        op.getLoc(), op.getSymName(),
        FunctionType::get(op.getContext(), newInputs, newResults));
    SmallVector<Location> locs =
        llvm::map_to_vector(op.getBody().getArguments(),
                            [](BlockArgument arg) { return arg.getLoc(); });
    Block *newBlock = rewriter.createBlock(
        &newFuncOp.getBody(), newFuncOp.getBody().begin(), newInputs, locs);
    rewriter.setInsertionPointToStart(newBlock);
    SmallVector<Value> argRepl;
    for (auto [oldType, newType, newArg] : llvm::zip_equal(
             type.getInputs(), newInputs, newBlock->getArguments())) {
      if (oldType == newType) {
        argRepl.push_back(newArg);
      } else {
        argRepl.push_back(
            rewriter.create<ExtFOp>(newArg.getLoc(), oldType, newArg));
      }
    }
    rewriter.inlineBlockBefore(&op.getBody().front(), newBlock, newBlock->end(),
                               argRepl);
    rewriter.eraseOp(op);

    auto returnOp = cast<func::ReturnOp>(newBlock->getTerminator());
    rewriter.setInsertionPoint(returnOp);
    SmallVector<Value> resultRepl;
    for (auto [oldResult, newType] :
         llvm::zip_equal(returnOp.getOperands(), newResults)) {
      if (oldResult.getType() == newType) {
        resultRepl.push_back(oldResult);
      } else {
        resultRepl.push_back(
            rewriter.create<TruncFOp>(oldResult.getLoc(), newType, oldResult));
      }
    }
    rewriter.modifyOpInPlace(
        returnOp, [&]() { returnOp.getOperandsMutable().assign(resultRepl); });
    return success();
  }

private:
  const ReduceFloatOptions &options;
};

class ExtTruncFolding : public OpRewritePattern<TruncFOp> {
public:
  ExtTruncFolding(MLIRContext *context, const ReduceFloatOptions &options,
                  PatternBenefit benefit = 1)
      : OpRewritePattern(context, benefit), options(options) {}

  LogicalResult matchAndRewrite(TruncFOp op,
                                PatternRewriter &rewriter) const override {
    auto extfOp = op.getIn().getDefiningOp<ExtFOp>();
    if (!extfOp)
      return rewriter.notifyMatchFailure(op,
                                         "'in' is not defined by arith.extf");
    if (extfOp.getIn().getType() != op.getType())
      return rewriter.notifyMatchFailure(op, "types do not match");
    rewriter.replaceOp(op, extfOp.getIn());
    return success();
  }

private:
  const ReduceFloatOptions &options;
};

struct TestReduceFloatBitwidthPass
    : public PassWrapper<TestReduceFloatBitwidthPass, OperationPass<>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(TestReduceFloatBitwidthPass)

  TestReduceFloatBitwidthPass() = default;
  TestReduceFloatBitwidthPass(const TestReduceFloatBitwidthPass &pass)
      : PassWrapper(pass) {}

  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<arith::ArithDialect>();
  }
  StringRef getArgument() const final {
    return "test-arith-reduce-float-bitwidth";
  }
  StringRef getDescription() const final {
    return "Pass that reduces the bitwidth of floating-point ops";
  }

  void runOnOperation() override {
    MLIRContext *ctx = &getContext();
    ReduceFloatOptions options;
    options.sourceType = FloatType::getF32(ctx);
    options.targetType = FloatType::getF16(ctx);

    RewritePatternSet patterns(ctx);
    patterns.insert<ConstantOpPattern, AddFOpPattern, FuncOpPattern,
                    ExtTruncFolding>(ctx, options);

    GreedyRewriteConfig config;
    config.fold = false;
    if (failed(applyPatternsGreedily(getOperation(), std::move(patterns),
                                     config))) {
      getOperation()->emitError() << getArgument() << " failed";
      signalPassFailure();
    }
  }
};
} // namespace

namespace mlir::test {
void registerTestReduceFloatBitwidthPass() {
  PassRegistration<TestReduceFloatBitwidthPass>();
}
} // namespace mlir::test
