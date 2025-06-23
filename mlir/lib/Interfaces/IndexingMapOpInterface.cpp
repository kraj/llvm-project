//===- IndexingMapOpInterface.cpp -- IndexingMapOpInterface impl ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mlir/Interfaces/IndexingMapOpInterface.h"

using namespace mlir;

namespace mlir {
#include "mlir/Interfaces/IndexingMapOpInterface.cpp.inc"
} // namespace mlir

LogicalResult mlir::detail::verifyIndexingMapOpInterface(Operation *op) {
  auto imOp = cast<IndexingMapOpInterface>(op);

  // Check if given shapes match to inferred shapes.
  SmallVector<int64_t, 4> endLoopRangeValues = imOp.getStaticLoopRanges();
  SmallVector<int64_t, 4> startLoopRangeValues(endLoopRangeValues.size(), 0);
  // Verify only static cases since we can't get exact dimension sizes and
  // loop ranges for dynamic cases in this stage.
  if (llvm::none_of(endLoopRangeValues, ShapedType::isDynamic)) {
    // Exclusive end range.
    for (int64_t &range : endLoopRangeValues)
      range -= 1;
    for (OpOperand &opOperand : imOp->getOpOperands()) {
      AffineMap indexingMap = imOp.getMatchingIndexingMap(&opOperand);
      SmallVector<int64_t, 4> startIndices =
          indexingMap.compose(startLoopRangeValues);
      SmallVector<int64_t, 4> endIndices =
          indexingMap.compose(endLoopRangeValues);
      SmallVector<int64_t> shape = imOp.getStaticOperandShape(&opOperand);
      for (auto dim : llvm::seq<int64_t>(0, shape.size())) {
        // Ignore dynamic dimension or the case that the dimension size is 0
        if (ShapedType::isDynamic(shape[dim]) || shape[dim] == 0)
          continue;

        // The first index or last index should be the maximum or the minimum in
        // the inferred index ranges since the range is increasing or
        // decreasing. The size of dimensions of input/output operands and the
        // maximum value + 1 in the inferred range should be the same. But, for
        // now we check if the inferred ranges are in boundary of input/output
        // operands' size or not in case that Affine Expressions are complicated
        // such as d0 * 3
        // + d1 since it is not easy to handle the issues.
        // Found the case that this solution can't check, for example, (d0, d1)
        // -> (d1 - d0)
        int64_t inferredDimSize =
            std::max(startIndices[dim], endIndices[dim]) + 1;
        if (std::min(startIndices[dim], endIndices[dim]) < 0) {
          std::string mapStr;
          {
            llvm::raw_string_ostream os(mapStr);
            os << indexingMap;
          }
          return op->emitOpError(
                     "unexpected result less than 0 at expression #")
                 << dim << " in " << mapStr;
        }
        if (isa<AffineDimExpr>(indexingMap.getResult(dim))) {
          if (inferredDimSize != shape[dim]) {
            return op->emitOpError("inferred input/output operand #")
                   << opOperand.getOperandNumber() << " has shape's dimension #"
                   << dim << " to be " << inferredDimSize << ", but found "
                   << shape[dim];
          }
        } else {
          if (inferredDimSize > shape[dim]) {
            return op->emitOpError("inferred input/output operand #")
                   << opOperand.getOperandNumber() << " has shape's dimension #"
                   << dim << " to be greater than or equal to "
                   << inferredDimSize << ", but found " << shape[dim];
          }
        }
      }
    }
  }

  return success();
}
