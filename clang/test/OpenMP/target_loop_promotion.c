// REQUIRES: amdgpu-registered-target

// RUN: %clang_cc1 -verify -fopenmp -x c -triple x86_64-unknown-linux-gnu \
// RUN:   -fopenmp-targets=amdgcn-amd-amdhsa -emit-llvm-bc %s -o %t-host.bc

// RUN: %clang_cc1 -verify -fopenmp -x c -triple amdgcn-amd-amdhsa \
// RUN:   -fopenmp-targets=amdgcn-amd-amdhsa -fopenmp-is-target-device \
// RUN:   -fopenmp-host-ir-file-path %t-host.bc \
// RUN:   -fopenmp-assume-teams-oversubscription \
// RUN:   -fopenmp-assume-threads-oversubscription \
// RUN:   -emit-llvm %s -o - | FileCheck %s \
// RUN:   --check-prefixes=CHECK,NOLOOP

// RUN: %clang_cc1 -fopenmp -x c -triple amdgcn-amd-amdhsa \
// RUN:   -fopenmp-targets=amdgcn-amd-amdhsa -fopenmp-is-target-device \
// RUN:   -fopenmp-host-ir-file-path %t-host.bc \
// RUN:   -fopenmp-assume-teams-oversubscription \
// RUN:   -fopenmp-assume-threads-oversubscription \
// RUN:   -emit-pch %s -o %t.pch

// RUN: %clang_cc1 -verify -fopenmp -x c -triple amdgcn-amd-amdhsa \
// RUN:   -fopenmp-targets=amdgcn-amd-amdhsa -fopenmp-is-target-device \
// RUN:   -fopenmp-host-ir-file-path %t-host.bc \
// RUN:   -fopenmp-assume-teams-oversubscription \
// RUN:   -fopenmp-assume-threads-oversubscription \
// RUN:   -include-pch %t.pch -emit-llvm %s -o - | FileCheck %s \
// RUN:   --check-prefixes=CHECK,NOLOOP

// RUN: %clang_cc1 -verify -fopenmp -x c -triple amdgcn-amd-amdhsa \
// RUN:   -fopenmp-targets=amdgcn-amd-amdhsa -fopenmp-is-target-device \
// RUN:   -fopenmp-host-ir-file-path %t-host.bc \
// RUN:   -emit-llvm %s -o - | FileCheck %s --check-prefixes=CHECK,STRIDED

// RUN: %clang_cc1 -verify -fopenmp -x c -triple amdgcn-amd-amdhsa \
// RUN:   -fopenmp-targets=amdgcn-amd-amdhsa -fopenmp-is-target-device \
// RUN:   -fopenmp-host-ir-file-path %t-host.bc \
// RUN:   -fopenmp-assume-teams-oversubscription \
// RUN:   -emit-llvm %s -o - | FileCheck %s --check-prefixes=CHECK,STRIDED

// RUN: %clang_cc1 -verify -fopenmp -x c -triple amdgcn-amd-amdhsa \
// RUN:   -fopenmp-targets=amdgcn-amd-amdhsa -fopenmp-is-target-device \
// RUN:   -fopenmp-host-ir-file-path %t-host.bc \
// RUN:   -fopenmp-assume-threads-oversubscription \
// RUN:   -emit-llvm %s -o - | FileCheck %s --check-prefixes=CHECK,STRIDED

// expected-no-diagnostics

#ifndef HEADER
#define HEADER

int foo(int i);

void promotable_no_loop_combined_directive(int *array) {
#pragma omp target teams distribute parallel for
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams distribute parallel for simd
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams distribute parallel for nowait
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams loop
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
}

void promotable_no_loop_split_directive(int *array) {
#pragma omp target teams
  {
#pragma omp distribute parallel for
    for (int i = 0; i < 1024; ++i)
      array[i] = i + 1;
  }
#pragma omp target
  {
#pragma omp teams
    {
#pragma omp distribute parallel for
      for (int i = 0; i < 1024; ++i)
        array[i] = i + 1;
    }
  }
#pragma omp target
  {
#pragma omp teams loop
    for (int i = 0; i < 1024; ++i)
      array[i] = i + 1;
  }
}

void promotable_no_loop_lastprivate(int *array) {
  {
    int i;
#pragma omp target teams distribute parallel for lastprivate(i)
    for (i = 0; i < 1024; ++i)
      array[i] = i + 1;
  }
  {
    int last = 0;
#pragma omp target teams distribute parallel for lastprivate(last)
    for (int i = 0; i < 1024; ++i) {
      array[i] = i + 1;
      last = i;
    }
  }
  {
    int last = 0;
#pragma omp target teams distribute parallel for lastprivate(last) nowait
    for (int i = 0; i < 1024; ++i) {
      array[i] = i + 1;
      last = i;
    }
  }
}

void promotable_no_loop_schedule(int *array) {
#pragma omp target teams distribute parallel for schedule(static, 1)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams distribute parallel for schedule(auto)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
}

