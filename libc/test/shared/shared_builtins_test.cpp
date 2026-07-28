//===-- Unittests for shared builtins -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "shared/builtins.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

namespace shared = LIBC_NAMESPACE::shared;

TEST(LlvmLibcSharedBuiltinsTest, AllFloat) {
  EXPECT_FP_EQ(3.0f, LIBC_NAMESPACE::shared::addsf3(1.0f, 2.0f));
  EXPECT_FP_EQ(3.0f, LIBC_NAMESPACE::shared::divsf3(6.0f, 2.0f));
  EXPECT_FP_EQ(6.0f, LIBC_NAMESPACE::shared::mulsf3(2.0f, 3.0f));
  EXPECT_FP_EQ(2.0f, LIBC_NAMESPACE::shared::subsf3(5.0f, 3.0f));
}

TEST(LlvmLibcSharedBuiltinsTest, AllDouble) {
  EXPECT_FP_EQ(3.0, LIBC_NAMESPACE::shared::adddf3(1.0, 2.0));
  EXPECT_FP_EQ(3.0, LIBC_NAMESPACE::shared::divdf3(6.0, 2.0));
  EXPECT_FP_EQ(6.0, LIBC_NAMESPACE::shared::muldf3(2.0, 3.0));
  EXPECT_FP_EQ(2.0, LIBC_NAMESPACE::shared::subdf3(5.0, 3.0));
}

#ifdef LIBC_TYPES_HAS_FLOAT128

TEST(LlvmLibcSharedBuiltinsTest, AllFloat128) {
  EXPECT_FP_EQ(float128(3.0),
               LIBC_NAMESPACE::shared::addtf3(float128(1.0), float128(2.0)));
  EXPECT_FP_EQ(float128(3.0),
               LIBC_NAMESPACE::shared::divtf3(float128(6.0), float128(2.0)));
  EXPECT_FP_EQ(float128(6.0),
               LIBC_NAMESPACE::shared::multf3(float128(2.0), float128(3.0)));
  EXPECT_FP_EQ(float128(2.0),
               LIBC_NAMESPACE::shared::subtf3(float128(5.0), float128(3.0)));
}

#endif // LIBC_TYPES_HAS_FLOAT128

TEST(LlvmLibcSharedBuiltinsTest, SingleCompare) {
  const float aNaN =
      LIBC_NAMESPACE::fputil::FPBits<float>::quiet_nan().get_val();
  EXPECT_EQ(-1, shared::gesf2(1.0f, 2.0f));
  EXPECT_EQ(0, shared::gesf2(1.0f, 1.0f));
  EXPECT_EQ(1, shared::gesf2(2.0f, 1.0f));
  EXPECT_EQ(-1, shared::gesf2(aNaN, 1.0f));
  EXPECT_EQ(-1, shared::lesf2(1.0f, 2.0f));
  EXPECT_EQ(0, shared::lesf2(1.0f, 1.0f));
  EXPECT_EQ(1, shared::lesf2(2.0f, 1.0f));
  EXPECT_EQ(1, shared::lesf2(aNaN, 1.0f));
  EXPECT_EQ(0, shared::unordsf2(1.0f, 2.0f));
  EXPECT_EQ(1, shared::unordsf2(aNaN, 1.0f));
}

TEST_F(LlvmLibcSharedBuiltinsTest, DoubleCompare) {
  const double aNaN =
      LIBC_NAMESPACE::fputil::FPBits<double>::quiet_nan().get_val();
  const float aNaN =
      LIBC_NAMESPACE::fputil::FPBits<float>::quiet_nan().get_val();
  EXPECT_EQ(-1, shared::gedf2(1.0, 2.0));
  EXPECT_EQ(0, shared::gedf2(1.0, 1.0));
  EXPECT_EQ(1, shared::gedf2(2.0, 1.0));
  EXPECT_EQ(-1, shared::gedf2(aNaN, 1.0));
  EXPECT_EQ(-1, shared::ledf2(1.0, 2.0));
  EXPECT_EQ(0, shared::ledf2(1.0, 1.0));
  EXPECT_EQ(1, shared::ledf2(2.0, 1.0));
  EXPECT_EQ(1, shared::ledf2(aNaN, 1.0));
  EXPECT_EQ(0, shared::unorddf2(1.0, 2.0));
  EXPECT_EQ(1, shared::unorddf2(aNaN, 1.0));
}
