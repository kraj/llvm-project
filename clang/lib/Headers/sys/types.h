/*===---- sys/types.h - BSD header for types ------------------------------===*\
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

// Limit the effects to those platforms that have this as a system header.
#if defined(_AIX)

// Ensure that the definition of NULL (if present) is correct since it might
// not be redefined if it is already defined.  This ensures any use of NULL is
// correct while processing the include_next.
#define __need_NULL
#include <stddef.h>

#endif

// Always include_next the file so that it will be included when requested.
// This will trigger an error on platforms where it is not found unless
// system include paths find it, which is the correct behaviour.
#include_next <sys/types.h>

// Limit the effects to those platforms that have this as a system header.
#if defined(_AIX) && defined(NULL)

// Ensure that the definition of NULL (if present) is consistent with what
// is expected, regardless of where it came from.
#define __need_NULL
#include <stddef.h>

#endif