void promotable_no_loop_if(int *array, int c) {
#pragma omp target teams distribute parallel for if(parallel : 1)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams distribute parallel for if(target : c)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
}

void promotable_strided_combined_directive(int *array) {
#pragma omp target teams distribute parallel for num_teams(3)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
}

void promotable_strided_split_directive(int *array) {
#pragma omp target teams num_teams(3)
  {
#pragma omp distribute parallel for
    for (int i = 0; i < 1024; ++i)
      array[i] = i + 1;
  }
}

void non_promotable_combined_directive(int *array) {
#pragma omp target teams distribute parallel for collapse(2)
  for (int i = 0; i < 32; ++i)
    for (int j = 0; j < 32; ++j)
      array[i * 32 + j] = i + j;
#pragma omp target teams distribute parallel for
  for (int i = 0; i < 1024; ++i) {
    array[i] = i + 1;
#pragma omp cancel for
  }
}

void non_promotable_split_directive(int *array) {
#pragma omp target
  {
#pragma omp teams loop
    for (int i = 0; i < 1024; ++i)
      array[i] = foo(i);
  }
}

void non_promotable_schedule(int *array) {
#pragma omp target teams distribute parallel for schedule(static)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams distribute parallel for schedule(guided)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams distribute parallel for dist_schedule(static)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
}

void non_promotable_if(int *array, int c) {
#pragma omp target teams distribute parallel for if(parallel : c)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams distribute parallel for if(c)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
#pragma omp target teams
  {
#pragma omp distribute parallel for if(parallel : c)
    for (int i = 0; i < 1024; ++i)
      array[i] = i + 1;
  }
#pragma omp target teams loop if(c)
  for (int i = 0; i < 1024; ++i)
    array[i] = i + 1;
}

#endif

// NOLOOP-COUNT-14: promotable_no_loop_{{.*}}_kernel_environment {{.*}} i8 0, i8 1, i8 6
// NOLOOP-COUNT-2: promotable_strided_{{.*}}_kernel_environment {{.*}} i8 0, i8 1, i8 10
// NOLOOP-COUNT-10: non_promotable_{{.*}}_kernel_environment {{.*}} i8 0, i8 1, i8 2

// STRIDED-COUNT-14: promotable_no_loop_{{.*}}_kernel_environment {{.*}} i8 0, i8 1, i8 10
// STRIDED-COUNT-2: promotable_strided_{{.*}}_kernel_environment {{.*}} i8 0, i8 1, i8 10
// STRIDED-COUNT-10: non_promotable_{{.*}}_kernel_environment {{.*}} i8 0, i8 1, i8 2

// no clause
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_combined_directive_l54{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l54{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l54{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// simd
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_combined_directive_l57{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: store i32 1024, ptr %i
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l57{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l57{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// nowait
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_combined_directive_l60{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l60{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l60{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.exit:
// CHECK-NEXT: br label %omp_loop.after
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// target teams loop
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_combined_directive_l63{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l63{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_combined_directive_l63{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// target teams + distribute parallel for
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_split_directive_l69{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_split_directive_l69{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_split_directive_l69{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// target + teams + distribute parallel for
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_split_directive_l75{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_split_directive_l75{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_split_directive_l75{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// target + teams loop
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_split_directive_l84{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_split_directive_l84{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_split_directive_l84{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// loop counter
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_lastprivate_l95{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: store i32 1024, ptr %i
// CHECK-NEXT: @__kmpc_free_shared(ptr %i{{.*}})
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_lastprivate_l95{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_lastprivate_l95{{.*}}, i32 0, i32 0, i8 0)
// CHECK: @__kmpc_barrier
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// scalar
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_lastprivate_l101{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: @__kmpc_free_shared(ptr %last{{.*}})
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_lastprivate_l101{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_lastprivate_l101{{.*}}, i32 0, i32 0, i8 0)
// CHECK: @__kmpc_barrier
// CHECK: store {{.*}}, ptr %last.
// CHECK-NEXT: %.omp.lastprivate.done

// scalar, nowait
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_no_loop_lastprivate_l109{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: @__kmpc_free_shared(ptr %last{{.*}})
// CHECK-NEXT: ret void
// NOLOOP: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_lastprivate_l109{{.*}}, i32 0, i32 0, i8 1)
// STRIDED: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_no_loop_lastprivate_l109{{.*}}, i32 0, i32 0, i8 0)
// CHECK: @__kmpc_barrier
// CHECK: store {{.*}}, ptr %last.
// CHECK-NEXT: %.omp.lastprivate.done

// num_teams(3)
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_strided_combined_directive_l136{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// CHECK: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_strided_combined_directive_l136{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void

// num_teams(3)
// CHECK-LABEL: @__kmpc_parallel_60({{.*}}promotable_strided_split_directive_l142{{.*}})
// CHECK: omp.loop.exit:
// CHECK-NEXT: ret void
// CHECK: @__kmpc_distribute_for_static_loop_4u({{.*}}promotable_strided_split_directive_l142{{.*}}, i32 0, i32 0, i8 0)
// CHECK: omp_loop.after:
// CHECK-NEXT: ret void
