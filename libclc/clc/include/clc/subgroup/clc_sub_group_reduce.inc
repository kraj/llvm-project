//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#if defined(__CLC_SCALAR) &&                                                   \
    (defined(__CLC_FPSIZE) || __CLC_GENSIZE == 32 || __CLC_GENSIZE == 64)
_CLC_OVERLOAD _CLC_DECL _CLC_CONST __CLC_GENTYPE
__CLC_FUNCTION(__CLC_GENTYPE x);
#endif
