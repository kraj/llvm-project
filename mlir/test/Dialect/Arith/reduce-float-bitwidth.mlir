// RUN: mlir-opt %s -test-arith-reduce-float-bitwidth -split-input-file

func.func @test_constant() -> f32 {
  %0 = arith.constant 2.0 : f32
  return %0 : f32
}

// -----

func.func @test_add(%arg0: f32, %arg1: f32) -> f32 {
  %0 = arith.addf %arg0, %arg1 : f32
  return %0 : f32
}

// -----

func.func @test_add_constant(%arg0: f32) -> f32 {
  %0 = arith.constant 2.0 : f32
  %1 = arith.addf %arg0, %0 : f32
  return %1 : f32
}

// -----

func.func @test_func(%arg0: f32) -> f32 {
  return %arg0 : f32
}
