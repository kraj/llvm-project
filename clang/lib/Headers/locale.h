/*===---- locale.h - Standard header for localization ---------------------===*\
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
\*===----------------------------------------------------------------------===*/

// Although technically correct according to the standard, NULL defined as 0
// may be problematic since it may result in a different size object than a
// pointer (eg 64 bit mode on AIX).  Therefore, re-define the macro to
// ((void*)0) for consistency where needed.

// The standard specifies that locale.h defines NULL so ensure that the
// definition is correct since it might not be redefined if it is already
// defined.  This ensures any use of NULL is correct while processing the
// include_next.
#define __need_NULL
#include <stddef.h>

#include_next <locale.h>

// Ensure that the definition of NULL is as expected (likely redundant).
#define __need_NULL
#include <stddef.h>
