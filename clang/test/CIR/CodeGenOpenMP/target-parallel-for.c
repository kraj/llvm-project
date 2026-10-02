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
// omp.loop_nest inside omp.parallel inside omp.target. This explicit nesting
// is equivalent to the combined `target parallel for` spelling below, so it
// is likewise a target SPMD construct: the omp.target is marked
// kernel_type(spmd), its loop trip count is evaluated on the host and
// forwarded through host_eval block arguments, and the omp.loop_nest bounds
// reference those block arguments. The bounds themselves are cast between
// CIR and builtin integers with cir.builtin_int_cast on both the host and
// the GPU device.
void target_parallel_for() {
#pragma omp target
#pragma omp parallel
#pragma omp for
  for (int i = 0; i < 10; i++) {
    during(i);
  }
}

// CIR-HOST: cir.func{{.*}}@target_parallel_for
// CIR-HOST: %[[LB:.*]] = cir.builtin_int_cast %{{.*}} : !s32i -> i32
// CIR-HOST: %[[UB:.*]] = cir.builtin_int_cast %{{.*}} : !s32i -> i32
// CIR-HOST: %[[STEP:.*]] = cir.builtin_int_cast %{{.*}} : !s32i -> i32
// CIR-HOST: omp.target kernel_type(spmd) host_eval(%[[LB]] -> %[[ALB:.*]], %[[UB]] -> %[[AUB:.*]], %[[STEP]] -> %[[ASTEP:.*]] : i32, i32, i32) {
// CIR-HOST: omp.parallel {

// CIR-HOST: %[[I_ALLOCA:.*]] = cir.alloca "i" align(4) init : !cir.ptr<!s32i>

// CIR-HOST: omp.wsloop {
// CIR-HOST-NEXT: omp.loop_nest (%[[IV:.*]]) : i32 = (%[[ALB]]) to (%[[AUB]]) step (%[[ASTEP]]) {
// CIR-HOST: %[[IV_CIR:.*]] = cir.builtin_int_cast %[[IV]] : i32 -> !s32i
// CIR-HOST: cir.store align(4) %[[IV_CIR]], %[[I_ALLOCA]] : !s32i, !cir.ptr<!s32i>
// CIR-HOST: cir.call @{{.*}}during
// CIR-HOST: omp.yield
// CIR-HOST: }
// CIR-HOST: }
// CIR-HOST: omp.terminator
// CIR-HOST: } {omp.combined}
// CIR-HOST: omp.terminator
// CIR-HOST: } {omp.combined}

// CIR-DEVICE: cir.func{{.*}}@target_parallel_for
// CIR-DEVICE: omp.target kernel_type(spmd) host_eval(%{{.*}} -> %[[ALB:.*]], %{{.*}} -> %[[AUB:.*]], %{{.*}} -> %[[ASTEP:.*]] : i32, i32, i32) {
// CIR-DEVICE: omp.parallel {

// CIR-DEVICE: %[[I_ALLOCA:.*]] = cir.alloca "i" align(4) init : !cir.ptr<!s32i, target_address_space(5)>
// CIR-DEVICE: %[[I_CAST:.*]] = cir.cast address_space %[[I_ALLOCA]] : !cir.ptr<!s32i, target_address_space(5)> -> !cir.ptr<!s32i>

// CIR-DEVICE: omp.wsloop {
// CIR-DEVICE-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%[[ALB]]) to (%[[AUB]]) step (%[[ASTEP]]) {
// CIR-DEVICE: %[[IV_CIR:.*]] = cir.builtin_int_cast %{{.*}} : i32 -> !s32i
// CIR-DEVICE: cir.store align(4) %[[IV_CIR]], %[[I_CAST]] : !s32i, !cir.ptr<!s32i>
// CIR-DEVICE: cir.call @{{.*}}during
// CIR-DEVICE: omp.yield
// CIR-DEVICE: }
// CIR-DEVICE: }
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: } {omp.combined}
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: } {omp.combined}

// The combined `target parallel for` directive decomposes into `target`,
// `parallel` and `for` leaves and lowers to the same nesting as the explicit
// target/parallel/for above: an omp.wsloop + omp.loop_nest inside omp.parallel
// inside omp.target. This is a target SPMD construct, so the omp.target is
// marked kernel_type(spmd), its loop trip count is evaluated on the host and
// forwarded through host_eval block arguments, and the omp.loop_nest bounds
// reference those block arguments.
void combined_target_parallel_for() {
#pragma omp target parallel for
  for (int i = 0; i < 10; i++) {
    during(i);
  }
}

// The `target` and `parallel` are non-innermost leaves of the combined
// construct, so both carry the omp.combined attribute, just like the
// explicitly nested directives above.
// CIR-HOST: cir.func{{.*}}@combined_target_parallel_for
// CIR-HOST: %[[LB:.*]] = cir.builtin_int_cast %{{.*}} : !s32i -> i32
// CIR-HOST: %[[UB:.*]] = cir.builtin_int_cast %{{.*}} : !s32i -> i32
// CIR-HOST: %[[STEP:.*]] = cir.builtin_int_cast %{{.*}} : !s32i -> i32
// CIR-HOST: omp.target kernel_type(spmd) host_eval(%[[LB]] -> %[[ALB:.*]], %[[UB]] -> %[[AUB:.*]], %[[STEP]] -> %[[ASTEP:.*]] : i32, i32, i32) {
// CIR-HOST: omp.parallel {
// CIR-HOST: %[[CI_ALLOCA:.*]] = cir.alloca "i" align(4) init : !cir.ptr<!s32i>
// CIR-HOST: omp.wsloop {
// CIR-HOST-NEXT: omp.loop_nest (%[[CIV:.*]]) : i32 = (%[[ALB]]) to (%[[AUB]]) step (%[[ASTEP]]) {
// CIR-HOST: cir.call @{{.*}}during
// CIR-HOST: omp.yield
// CIR-HOST: }
// CIR-HOST: }
// CIR-HOST: omp.terminator
// CIR-HOST: } {omp.combined}
// CIR-HOST: omp.terminator
// CIR-HOST: } {omp.combined}

// CIR-DEVICE: cir.func{{.*}}@combined_target_parallel_for
// CIR-DEVICE: omp.target kernel_type(spmd) host_eval(%{{.*}} -> %[[ALB:.*]], %{{.*}} -> %[[AUB:.*]], %{{.*}} -> %[[ASTEP:.*]] : i32, i32, i32) {
// CIR-DEVICE: omp.parallel {
// CIR-DEVICE: omp.wsloop {
// CIR-DEVICE-NEXT: omp.loop_nest (%{{.*}}) : i32 = (%[[ALB]]) to (%[[AUB]]) step (%[[ASTEP]]) {
// CIR-DEVICE: cir.call @{{.*}}during
// CIR-DEVICE: omp.yield
// CIR-DEVICE: }
// CIR-DEVICE: }
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: } {omp.combined}
// CIR-DEVICE: omp.terminator
// CIR-DEVICE: } {omp.combined}
