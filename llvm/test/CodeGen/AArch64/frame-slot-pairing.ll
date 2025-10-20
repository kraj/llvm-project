; RUN: llc -mtriple=aarch64-linux-gnu -verify-machineinstrs -aarch64-order-frame-objects=false < %s | FileCheck %s --check-prefix=NOOPT
; RUN: llc -mtriple=aarch64-linux-gnu -verify-machineinstrs -aarch64-order-frame-objects=true < %s | FileCheck %s --check-prefix=OPT

; Test that frame slot reordering creates more load/store pairs

define void @test_frame_pairing(ptr %p) {
; NOOPT-LABEL: test_frame_pairing:
; OPT-LABEL: test_frame_pairing:
entry:
  ; Load 16 values that will need to be spilled
  %v0 = load i64, ptr %p, align 8
  %p1 = getelementptr i64, ptr %p, i64 1
  %v1 = load i64, ptr %p1, align 8
  %p2 = getelementptr i64, ptr %p, i64 2
  %v2 = load i64, ptr %p2, align 8
  %p3 = getelementptr i64, ptr %p, i64 3
  %v3 = load i64, ptr %p3, align 8
  %p4 = getelementptr i64, ptr %p, i64 4
  %v4 = load i64, ptr %p4, align 8
  %p5 = getelementptr i64, ptr %p, i64 5
  %v5 = load i64, ptr %p5, align 8
  %p6 = getelementptr i64, ptr %p, i64 6
  %v6 = load i64, ptr %p6, align 8
  %p7 = getelementptr i64, ptr %p, i64 7
  %v7 = load i64, ptr %p7, align 8
  %p8 = getelementptr i64, ptr %p, i64 8
  %v8 = load i64, ptr %p8, align 8
  %p9 = getelementptr i64, ptr %p, i64 9
  %v9 = load i64, ptr %p9, align 8
  %p10 = getelementptr i64, ptr %p, i64 10
  %v10 = load i64, ptr %p10, align 8
  %p11 = getelementptr i64, ptr %p, i64 11
  %v11 = load i64, ptr %p11, align 8
  %p12 = getelementptr i64, ptr %p, i64 12
  %v12 = load i64, ptr %p12, align 8
  %p13 = getelementptr i64, ptr %p, i64 13
  %v13 = load i64, ptr %p13, align 8
  %p14 = getelementptr i64, ptr %p, i64 14
  %v14 = load i64, ptr %p14, align 8
  %p15 = getelementptr i64, ptr %p, i64 15
  %v15 = load i64, ptr %p15, align 8

  ; Force all values to be spilled
  call void asm sideeffect "", "~{x1},~{x2},~{x3},~{x4},~{x5},~{x6},~{x7},~{x8},~{x9},~{x10},~{x11},~{x12},~{x13},~{x14},~{x15},~{x16},~{x17},~{x18},~{x19},~{x20},~{x21},~{x22},~{x23},~{x24},~{x25},~{x26},~{x27},~{x28}"()

  ; Use values in pairs - this should make our optimization group them
  ; With optimization, these pairs should get adjacent stack slots

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add01 = add i64 %v0, %v1
  store i64 %add01, ptr %p, align 8

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add23 = add i64 %v2, %v3
  store i64 %add23, ptr %p1, align 8

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add45 = add i64 %v4, %v5
  store i64 %add45, ptr %p2, align 8

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add67 = add i64 %v6, %v7
  store i64 %add67, ptr %p3, align 8

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add89 = add i64 %v8, %v9
  store i64 %add89, ptr %p4, align 8

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add1011 = add i64 %v10, %v11
  store i64 %add1011, ptr %p5, align 8

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add1213 = add i64 %v12, %v13
  store i64 %add1213, ptr %p6, align 8

  ; OPT: ldp {{x[0-9]+}}, {{x[0-9]+}}, [sp
  %add1415 = add i64 %v14, %v15
  store i64 %add1415, ptr %p7, align 8

  ret void
}