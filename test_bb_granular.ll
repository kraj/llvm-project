; Test for BB-granular stack slot pairing
; RUN: llc -mtriple=aarch64-linux-gnu < %s | FileCheck %s

define void @test_same_bb_pairing(ptr %p) {
; CHECK-LABEL: test_same_bb_pairing:
entry:
  %v0 = load i64, ptr %p
  %p1 = getelementptr i64, ptr %p, i64 1
  %v1 = load i64, ptr %p1
  %p2 = getelementptr i64, ptr %p, i64 2
  %v2 = load i64, ptr %p2
  %p3 = getelementptr i64, ptr %p, i64 3
  %v3 = load i64, ptr %p3

  ; Force spills in same BB
  call void asm sideeffect "", "~{x0},~{x1},~{x2},~{x3},~{x4},~{x5},~{x6},~{x7},~{x8},~{x9},~{x10},~{x11},~{x12},~{x13},~{x14},~{x15},~{x16},~{x17},~{x18},~{x19},~{x20},~{x21},~{x22},~{x23},~{x24},~{x25},~{x26},~{x27},~{x28}"()

  ; Use values in pairs - should create ldp
  %sum01 = add i64 %v0, %v1
  store i64 %sum01, ptr %p

  %sum23 = add i64 %v2, %v3
  store i64 %sum23, ptr %p1

  ret void
}

define void @test_different_bb_no_pairing(ptr %p, i1 %cond) {
; CHECK-LABEL: test_different_bb_no_pairing:
entry:
  %v0 = load i64, ptr %p
  %p1 = getelementptr i64, ptr %p, i64 1
  %v1 = load i64, ptr %p1

  br i1 %cond, label %bb1, label %bb2

bb1:
  ; Spill v0 in bb1
  call void asm sideeffect "", "~{x0},~{x1},~{x2},~{x3},~{x4},~{x5},~{x6},~{x7},~{x8},~{x9},~{x10},~{x11},~{x12},~{x13},~{x14},~{x15},~{x16},~{x17},~{x18}"()

  ; Use v0
  %sum0 = add i64 %v0, 1
  store i64 %sum0, ptr %p
  br label %exit

bb2:
  ; Spill v1 in bb2 (different BB - should NOT pair with v0)
  call void asm sideeffect "", "~{x0},~{x1},~{x2},~{x3},~{x4},~{x5},~{x6},~{x7},~{x8},~{x9},~{x10},~{x11},~{x12},~{x13},~{x14},~{x15},~{x16},~{x17},~{x18}"()

  ; Use v1
  %sum1 = add i64 %v1, 2
  store i64 %sum1, ptr %p1
  br label %exit

exit:
  ret void
}