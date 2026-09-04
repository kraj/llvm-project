// RUN: mlir-opt -allow-unregistered-dialect %s -split-input-file -test-greedy-patterns | FileCheck %s

// A rewrite can create an unreachable block after the iteration's initial
// unreachable-block sweep. Do not process its self-referential operation.
// CHECK-LABEL: func.func @insert_unreachable
// CHECK-NOT: arith.addi
// CHECK: return
func.func @insert_unreachable(%arg: i32) {
  "test.greedy_create_block"(%arg) {mode = "unreachable"} : (i32) -> ()
  return
}

// -----

// A block connected before the rewrite returns remains reachable.
// CHECK-LABEL: func.func @connect_before_return
// CHECK: "test.keep"()
// CHECK: return
func.func @connect_before_return() {
  "test.greedy_create_block"() {mode = "reachable"} : () -> ()
  return
}

// -----

// Inserting a new entry can disconnect the old entry without editing its
// terminator.
// CHECK-LABEL: func.func @insert_entry
// CHECK-NEXT: return
// CHECK-NEXT: }
func.func @insert_entry() {
  "test.greedy_create_block"() {mode = "entry"} : () -> ()
  return
}

// -----

// An in-place successor change can disconnect a block.
// CHECK-LABEL: func.func @redirect
// CHECK-NOT: "test.keep"
// CHECK: return
func.func @redirect() {
  "test.greedy_create_block"() {mode = "redirect"} : () -> ()
  cf.br ^body
^body:
  "test.keep"() : () -> ()
  cf.br ^exit
^exit:
  return
}
