//===- PassPipelineTest.cpp -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Tests for the HLFIR extension points of the HLFIR-to-FIR pass pipeline, and
// for the config augmentor registry that -load'ed plugins use to reach them.
//
// The callbacks run when the pipeline is built, so no IR is needed: building
// the pipeline is enough to observe them.
//
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "flang/Optimizer/Passes/Pipelines.h"
#include "flang/Tools/CrossToolHelpers.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <vector>

namespace {

/// A no-op pass, identifiable by name in a textual pipeline, used to locate an
/// extension point.
struct MarkerPass : public mlir::PassWrapper<MarkerPass,
                        mlir::OperationPass<mlir::ModuleOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(MarkerPass)

  llvm::StringRef getArgument() const override { return "ep-marker"; }
  llvm::StringRef getDescription() const override {
    return "No-op pass used to locate an extension point in a pipeline";
  }
  void runOnOperation() override {}
};

/// Render \p pm as a textual pipeline.
std::string pipelineAsString(mlir::PassManager &pm) {
  std::string out;
  llvm::raw_string_ostream os(out);
  pm.printAsTextualPipeline(os);
  return out;
}

/// Build a HLFIR-to-FIR pipeline at \p level with a MarkerPass injected at each
/// requested extension point, and return it as a textual pipeline.
std::string pipelineWithMarkers(
    llvm::OptimizationLevel level, bool early, bool last) {
  mlir::MLIRContext context;
  mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
  MLIRToLLVMPassPipelineConfig config(level);
  if (early) {
    config.registerHLFIROptEarlyEPCallbacks(
        [](mlir::PassManager &nestedPm, llvm::OptimizationLevel) {
          nestedPm.addPass(std::make_unique<MarkerPass>());
        });
  }
  if (last) {
    config.registerHLFIROptLastEPCallbacks(
        [](mlir::PassManager &nestedPm, llvm::OptimizationLevel) {
          nestedPm.addPass(std::make_unique<MarkerPass>());
        });
  }
  fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);
  return pipelineAsString(pm);
}

// Both callbacks are invoked, Early before Last.
TEST(HLFIRExtensionPoint, CallbacksAreInvokedInOrder) {
  mlir::MLIRContext context;
  mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
  MLIRToLLVMPassPipelineConfig config(llvm::OptimizationLevel::O2);

  std::vector<std::string> order;
  size_t earlySizeAtCall = ~size_t{0}; // sentinel

  config.registerHLFIROptEarlyEPCallbacks(
      [&](mlir::PassManager &nestedPm, llvm::OptimizationLevel) {
        order.push_back("early");
        earlySizeAtCall = nestedPm.size();
      });
  config.registerHLFIROptLastEPCallbacks(
      [&](mlir::PassManager &, llvm::OptimizationLevel) {
        order.push_back("last");
      });

  fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);

  ASSERT_EQ(order.size(), 2u);
  EXPECT_EQ(order[0], "early");
  EXPECT_EQ(order[1], "last");
  // Early runs before anything has been added to the pipeline.
  EXPECT_EQ(earlySizeAtCall, 0u);
  EXPECT_GT(pm.size(), 0u);
}

// The callbacks fire at every level, including O0 where the simplification
// passes are skipped.
TEST(HLFIRExtensionPoint, CallbacksAreInvokedAtEveryOptLevel) {
  for (llvm::OptimizationLevel level :
      {llvm::OptimizationLevel::O0, llvm::OptimizationLevel::O1,
          llvm::OptimizationLevel::O2, llvm::OptimizationLevel::O3}) {
    mlir::MLIRContext context;
    mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
    MLIRToLLVMPassPipelineConfig config(level);

    int earlyCount = 0;
    int lastCount = 0;
    llvm::OptimizationLevel seenLevel = llvm::OptimizationLevel::O0;
    config.registerHLFIROptEarlyEPCallbacks(
        [&](mlir::PassManager &, llvm::OptimizationLevel cbLevel) {
          ++earlyCount;
          seenLevel = cbLevel;
        });
    config.registerHLFIROptLastEPCallbacks(
        [&](mlir::PassManager &, llvm::OptimizationLevel) { ++lastCount; });

    fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);

    EXPECT_EQ(earlyCount, 1);
    EXPECT_EQ(lastCount, 1);
    // The callback is handed the level the pipeline was configured with.
    EXPECT_EQ(seenLevel, level);
  }
}

