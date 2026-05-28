// Test for ompx_name clause error checking
// RUN: %clang_cc1 -verify -fopenmp %s

void foo() {
  int x = 5;

  // expected-error@+1 {{argument to 'ompx_name' clause must be a string literal}}
  #pragma omp target ompx_name(x)
  {
  }

  // expected-error@+1 {{argument to 'ompx_name' clause must be a string literal}}
  #pragma omp target ompx_name(123)
  {
  }

  // This should work - string literal
  #pragma omp target ompx_name("valid_name")
  {
  }
}
