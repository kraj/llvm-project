// RUN: rm -rf %t && mkdir -p %t
// RUN: %clang_cc1 -fmodules -fimplicit-module-maps -fmodules-cache-path=%t/ModulesCache -fapinotes-modules -fsyntax-only -I %S/Inputs/Headers -F %S/Inputs/Frameworks %s -ast-dump -ast-dump-filter asdf | FileCheck %s

#include "BoundsUnsafe.h"

// CHECK: imported in BoundsUnsafe asdf_counted 'void (int *, int)'
// CHECK: imported in BoundsUnsafe buf 'int *'

// CHECK: imported in BoundsUnsafe asdf_sized 'void (int *, int)'
// CHECK: imported in BoundsUnsafe buf 'int *'

// CHECK: imported in BoundsUnsafe asdf_counted_n 'void (int *, int)'
// CHECK: imported in BoundsUnsafe buf 'int *'

// CHECK: imported in BoundsUnsafe asdf_sized_n 'void (int *, int)'
// CHECK: imported in BoundsUnsafe buf 'int *'

// CHECK: imported in BoundsUnsafe asdf_ended 'void (int *, int *)'
// CHECK: imported in BoundsUnsafe buf 'int *'
