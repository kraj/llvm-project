// There are at least 2 valid C null-pointer constants as defined
// by the C language standard.
// Test that the macro NULL is defined consistently by those system headers
// on AIX that have a macro definition for NULL.

// REQUIRES: system-aix

// RUN: %clang %s -Dheader="<dbm.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<sys/dir.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<sys/param.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<sys/types.h>" -E | tail -1 | FileCheck %s
// RUN: %clang %s -Dheader="<unistd.h>" -E | tail -1 | FileCheck %s

#include header
void *p = NULL;
// CHECK: ({{ *}}({{ *}}void{{ *}}*{{ *}}){{ *}}0{{ *}})
