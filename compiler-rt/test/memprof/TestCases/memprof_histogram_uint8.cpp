// Test for memprof histogram uint8_t optimization
// RUN: %clangxx_memprof -O0 %s -o %t && %env_memprof_opts=print_text=1:histogram=1 %run %t 2>&1 | FileCheck %s

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  // Allocate memory that will create a histogram
  char *buffer = (char *)malloc(1024);
  if (!buffer) return 1;
  
  // Access pattern: write to specific bytes to create histogram entries
  for (int i = 0; i < 10; ++i) {
    buffer[i * 8] = 'A';  // Access every 8th byte to create histogram patterns
  }
  
  // Access some bytes multiple times to test counter behavior
  for (int j = 0; j < 200; ++j) {
    buffer[0] = 'B';  // This should create a counter value that stays within uint8_t range
  }
  
  // Free the memory to trigger MIB creation with histogram
  free(buffer);
  
  printf("Test completed successfully\n");
  return 0;
}

// CHECK: AccessCountHistogram
// CHECK: Test completed successfully