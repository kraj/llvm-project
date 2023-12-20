//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Make sure that the module flags contain the expected elements.
// The tests only look for the expected components and not the exact flags.
// Otherwise changing the location of the module breaks this test.

// MODULES: std
//
// RUN: echo "%{module_flags}" | grep -- "-fprebuilt-module-path="
// RUN: echo "%{module_flags}" | grep "std.pcm"

// The std module should not provide the std.compat module
// RUN: echo "%{module_flags}" | grep -v "std.compat.pcm"