// Early must precede simplify-hlfir-intrinsics and Last must sit between it and
// lower-hlfir-intrinsics. Asserting on the textual pipeline means a reordering
// of createHLFIRToFIRPassPipeline breaks this test.
TEST(HLFIRExtensionPoint, MarkersAreAtTheDocumentedPositions) {
  std::string pipeline = pipelineWithMarkers(llvm::OptimizationLevel::O2,
      /*early=*/true, /*last=*/true);

  size_t earlyMarker = pipeline.find("ep-marker");
  ASSERT_NE(earlyMarker, std::string::npos) << pipeline;
  size_t lastMarker = pipeline.find("ep-marker", earlyMarker + 1);
  ASSERT_NE(lastMarker, std::string::npos) << pipeline;

  size_t simplify = pipeline.find("simplify-hlfir-intrinsics");
  ASSERT_NE(simplify, std::string::npos) << pipeline;
  size_t lowerIntrinsics = pipeline.find("lower-hlfir-intrinsics");
  ASSERT_NE(lowerIntrinsics, std::string::npos) << pipeline;

  EXPECT_LT(earlyMarker, simplify) << pipeline;
  EXPECT_GT(lastMarker, simplify) << pipeline;
  EXPECT_LT(lastMarker, lowerIntrinsics) << pipeline;
}

// At O0 the simplification passes are absent, but Last must still precede
// lower-hlfir-intrinsics.
TEST(HLFIRExtensionPoint, LastMarkerPrecedesLoweringAtO0) {
  std::string pipeline = pipelineWithMarkers(llvm::OptimizationLevel::O0,
      /*early=*/false, /*last=*/true);

  size_t marker = pipeline.find("ep-marker");
  ASSERT_NE(marker, std::string::npos) << pipeline;
  size_t lowerIntrinsics = pipeline.find("lower-hlfir-intrinsics");
  ASSERT_NE(lowerIntrinsics, std::string::npos) << pipeline;
  EXPECT_LT(marker, lowerIntrinsics) << pipeline;
}

// A callback may add passes at the extension point.
TEST(HLFIRExtensionPoint, CallbackCanAddPasses) {
  mlir::MLIRContext context;
  mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
  MLIRToLLVMPassPipelineConfig config(llvm::OptimizationLevel::O0);

  size_t sizeBefore = ~size_t{0};
  size_t sizeAfter = ~size_t{0};
  config.registerHLFIROptEarlyEPCallbacks(
      [&](mlir::PassManager &nestedPm, llvm::OptimizationLevel) {
        sizeBefore = nestedPm.size();
        nestedPm.addPass(mlir::createCanonicalizerPass());
        sizeAfter = nestedPm.size();
      });

  fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);

  EXPECT_EQ(sizeBefore, 0u);
  EXPECT_EQ(sizeAfter, 1u);
}

// Callbacks at the same extension point run in registration order.
TEST(HLFIRExtensionPoint, MultipleCallbacksRunInRegistrationOrder) {
  mlir::MLIRContext context;
  mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
  MLIRToLLVMPassPipelineConfig config(llvm::OptimizationLevel::O2);

  std::vector<int> order;
  for (int i = 0; i < 3; ++i) {
    config.registerHLFIROptEarlyEPCallbacks(
        [&order, i](mlir::PassManager &, llvm::OptimizationLevel) {
          order.push_back(i);
        });
  }

  fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);

  EXPECT_EQ(order, (std::vector<int>{0, 1, 2}));
}

// With no callbacks registered the pipeline is unchanged.
TEST(HLFIRExtensionPoint, NoCallbacksIsNoOp) {
  mlir::MLIRContext context;
  mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
  MLIRToLLVMPassPipelineConfig config(llvm::OptimizationLevel::O2);

  fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);

  EXPECT_GT(pm.size(), 0u);
  EXPECT_EQ(pipelineAsString(pm),
      pipelineWithMarkers(llvm::OptimizationLevel::O2, /*early=*/false,
          /*last=*/false));
}

