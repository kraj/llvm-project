// Verify that OpenMP callback analysis proves the captured pointer containers
// noalias, allowing their loads to be hoisted out of the loop across an
// unrelated opaque call.
//
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -O2 -ffast-math -fopenmp \
// RUN:   -x c++ -emit-llvm -o - %s \
// RUN:   | FileCheck %s

extern "C" int omp_get_num_threads();
extern "C" int omp_get_thread_num();
extern "C" void opaque();

double sum_if(double *values, const bool *enabled, int count) {
  double sum = 0.0;
#pragma omp parallel reduction(+ : sum)
  {
    int threads = omp_get_num_threads();
    int thread = omp_get_thread_num();

    for (int i = thread * count / threads;
         i < (thread + 1) * count / threads; ++i) {
      opaque();
      if (enabled[i])
        sum += values[i];
    }
  }
  return sum;
}

// CHECK-LABEL: define internal void @_Z6sum_ifPdPKbi.omp_outlined(
// CHECK-SAME: ptr noalias noundef nonnull readonly align 8 captures(none) dereferenceable(8) %enabled,
// CHECK-SAME: {{.*}}ptr noalias noundef nonnull readonly align 8 captures(none) dereferenceable(8) %values)
// CHECK: call i32 @omp_get_num_threads()
// CHECK: call i32 @omp_get_thread_num()
// CHECK: for.body.lr.ph:
// CHECK: [[VALUES:%.*]] = load ptr, ptr %values
// CHECK: for.body:
// CHECK-NOT: load ptr, ptr %values
// CHECK: call void @opaque()
// CHECK: if.then:
// CHECK: getelementptr {{.*}}, ptr [[VALUES]],
