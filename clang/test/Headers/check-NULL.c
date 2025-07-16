// There are at least 2 valid C null-pointer constants as defined
// by the C language standard.
// Test that the macro NULL is defined consistently for all platforms by
// those headers that the C standard mandates a macro definition for NULL.

// RUN: %clang %s -Dheader="<locale.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<stdio.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<stdlib.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<string.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<time.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<wchar.h>" -E | tail -1 | FileCheck %s

#include header
void *p = NULL;
// CHECK: ({{ *}}({{ *}}void{{ *}}*{{ *}}){{ *}}0{{ *}})