// The registry is process-global and append-only, with no way to unregister, so
// a callback capturing a local by reference would be re-invoked from a later
// test with the referent destroyed. The tests record into this
// process-lifetime recorder instead, and each asserts only on the markers it
// wrote, so they do not depend on execution order.
struct AugmentorRecorder {
  std::vector<std::string> order;
  MLIRToLLVMPassPipelineConfig *seenConfig = nullptr;
  bool epRan = false;

  void reset() {
    order.clear();
    seenConfig = nullptr;
    epRan = false;
  }
  /// Index of \p marker in `order`, or npos.
  size_t indexOf(llvm::StringRef marker) const {
    for (size_t i = 0, e = order.size(); i != e; ++i)
      if (order[i] == marker)
        return i;
    return std::string::npos;
  }
};

AugmentorRecorder &recorder() {
  static AugmentorRecorder r;
  return r;
}

TEST(PassPipelineConfigCallback, CallbacksRunInRegistrationOrderOnTheConfig) {
  fir::registerPassPipelineConfigCallback(
      [](MLIRToLLVMPassPipelineConfig &config) {
        recorder().order.push_back("order-first");
        recorder().seenConfig = &config;
      });
  fir::registerPassPipelineConfigCallback([](MLIRToLLVMPassPipelineConfig &) {
    recorder().order.push_back("order-second");
  });

  recorder().reset();
  MLIRToLLVMPassPipelineConfig config(llvm::OptimizationLevel::O2);
  fir::invokePassPipelineConfigCallbacks(config);

  size_t first = recorder().indexOf("order-first");
  size_t second = recorder().indexOf("order-second");
  ASSERT_NE(first, std::string::npos);
  ASSERT_NE(second, std::string::npos);
  EXPECT_LT(first, second);
  // The callback receives the config the pipeline will be built from.
  EXPECT_EQ(recorder().seenConfig, &config);
}

// The plugin shape: the augmentor registers an extension-point callback, which
// then contributes a pass when the pipeline is built.
TEST(PassPipelineConfigCallback, CanRegisterHLFIRExtensionPoints) {
  fir::registerPassPipelineConfigCallback(
      [](MLIRToLLVMPassPipelineConfig &config) {
        config.registerHLFIROptEarlyEPCallbacks(
            [](mlir::PassManager &pm, llvm::OptimizationLevel) {
              recorder().epRan = true;
              pm.addPass(std::make_unique<MarkerPass>());
            });
      });

  recorder().reset();
  mlir::MLIRContext context;
  mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
  MLIRToLLVMPassPipelineConfig config(llvm::OptimizationLevel::O2);
  fir::invokePassPipelineConfigCallbacks(config);
  fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);

  EXPECT_TRUE(recorder().epRan);
  EXPECT_NE(pipelineAsString(pm).find("ep-marker"), std::string::npos);
}

// A config never handed to invokePassPipelineConfigCallbacks is unaffected,
// which is what keeps bbc and tco out of the registry.
TEST(PassPipelineConfigCallback, NotInvokedMeansNoEffect) {
  fir::registerPassPipelineConfigCallback(
      [](MLIRToLLVMPassPipelineConfig &config) {
        config.registerHLFIROptEarlyEPCallbacks(
            [](mlir::PassManager &pm, llvm::OptimizationLevel) {
              pm.addPass(std::make_unique<MarkerPass>());
            });
      });

  recorder().reset();
  mlir::MLIRContext context;
  mlir::PassManager pm(&context, mlir::ModuleOp::getOperationName());
  MLIRToLLVMPassPipelineConfig config(llvm::OptimizationLevel::O2);
  fir::createHLFIRToFIRPassPipeline(pm, fir::EnableOpenMP::None, config);

  EXPECT_EQ(pipelineAsString(pm).find("ep-marker"), std::string::npos);
}

} // namespace
