// REQUIRES: amdgpu-registered-target

// Host compilation (x86 host, AMDGPU offload target).
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -emit-cir -fclangir %s -o - \
// RUN:   | FileCheck %s --check-prefix=CIR-HOST

// Device compilation (AMDGPU): allocas live in the private address space.
// RUN: %clang_cc1 -triple amdgcn-amd-amdhsa -fopenmp -fopenmp-is-target-device \
// RUN:   -emit-cir -fclangir %s -o - \
// RUN:   | FileCheck %s --check-prefix=CIR-DEVICE

void during(int);

// The legal nesting of target, parallel and for lowers to an omp.wsloop +
// omp.loop_nest inside omp.parallel inside omp.target. The worksharing loop
// bounds and induction variable are cast between CIR and builtin integers
// with cir.builtin_int_cast on both the host and the GPU device.
void target_parallel_for() {
#pragma omp target
#pragma omp parallel
#pragma omp for
  for (int i = 0; i < 10; i++) {
    during(i);
  }
}

// CIR-HOST: cir.func{{.*}}@target_parallel_for
// CIR-HOST: omp.target kernel_type(generic) {
// CIR-HOST: omp.parallel {

// CIR-HOST: %[[I_ALLOCA:.*]] = cir.alloca "i" align(4) init : !cir.ptr<!s32i>
// CIR-HOST: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
// CIR-HOST: %[[C10_CIR:.*]] = cir.const #cir.int<10> : !s32i
// CIR-HOST: %[[C1_CIR:.*]] = cir.const #cir.int<1> : !s32i
// CIR-HOST: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
// CIR-HOST: %[[C10:.*]] = cir.builtin_int_cast %[[C10_CIR]] : !s32i -> i32
// CIR-HOST: %[[C1:.*]] = cir.builtin_int_cast %[[C1_CIR]] : !s32i -> i32

// CIR-HOST: omp.wsloop {
// CIR-HOST-NEXT: omp.loop_nest (%[[IV:.*]]) : i32 = (%[[C0]]) to (%[[C10]]) step (%[[C1]]) {
// CIR-HOST: %[[IV_CIR:.*]] = cir.builtin_int_cast %[[IV]] : i32 -> !s32i
// CIR-HOST: cir.store align(4) %[[IV_CIR]], %[[I_ALLOCA]] : !s32i, !cir.ptr<!s32i>
// CIR-HOST: cir.call @{{.*}}during
// CIR-HOST: omp.yield
// CIR-HOST: }
// CIR-HOST: }
// CIR-HOST: omp.terminator
// CIR-HOST: omp.terminator
// CIR-HOST: }

// CIR-DEVICE: cir.func{{.*}}@target_parallel_for
// CIR-DEVICE: omp.target kernel_type(generic) {
// CIR-DEVICE: omp.parallel {

// CIR-DEVICE: %[[I_ALLOCA:.*]] = cir.alloca "i" align(4) init : !cir.ptr<!s32i, target_address_space(5)>
// CIR-DEVICE: %[[I_CAST:.*]] = cir.cast address_space %[[I_ALLOCA]] : !cir.ptr<!s32i, target_address_space(5)> -> !cir.ptr<!s32i>
// CIR-DEVICE: %[[C0_CIR:.*]] = cir.const #cir.int<0> : !s32i
// CIR-DEVICE: %[[C10_CIR:.*]] = cir.const #cir.int<10> : !s32i
// CIR-DEVICE: %[[C1_CIR:.*]] = cir.const #cir.int<1> : !s32i
// CIR-DEVICE: %[[C0:.*]] = cir.builtin_int_cast %[[C0_CIR]] : !s32i -> i32
// CIR-DEVICE: %[[C10:.*]] = cir.builtin_int_cast %[[C10_CIR]] : !s32i -> i32
// CIR-DEVICE: %[[C1:.*]] = cir.builtin_int_cast %[[C1_CIR]] : !s32i -> i32

// CIR-DEVICE: omp.wsloop {
// CIR-DEVICE-NEXT: omp.loop_nest (%[[IV:.*]]) : i32 = (%[[C0]]) to (%[[C10]]) step (%[[C1]]) {
// CIR-DEVICE: %[[IV_CIR:.*]] = cir.builtin_int_cast %[[IV]] : i32 -> !s32i
// CIR-DEVICE: cir.store align(4) %[[IV_CIR]], %[[I_CAST]] : !s32i, !cir.ptr<!s32i>
// CIR-DEVICE: cir.call @{{.*}}during
// CIR-DEVICE: omp.yield
// CIR-DEVICE: }
// CIR-DEVICE: }
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: }

// The combined `target parallel for` directive decomposes into `target`,
// `parallel` and `for` leaves and lowers to the same nesting as the explicit
// target/parallel/for above: an omp.wsloop + omp.loop_nest inside omp.parallel
// inside omp.target.
void combined_target_parallel_for() {
#pragma omp target parallel for
  for (int i = 0; i < 10; i++) {
    during(i);
  }
}

// The `target` and `parallel` are non-innermost leaves of the combined
// construct, so both carry the omp.combined attribute (unlike the explicitly
// nested directives above).
// CIR-HOST: cir.func{{.*}}@combined_target_parallel_for
// CIR-HOST: omp.target kernel_type(generic) {
// CIR-HOST: omp.parallel {
// CIR-HOST: %[[CI_ALLOCA:.*]] = cir.alloca "i" align(4) init : !cir.ptr<!s32i>
// CIR-HOST: omp.wsloop {
// CIR-HOST-NEXT: omp.loop_nest (%[[CIV:.*]]) : i32 = (%{{.*}}) to (%{{.*}}) step (%{{.*}}) {
// CIR-HOST: cir.call @{{.*}}during
// CIR-HOST: omp.yield
// CIR-HOST: }
// CIR-HOST: }
// CIR-HOST: omp.terminator
// CIR-HOST: } {omp.combined}
// CIR-HOST: omp.terminator
// CIR-HOST: } {omp.combined}

// CIR-DEVICE: cir.func{{.*}}@combined_target_parallel_for
// CIR-DEVICE: omp.target kernel_type(generic) {
// CIR-DEVICE: omp.parallel {
// CIR-DEVICE: omp.wsloop {
// CIR-DEVICE-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%{{.*}}) to (%{{.*}}) step (%{{.*}}) {
// CIR-DEVICE: cir.call @{{.*}}during
// CIR-DEVICE: omp.yield
// CIR-DEVICE: }
// CIR-DEVICE: }
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: } {omp.combined}
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: } {omp.combined}
