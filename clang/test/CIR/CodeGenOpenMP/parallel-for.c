// RUN: %clang_cc1 -fopenmp -emit-cir -fclangir %s -o - | FileCheck %s

void during(int);

// The combined `parallel for` directive decomposes into a `parallel` leaf and a
// `for` leaf. It lowers to an omp.wsloop + omp.loop_nest nested directly inside
// an omp.parallel, mirroring the separate `parallel` / `for` nesting.
void parallel_for() {
  // CHECK: cir.func{{.*}}@{{.*}}parallel_for
#pragma omp parallel for
  for (int i = 0; i < 10; i++) {
    during(i);
  }

  // CHECK: omp.parallel {

  // The induction variable alloca is emitted before the wsloop.
  // CHECK: %[[I_ALLOCA:.*]] = cir.alloca "i" align(4) init : !cir.ptr<!s32i>

  // CIR constants for the loop bounds cast to builtin integers.
  // CHECK: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
  // CHECK: %[[C10_CIR:.*]] = cir.const #cir.int<10> : !s32i
  // CHECK: %[[C1_CIR:.*]] = cir.const #cir.int<1> : !s32i
  // CHECK: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
  // CHECK: %[[C10:.*]] = cir.builtin_int_cast %[[C10_CIR]] : !s32i -> i32
  // CHECK: %[[C1:.*]] = cir.builtin_int_cast %[[C1_CIR]] : !s32i -> i32

  // CHECK: omp.wsloop {
  // CHECK-NEXT: omp.loop_nest (%[[IV:.*]]) : i32 = (%[[C0]]) to (%[[C10]]) step (%[[C1]]) {

  // CHECK: %[[IV_CIR:.*]] = cir.builtin_int_cast %[[IV]] : i32 -> !s32i
  // CHECK: cir.store align(4) %[[IV_CIR]], %[[I_ALLOCA]] : !s32i, !cir.ptr<!s32i>
  // CHECK: cir.call @{{.*}}during

  // CHECK: omp.yield
  // CHECK: }
  // CHECK: }
  // CHECK: omp.terminator
  // The parallel is a non-innermost leaf of the combined construct.
  // CHECK: } {omp.combined}
}
