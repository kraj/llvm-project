//===-- Unittests for strcpy_s --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#define __STDC_WANT_LIB_EXT1__ 1
#include "hdr/stdint_proxy.h"
#undef __STDC_WANT_LIB_EXT1__

#include "hdr/types/errno_t.h"
#include "src/string/strcpy_s.h"
#include "test/UnitTest/ConstraintHandlerCheckingTest.h"

using LlvmLibcStrCpySTest =
    LIBC_NAMESPACE::testing::ConstraintHandlerCheckingTest;

// Success cases
TEST_F(LlvmLibcStrCpySTest, SuccessfulCopy) {
  char s1[8];
  const char *s2 = "abc";

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, sizeof(s1), s2);
  ASSERT_EQ(result, 0);
  ASSERT_STREQ(s1, s2);
  ASSERT_STREQ(buffer, "");
}

TEST_F(LlvmLibcStrCpySTest, ExactFitSuccessfulCopy) {
  char s1[4];
  const char *s2 = "abc";

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, sizeof(s1), s2);
  ASSERT_EQ(result, 0);
  ASSERT_STREQ(s1, s2);
  ASSERT_STREQ(buffer, "");
}

TEST_F(LlvmLibcStrCpySTest, AdjacentObjectsDoNotOverlap) {
  char s[8] = {'a', 'b', 'c', '\0', '?', '?', '?', '?'};

  errno_t result = LIBC_NAMESPACE::strcpy_s(s + 4, 4, s);
  ASSERT_EQ(result, 0);
  ASSERT_STREQ(s + 4, "abc");
  ASSERT_STREQ(buffer, "");
}

TEST_F(LlvmLibcStrCpySTest, EmptySourceString) {
  char s1[4];

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, sizeof(s1), "");
  ASSERT_EQ(result, 0);
  ASSERT_EQ(s1[0], '\0');
  ASSERT_STREQ(buffer, "");
}

// Failure cases
TEST_F(LlvmLibcStrCpySTest, NullS1) {
  const char *s2 = "abc";
  char *s1 = 0;

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, 4, s2);
  ASSERT_NE(result, 0);
  ASSERT_STREQ(buffer, "strcpy_s: s1 is null");
}

TEST_F(LlvmLibcStrCpySTest, NullS2) {
  char s1[4];
  const char *s2 = 0;

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, 4, s2);
  ASSERT_NE(result, 0);
  ASSERT_EQ(s1[0], '\0');
  ASSERT_STREQ(buffer, "strcpy_s: s2 is null");
}

TEST_F(LlvmLibcStrCpySTest, S1MaxGreaterThanRSizeMax) {
  char s1[4];
  const char *s2 = "abc";

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, RSIZE_MAX + 1, s2);
  ASSERT_NE(result, 0);
  ASSERT_STREQ(buffer, "strcpy_s: s1max exceeds RSIZE_MAX");
}

TEST_F(LlvmLibcStrCpySTest, S1MaxIsZero) {
  char s1[4];
  const char *s2 = "abc";

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, 0, s2);
  ASSERT_NE(result, 0);
  ASSERT_STREQ(buffer, "strcpy_s: s1max is 0");
}

TEST_F(LlvmLibcStrCpySTest, S1MaxTooSmallForS2) {
  char s1[3];
  const char *s2 = "abc";

  errno_t result = LIBC_NAMESPACE::strcpy_s(s1, 3, s2);
  ASSERT_NE(result, 0);
  ASSERT_EQ(s1[0], '\0');
  ASSERT_STREQ(buffer, "strcpy_s: s1max is too small for s2");

  s2 = "abcd";
  s1[0] = '?';
  buffer[0] = '\0';
  result = LIBC_NAMESPACE::strcpy_s(s1, 3, s2);
  ASSERT_NE(result, 0);
  ASSERT_EQ(s1[0], '\0');
  ASSERT_STREQ(buffer, "strcpy_s: s1max is too small for s2");
}

TEST_F(LlvmLibcStrCpySTest, OverlappingObjects) {
  char s[10] = "123456789";

  errno_t result = LIBC_NAMESPACE::strcpy_s(s, 6, s + 4);
  ASSERT_NE(result, 0);
  ASSERT_EQ(s[0], '\0');
  ASSERT_STREQ(buffer, "strcpy_s: s1 and s2 overlap");
}
