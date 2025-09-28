; RUN: llc -mtriple=aarch64-linux-gnu -enable-early-mem-scheduler -stop-after=early-memory-scheduler < %s | FileCheck %s --check-prefix=EARLY
; RUN: llc -mtriple=aarch64-linux-gnu -enable-early-mem-scheduler=false -stop-before=machine-scheduler < %s | FileCheck %s --check-prefix=NOEARLY

; Test that the early memory scheduler actually reorders memory operations for AArch64

define i32 @test_loads_move_early(ptr %ptr1, ptr %ptr2, i32 %a, i32 %b) {
; EARLY-LABEL: name: test_loads_move_early
; EARLY: body:
; EARLY: bb.0.entry:
; With early scheduling, loads are clustered together
; EARLY: LDRWui %{{[0-9]+}}, 0 :: (load (s32) from %ir.ptr2)
; EARLY-NEXT: %{{[0-9]+}}:gpr64common = COPY
; EARLY-NEXT: LDRWui %{{[0-9]+}}, 0 :: (load (s32) from %ir.ptr1)
; EARLY: ADDWrs

; NOEARLY-LABEL: name: test_loads_move_early
; NOEARLY: body:
; NOEARLY: bb.0.entry:
; Without early scheduling, loads are separated by arithmetic
; NOEARLY: ADDWrs
; NOEARLY-NEXT: LDRWui %{{[0-9]+}}, 0 :: (load (s32) from %ir.ptr1)
; NOEARLY: MOVi32imm
; NOEARLY-NEXT: LDRWui %{{[0-9]+}}, 0 :: (load (s32) from %ir.ptr2)

entry:
  %add1 = add i32 %a, 5
  %mul1 = mul i32 %add1, 3
  %add2 = add i32 %b, 7
  %mul2 = mul i32 %add2, 11
  %load1 = load i32, ptr %ptr1, align 4
  %load2 = load i32, ptr %ptr2, align 4
  %sum1 = add i32 %mul1, %load1
  %sum2 = add i32 %sum1, %load2
  %result = add i32 %sum2, %mul2
  ret i32 %result
}

define i32 @test_same_pointer_aliasing(ptr %ptr) {
; EARLY-LABEL: name: test_same_pointer_aliasing
; EARLY: body:
; Load cannot move before store to same pointer - correctness preserved
; EARLY: STRWui %{{[0-9]+}}, %{{[0-9]+}}, 0 :: (store (s32) into %ir.ptr)
; EARLY: MOVi32imm 42

; NOEARLY-LABEL: name: test_same_pointer_aliasing
; NOEARLY: body:
; NOEARLY: STRWui %{{[0-9]+}}, %{{[0-9]+}}, 0 :: (store (s32) into %ir.ptr)

entry:
  store i32 42, ptr %ptr, align 4
  %load = load i32, ptr %ptr, align 4
  ret i32 %load
}

define i32 @test_multiple_loads_ordering(ptr %base) {
; EARLY-LABEL: name: test_multiple_loads_ordering
; EARLY: body:
; Multiple loads should be scheduled early and together
; The exact pattern depends on AArch64 load pair optimization
; EARLY: LDR{{.*}} :: (load {{.*}} from %ir.gep
; EARLY: LDR{{.*}} :: (load {{.*}} from %ir.gep

; NOEARLY-LABEL: name: test_multiple_loads_ordering
; NOEARLY: body:
; Without early scheduling, loads may be more spread out
; NOEARLY: LDR{{.*}} :: (load {{.*}} from %ir.gep

entry:
  %gep1 = getelementptr i32, ptr %base, i64 0
  %gep2 = getelementptr i32, ptr %base, i64 1
  %gep3 = getelementptr i32, ptr %base, i64 2
  %gep4 = getelementptr i32, ptr %base, i64 3

  ; Some computation between loads
  %dummy1 = add i32 5, 10
  %load1 = load i32, ptr %gep1, align 4
  %dummy2 = mul i32 %dummy1, 20
  %load2 = load i32, ptr %gep2, align 4
  %dummy3 = add i32 %dummy2, 30
  %load3 = load i32, ptr %gep3, align 4
  %dummy4 = mul i32 %dummy3, 40
  %load4 = load i32, ptr %gep4, align 4

  %sum1 = add i32 %load1, %load2
  %sum2 = add i32 %load3, %load4
  %sum3 = add i32 %sum1, %sum2
  %result = add i32 %sum3, %dummy4

  ret i32 %result
}