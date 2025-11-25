//===-- clang-doc/HTMLGeneratorTest.cpp ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClangDocTest.h"
#include "Generators.h"
#include "Representation.h"
#include "config.h"
#include "support/Utils.h"
#include "clang/Basic/Version.h"
#include "llvm/Testing/Support/Error.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace testing;
using namespace clang;
using namespace clang::doc;

// FIXME: Don't enable unit tests that can read files. Remove once we can use
// lit to test these properties.
#define ENABLE_LOCAL_TEST 0

static const std::string ClangDocVersion = getClangToolFullVersion("clang-doc");

static std::unique_ptr<Generator> getHTMLGenerator() {
  auto G = findGeneratorByName("html");
  if (!G)
    return nullptr;
  return std::move(G.get());
}

static ClangDocContext
getClangDocContext(std::vector<std::string> UserStylesheets = {},
                   StringRef RepositoryUrl = "",
                   StringRef RepositoryLinePrefix = "", StringRef Base = "") {
  ClangDocContext CDCtx{
      {},   "test-project", {}, {}, {}, RepositoryUrl, RepositoryLinePrefix,
      Base, UserStylesheets};
  CDCtx.UserStylesheets.insert(CDCtx.UserStylesheets.begin(), "");
  CDCtx.JsScripts.emplace_back("");
  return CDCtx;
}

TEST(HTMLGeneratorTest, createResources) {
  auto G = getHTMLGenerator();
  ASSERT_THAT(G, NotNull()) << "Could not find HTMLGenerator";
  ClangDocContext CDCtx = getClangDocContext();
  EXPECT_THAT_ERROR(G->createResources(CDCtx), Failed())
      << "Empty UserStylesheets or JsScripts should fail!";
}
