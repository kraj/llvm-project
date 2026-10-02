// RUN: %clang_cc1 -fopenmp -emit-cir -fclangir %s -o - | FileCheck %s

void during(int);

// Decreasing loop using the common `i > 0; i--` idiom: the induction
// variable is on the left of the comparison and the increment is a plain
// decrement.
void dec_unary() {
  // CHECK: cir.func{{.*}}@{{.*}}dec_unary
#pragma omp for
  for (int i = 20; i > 0; i--) {
    during(i);
  }
  // CHECK: %[[C20_CIR:.*]] = cir.const #cir.int<20> : !s32i
  // CHECK: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
  // CHECK: %[[CM1_CIR:.*]] = cir.const #cir.int<-1> : !s32i
  // CHECK: %[[C20:.*]] = cir.builtin_int_cast %[[C20_CIR]] : !s32i -> i32
  // CHECK: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
  // CHECK: %[[CM1:.*]] = cir.builtin_int_cast %[[CM1_CIR]] : !s32i -> i32
  // CHECK: omp.wsloop {
  // CHECK-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%[[C20]]) to (%[[C0]]) step (%[[CM1]]) {
}

// Decreasing loop using a compound assignment: the step must be negated,
// not just taken verbatim from the RHS of `-=`.
void dec_compound() {
  // CHECK: cir.func{{.*}}@{{.*}}dec_compound
#pragma omp for
  for (int i = 20; i > 0; i -= 2) {
    during(i);
  }
  // CHECK: %[[C20_CIR:.*]] = cir.const #cir.int<20> : !s32i
  // CHECK: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
  // CHECK: %[[CM2_CIR:.*]] = cir.const #cir.int<-2> : !s32i
  // CHECK: %[[C20:.*]] = cir.builtin_int_cast %[[C20_CIR]] : !s32i -> i32
  // CHECK: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
  // CHECK: %[[CM2:.*]] = cir.builtin_int_cast %[[CM2_CIR]] : !s32i -> i32
  // CHECK: omp.wsloop {
  // CHECK-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%[[C20]]) to (%[[C0]]) step (%[[CM2]]) {
}

// Decreasing loop using the `var = var - incr` canonical assignment form:
// the step must be negated here too.
void dec_assign_sub() {
  // CHECK: cir.func{{.*}}@{{.*}}dec_assign_sub
#pragma omp for
  for (int i = 20; i > 0; i = i - 2) {
    during(i);
  }
  // CHECK: %[[C20_CIR:.*]] = cir.const #cir.int<20> : !s32i
  // CHECK: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
  // CHECK: %[[CM2_CIR:.*]] = cir.const #cir.int<-2> : !s32i
  // CHECK: %[[C20:.*]] = cir.builtin_int_cast %[[C20_CIR]] : !s32i -> i32
  // CHECK: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
  // CHECK: %[[CM2:.*]] = cir.builtin_int_cast %[[CM2_CIR]] : !s32i -> i32
  // CHECK: omp.wsloop {
  // CHECK-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%[[C20]]) to (%[[C0]]) step (%[[CM2]]) {
}

// Increasing loop using the `var = incr + var` canonical assignment form.
void inc_assign_add_rev() {
  // CHECK: cir.func{{.*}}@{{.*}}inc_assign_add_rev
#pragma omp for
  for (int i = 0; i < 20; i = 2 + i) {
    during(i);
  }
  // CHECK: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
  // CHECK: %[[C20_CIR:.*]] = cir.const #cir.int<20> : !s32i
  // CHECK: %[[C2_CIR:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
  // CHECK: %[[C20:.*]] = cir.builtin_int_cast %[[C20_CIR]] : !s32i -> i32
  // CHECK: %[[C2:.*]] = cir.builtin_int_cast %[[C2_CIR]] : !s32i -> i32
  // CHECK: omp.wsloop {
  // CHECK-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%[[C0]]) to (%[[C20]]) step (%[[C2]]) {
}

// Increasing loop with the upper bound written on the left of the
// comparison (`ub > var`), which is also valid canonical loop form.
void ub_on_left() {
  // CHECK: cir.func{{.*}}@{{.*}}ub_on_left
#pragma omp for
  for (int i = 0; 20 > i; i++) {
    during(i);
  }
  // CHECK: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
  // CHECK: %[[C20_CIR:.*]] = cir.const #cir.int<20> : !s32i
  // CHECK: %[[C1_CIR:.*]] = cir.const #cir.int<1> : !s32i
  // CHECK: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
  // CHECK: %[[C20:.*]] = cir.builtin_int_cast %[[C20_CIR]] : !s32i -> i32
  // CHECK: %[[C1:.*]] = cir.builtin_int_cast %[[C1_CIR]] : !s32i -> i32
  // CHECK: omp.wsloop {
  // CHECK-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%[[C0]]) to (%[[C20]]) step (%[[C1]]) {
}
