# Exclude tests from libMLIR.so
add_mlir_library(MLIRArithTestPasses
  TestEmulateWideInt.cpp
  TestReduceFloatBitwidth.cpp

  EXCLUDE_FROM_LIBMLIR

  LINK_LIBS PUBLIC
  MLIRArithDialect
  MLIRArithTransforms
  MLIRFuncDialect
  MLIRLLVMDialect
  MLIRPass
  MLIRVectorDialect
)
