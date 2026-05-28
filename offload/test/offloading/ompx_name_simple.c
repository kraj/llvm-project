// RUN: %libomptarget-compile-generic
// RUN: %libomptarget-run-generic

#include <stdio.h>

int main() {
  int x = 0;

// Simple test of ompx_name clause
#pragma omp target ompx_name("simple_kernel") map(from : x)
  {
    x = 1;
  }

  if (x != 1) {
    printf("FAIL\n");
    return 1;
  }

  // Test with parallel construct
  int y = 0;
#pragma omp target parallel ompx_name("parallel_kernel") map(tofrom : y)
  {
#pragma omp atomic
    y++;
  }

  if (y == 0) {
    printf("FAIL\n");
    return 1;
  }

  // Test with teams construct
  int z = 0;
#pragma omp target teams ompx_name("teams_kernel") map(tofrom : z)
  {
#pragma omp atomic
    z++;
  }

  if (z == 0) {
    printf("FAIL\n");
    return 1;
  }

  printf("PASS\n");
  return 0;
}
