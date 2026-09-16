// RUN: %clang_cc1 -verify -fopenmp -fopenmp-version=52 \
// RUN:   -triple x86_64-unknown-linux -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -x c++ -verify -fopenmp -fopenmp-version=52 \
// RUN:   -triple x86_64-unknown-linux -emit-llvm %s -o - | FileCheck %s
// expected-no-diagnostics

#ifdef __cplusplus
extern "C" {
#endif

#pragma omp begin declare target
void cpu_variant(void);
void scored_variant(void);
void parallel_variant(void);

#pragma omp declare variant(cpu_variant) match(device = {kind(cpu)})
#pragma omp declare variant(scored_variant) \
    match(implementation = {vendor(score(3) : llvm)})
void target_base(void);

#pragma omp declare variant(cpu_variant) match(device = {kind(cpu)})
#pragma omp declare variant(parallel_variant) match(construct = {parallel})
void depth_base(void);

#pragma omp declare variant(parallel_variant) match(construct = {parallel})
void construct_base(void);
#pragma omp end declare target

// With only TARGET in the context, CPU scores 3 and the explicit score is 4.
void target_only(void) {
#pragma omp target
  { target_base(); }
}
// CHECK-LABEL: define internal void @__omp_offloading_{{.*}}target_only
// CHECK: call void @scored_variant()
// CHECK: ret void

// The outer PARALLEL must not increase the device score inside TARGET.
void parallel_target(void) {
#pragma omp parallel
  {
#pragma omp target
    { target_base(); }
  }
}
// CHECK-LABEL: define internal void @__omp_offloading_{{.*}}parallel_target
// CHECK: call void @scored_variant()
// CHECK: ret void

// In PARALLEL, CPU scores 3 and construct={parallel} scores 2.
void parallel_depth(void) {
#pragma omp parallel
  { depth_base(); }
}
// CHECK-LABEL: define internal void @parallel_depth.omp_outlined
// CHECK: call void @cpu_variant()
// CHECK: ret void

// TARGET is retained, as are constructs nested inside it: CPU scores 5.
void target_parallel(void) {
#pragma omp target
  {
#pragma omp parallel
    { target_base(); }
  }
}
// CHECK-LABEL: define internal void @__omp_offloading_{{.*}}target_parallel
// CHECK: define internal void @{{.*}}omp_outlined
// CHECK: call void @cpu_variant()
// CHECK: ret void

// Leaving TARGET restores the enclosing PARALLEL context.
void restore_context(void) {
#pragma omp parallel
  {
#pragma omp target
    { target_base(); }
    depth_base();
    construct_base();
  }
}
// CHECK-LABEL: define internal void @restore_context.omp_outlined
// CHECK: call void @cpu_variant()
// CHECK: call void @parallel_variant()
// CHECK: ret void
// CHECK-LABEL: define internal void @__omp_offloading_{{.*}}restore_context
// CHECK: call void @scored_variant()
// CHECK: ret void

#ifdef __cplusplus
}
#endif
