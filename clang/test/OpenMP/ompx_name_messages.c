// Test for ompx_name clause semantic checking
// RUN: %clang_cc1 -verify -fopenmp %s -Wuninitialized

// expected-no-diagnostics

void foo() {
  #pragma omp target ompx_name("my_kernel")
  {
  }

  #pragma omp target parallel ompx_name("parallel_kernel")
  {
  }

  #pragma omp target teams ompx_name("teams_kernel")
  {
  }

  #pragma omp target simd ompx_name("simd_kernel")
  for (int i = 0; i < 10; i++)
    ;
}

// Test with various target constructs
void bar() {
  #pragma omp target parallel for ompx_name("parallel_for_kernel")
  for (int i = 0; i < 10; i++)
    ;

  #pragma omp target parallel for simd ompx_name("parallel_for_simd_kernel")
  for (int i = 0; i < 10; i++)
    ;

  #pragma omp target teams distribute ompx_name("teams_distribute_kernel")
  for (int i = 0; i < 10; i++)
    ;

  #pragma omp target teams distribute parallel for ompx_name("teams_dist_par_for_kernel")
  for (int i = 0; i < 10; i++)
    ;

  #pragma omp target teams distribute parallel for simd ompx_name("teams_dist_par_for_simd_kernel")
  for (int i = 0; i < 10; i++)
    ;

  #pragma omp target teams distribute simd ompx_name("teams_distribute_simd_kernel")
  for (int i = 0; i < 10; i++)
    ;
}
