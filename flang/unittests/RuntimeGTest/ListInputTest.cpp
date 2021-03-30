//===-- flang/unittests/RuntimeGTest/ListInputTest.cpp ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CrashHandlerFixture.h"
#include "../../runtime/descriptor.h"
#include "../../runtime/io-api.h"
#include "../../runtime/io-error.h"

using namespace Fortran::runtime;
using namespace Fortran::runtime::io;

// Pads characters with whitespace when needed
void SetCharacter(char *to, std::size_t n, const char *from) {
  auto len{std::strlen(from)};
  std::memcpy(to, from, std::min(len, n));
  if (len < n) {
    std::memset(to + len, ' ', n - len);
  }
}

struct InputTest : CrashHandlerFixture {};

TEST(InputTest, TestListInput) {
  static constexpr int numBuffers{4};
  static constexpr int maxBufferLength{32};
  static char buffer[numBuffers][maxBufferLength];
  int j{0};
  for (const char *p : {"1 2 2*3  ,", ",6,,8,1*",
           "2*'abcdefghijklmnopqrstuvwxyzABC", "DEFGHIJKLMNOPQRSTUVWXYZ'"}) {
    SetCharacter(buffer[j++], maxBufferLength, p);
  }

  static StaticDescriptor<1> staticDescriptor;
  static Descriptor &whole{staticDescriptor.descriptor()};
  static SubscriptValue extent[]{numBuffers};
  whole.Establish(TypeCode{CFI_type_char}, maxBufferLength, &buffer, 1, extent,
      CFI_attribute_pointer);
  whole.Dump();
  whole.Check();

  static auto cookie{IONAME(BeginInternalArrayListInput)(whole)};
  static constexpr int listInputLength{9};
  static std::int64_t n[listInputLength]{-1, -2, -3, -4, 5, -6, 7, -8, 9};
  static const std::int64_t want[listInputLength]{1, 2, 3, 3, 5, 6, 7, 8, 9};
  for (j = 0; j < listInputLength; ++j) {
    IONAME(InputInteger)(cookie, n[j]);
  }

  static constexpr int numInputBuffers{2};
  static constexpr int inputBufferLength{54};
  static char inputBuffers[numInputBuffers][inputBufferLength]{};
  IONAME(InputAscii)(cookie, inputBuffers[0], inputBufferLength - 1);
  IONAME(InputAscii)(cookie, inputBuffers[1], inputBufferLength - 1);

  static const auto status{IONAME(EndIoStatement)(cookie)};
  ASSERT_EQ(status, 0) << "list-directed input failed, status "
                             << static_cast<int>(status) << '\n';

  for (j = 0; j < listInputLength; ++j) {
    ASSERT_EQ(n[j], want[j])
        << "wanted n[" << j << "]==" << want[j] << ", got " << n[j] << '\n';
  }

  for (j = 0; j < numInputBuffers; ++j) {
    ASSERT_EQ(std::strcmp(inputBuffers[j],
                  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ "),
        0)
        << "wanted asc[" << j << "]=alphabets, got '" << inputBuffers[j] << "'\n";
  }
}

