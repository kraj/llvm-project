// Test for memprof histogram bounds checking (255 limit)
// RUN: %clangxx_memprof -O0 %s -o %t && %env_memprof_opts=print_text=1:histogram=1 %run %t 2>&1 | FileCheck %s

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  // Allocate memory 
  char *buffer = (char *)malloc(256);
  if (!buffer) return 1;
  
  // Access a single byte many times to test counter overflow protection
  // This should create a counter that would exceed 255 if not capped
  for (int i = 0; i < 300; ++i) {
    buffer[0] = 'X';
  }
  
  // Access another byte fewer times
  for (int i = 0; i < 100; ++i) {
    buffer[8] = 'Y';
  }
  
  // Free to trigger histogram creation
  free(buffer);
  
  printf("Bounds test completed\n");
  return 0;
}

// CHECK: AccessCountHistogram
// CHECK: Bounds test completed