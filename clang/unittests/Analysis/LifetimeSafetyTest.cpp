//===- LifetimeSafetyTest.cpp - Lifetime Safety Tests -*---------- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/Analysis/Analyses/LifetimeSafety.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Testing/TestAST.h"
#include "llvm/ADT/StringMap.h"
#include "gtest/gtest.h"

namespace clang::lifetimes::internal {
namespace {

using namespace ast_matchers;

// A helper class to run the full lifetime analysis on a piece of code
// and provide an interface for querying the results.
class LifetimeTestRunner {
public:
  LifetimeTestRunner(llvm::StringRef Code) {
    std::string FullCode = "struct MyObj { ~MyObj() {} int i; };\n";
    FullCode += Code.str();

    TestAST = std::make_unique<clang::TestAST>(FullCode);
    ASTCtx = &TestAST->context();

    // Find the target function using AST matchers.
    auto MatchResult =
        match(functionDecl(hasName("target")).bind("target"), *ASTCtx);
    auto *FD = selectFirst<FunctionDecl>("target", MatchResult);
    if (!FD) {
      ADD_FAILURE() << "Test case must have a function named 'target'";
      return;
    }
    AnalysisCtx = std::make_unique<AnalysisDeclContext>(nullptr, FD);
    AnalysisCtx->getCFGBuildOptions().setAllAlwaysAdd();

    // Run the main analysis.
    Analysis = std::make_unique<LifetimeSafetyAnalysis>(*AnalysisCtx);
    Analysis->run();

    AnnotationToPointMap = Analysis->getTestPoints();
  }

  LifetimeSafetyAnalysis &getAnalysis() { return *Analysis; }
  ASTContext &getASTContext() { return *ASTCtx; }

  ProgramPoint getProgramPoint(llvm::StringRef Annotation) {
    auto It = AnnotationToPointMap.find(Annotation);
    if (It == AnnotationToPointMap.end()) {
      ADD_FAILURE() << "Annotation '" << Annotation << "' not found.";
      return {nullptr, nullptr};
    }
    return It->second;
  }

private:
  std::unique_ptr<TestAST> TestAST;
  ASTContext *ASTCtx = nullptr;
  std::unique_ptr<AnalysisDeclContext> AnalysisCtx;
  std::unique_ptr<LifetimeSafetyAnalysis> Analysis;
  llvm::StringMap<ProgramPoint> AnnotationToPointMap;
};

// A convenience wrapper that uses the LifetimeSafetyAnalysis public API.
class LifetimeTestHelper {
public:
  LifetimeTestHelper(LifetimeTestRunner &Runner)
      : Runner(Runner), Analysis(Runner.getAnalysis()) {}

  OriginID getOriginForDecl(llvm::StringRef VarName) {
    auto *VD = findDecl<ValueDecl>(VarName);
    auto OID = Analysis.getOriginIDForDecl(VD);
    if (!OID)
      ADD_FAILURE() << "Origin for '" << VarName << "' not found.";
    return OID.value_or(OriginID());
  }

  LoanID getLoanForVar(llvm::StringRef VarName) {
    auto *VD = findDecl<VarDecl>(VarName);
    auto LID = Analysis.getLoanIDForVar(VD);
    if (!LID)
      ADD_FAILURE() << "Loan for '" << VarName << "' not found.";
    return LID.value_or(LoanID());
  }

  LoanSet getLoansAtPoint(OriginID OID, llvm::StringRef Annotation) {
    ProgramPoint PP = Runner.getProgramPoint(Annotation);
    if (!PP.first)
      return LoanSet{nullptr};
    return Analysis.getLoansAtPoint(OID, PP);
  }

private:
  template <typename DeclT> DeclT *findDecl(llvm::StringRef Name) {
    auto &Ctx = Runner.getASTContext();
    auto Results = match(valueDecl(hasName(Name)).bind("v"), Ctx);
    if (Results.empty()) {
      ADD_FAILURE() << "Declaration '" << Name << "' not found in AST.";
      return nullptr;
    }
    return const_cast<DeclT *>(selectFirst<DeclT>("v", Results));
  }

  LifetimeTestRunner &Runner;
  LifetimeSafetyAnalysis &Analysis;
};

void runLifetimeTest(llvm::StringRef Code,
                     std::function<void(LifetimeTestHelper &)> Verify) {
  LifetimeTestRunner Runner(Code);
  LifetimeTestHelper Helper(Runner);
  Verify(Helper);
}

// ========================================================================= //
//                                 TEST CASES
// ========================================================================= //

TEST(LifetimeAnalysis, SimpleLoanAndOrigin) {
  const char *Code = R"(
    void target() {
      int x;
      int* p = &x;
      void("__lifetime_test_point_p1");
    }
  )";
  runLifetimeTest(Code, [](LifetimeTestHelper &Helper) {
    auto O_p = Helper.getOriginForDecl("p");
    auto L_x = Helper.getLoanForVar("x");

    auto LoansAtP1 = Helper.getLoansAtPoint(O_p, "p1");
    EXPECT_TRUE(LoansAtP1.contains(L_x));
  });
}

TEST(LifetimeAnalysis, OverwriteOrigin) {
  const char *Code = R"(
    void target() {
      MyObj s1, s2;
      MyObj* p = &s1;
      void("__lifetime_test_point_after_s1");
      p = &s2;
      void("__lifetime_test_point_after_s2");
    }
  )";
  runLifetimeTest(Code, [](LifetimeTestHelper &Helper) {
    auto O_p = Helper.getOriginForDecl("p");
    auto L_s1 = Helper.getLoanForVar("s1");
    auto L_s2 = Helper.getLoanForVar("s2");

    auto Loans1 = Helper.getLoansAtPoint(O_p, "after_s1");
    EXPECT_TRUE(Loans1.contains(L_s1));
    EXPECT_FALSE(Loans1.contains(L_s2));

    auto Loans2 = Helper.getLoansAtPoint(O_p, "after_s2");
    EXPECT_FALSE(Loans2.contains(L_s1));
    EXPECT_TRUE(Loans2.contains(L_s2));
  });
}

TEST(LifetimeAnalysis, ConditionalLoan) {
  const char *Code = R"(
    void target(bool cond) {
      int a, b;
      int *p = nullptr;
      if (cond) {
        p = &a;
      } else {
        p = &b;
      }
      void("__lifetime_test_point_after_if");
    }
  )";
  runLifetimeTest(Code, [](LifetimeTestHelper &Helper) {
    auto O_p = Helper.getOriginForDecl("p");
    auto L_a = Helper.getLoanForVar("a");
    auto L_b = Helper.getLoanForVar("b");

    auto LoansAtEnd = Helper.getLoansAtPoint(O_p, "after_if");
    EXPECT_TRUE(LoansAtEnd.contains(L_a));
    EXPECT_TRUE(LoansAtEnd.contains(L_b));
  });
}
} // anonymous namespace
} // namespace clang::lifetimes::internal
