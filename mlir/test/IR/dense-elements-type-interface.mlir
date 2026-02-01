// RUN: mlir-opt -allow-unregistered-dialect %s | mlir-opt -allow-unregistered-dialect | FileCheck %s

// Test dense elements attribute with custom element type using DenseElementTypeInterface.
// Uses the new type-first syntax: dense<TYPE : [ATTR, ...]>
// Note: The type is embedded in the attribute, so it's not printed again at the end.

// CHECK-LABEL: func @dense_custom_element_type
func.func @dense_custom_element_type() {
  // The type is embedded in the dense attribute syntax, not printed separately.
  // CHECK: "unregistered_op"() {attr = dense<tensor<3x!test.dense_element> : [1 : i32, 2 : i32, 3 : i32]>}
  "unregistered_op"() {attr = dense<tensor<3x!test.dense_element> : [1 : i32, 2 : i32, 3 : i32]>} : () -> ()
  return
}

// CHECK-LABEL: func @dense_custom_element_type_2d
func.func @dense_custom_element_type_2d() {
  // CHECK: "unregistered_op"() {attr = dense<tensor<2x2x!test.dense_element> : {{\[}}{{\[}}1 : i32, 2 : i32], [3 : i32, 4 : i32]]>}
  "unregistered_op"() {attr = dense<tensor<2x2x!test.dense_element> : [[1 : i32, 2 : i32], [3 : i32, 4 : i32]]>} : () -> ()
  return
}

// CHECK-LABEL: func @dense_custom_element_splat
func.func @dense_custom_element_splat() {
  // A splat should be detected and stored efficiently
  // CHECK: "unregistered_op"() {attr = dense<tensor<4x!test.dense_element> : 42 : i32>}
  "unregistered_op"() {attr = dense<tensor<4x!test.dense_element> : 42 : i32>} : () -> ()
  return
}
