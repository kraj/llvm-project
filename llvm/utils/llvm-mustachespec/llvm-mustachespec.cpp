//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Simple drivers to test the mustache spec found at
// https://github.com/mustache/
//
// It is used to verify that the current implementation conforms to the spec.
// Simply download the spec and pass the test files to the driver.
//
// The current implementation only supports non-optional parts of the spec, so
// we do not expect any of the dynamic-names, inheritance, or lambda tests to
// pass. Additionally, Triple Mustache is not supported, so we expect the
// following tests to fail:
//    Triple Mustache
//    Triple Mustache Integer Interpolation
//    Triple Mustache Decimal Interpolation
//    Triple Mustache Null Interpolation
//    Triple Mustache Context Miss Interpolation
//    Dotted Names - Triple Mustache Interpolation
//    Implicit Iterators - Triple Mustache
//    Triple Mustache - Surrounding Whitespace
//    Triple Mustache - Standalone
//    Triple Mustache With Padding
//    Standalone Indentation
//    Implicit Iterator - Triple mustache
//
// Usage:
//  llvm-mustachespec path/to/test/file.json path/to/test/file2.json ...
//===----------------------------------------------------------------------===//

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Mustache.h"
#include <string>

using namespace llvm;
using namespace llvm::json;
using namespace llvm::mustache;

#define DEBUG_TYPE "llvm-mustachespec"

static cl::OptionCategory Cat("llvm-mustachespec Options");

cl::list<std::string> InputFiles(cl::Positional, cl::desc("<input files>"),
                                 cl::OneOrMore);

cl::opt<bool> ReportErrors("report-errors",
                           cl::desc("Report errors in spec tests"),
                           cl::cat(Cat));

static ExitOnError ExitOnErr;

struct TestData {
  static Expected<TestData> createTestData(json::Object *TestCase,
                                           StringRef InputFile) {
    // If any of the needed elements are missing, we cannot continue.
    // NOTE: partials are optional in the test schema.
    if (!TestCase || !TestCase->getString("template") ||
        !TestCase->getString("expected") || !TestCase->getString("name") ||
        !TestCase->get("data"))
      return createStringError(
          llvm::inconvertibleErrorCode(),
          "invalid JSON schema in test file: " + InputFile + "\n");

    return TestData{TestCase->getString("template").value(),
                    TestCase->getString("expected").value(),
                    TestCase->getString("name").value(), TestCase->get("data"),
                    TestCase->get("partials")};
  }

  TestData() = default;

  StringRef TemplateStr;
  StringRef ExpectedStr;
  StringRef Name;
  Value *Data;
  Value *Partials;
};

static void reportTestFailure(const TestData &TD, StringRef ActualStr) {
  LLVM_DEBUG(dbgs() << "Template: " << TD.TemplateStr << "\n");
  if (TD.Partials) {
    LLVM_DEBUG(dbgs() << "Partial: ");
    LLVM_DEBUG(TD.Partials->print(dbgs()));
    LLVM_DEBUG(dbgs() << "\n");
  }
  LLVM_DEBUG(dbgs() << "JSON Data: ");
  LLVM_DEBUG(TD.Data->print(dbgs()));
  LLVM_DEBUG(dbgs() << "\n");
  outs() << "Test Failed: " << TD.Name << "\n";
  if (ReportErrors) {
    outs() << "  Expected: \'" << TD.ExpectedStr << "\'\n"
           << "  Actual: \'" << ActualStr << "\'\n"
           << " ====================\n";
  }
}

static void registerPartials(Value *Partials, Template &T) {
  if (!Partials)
    return;
  for (const auto &[Partial, Str] : *Partials->getAsObject())
    T.registerPartial(Partial.str(), Str.getAsString()->str());
}

static json::Value readJsonFromFile(StringRef &InputFile) {
  std::unique_ptr<MemoryBuffer> Buffer =
      ExitOnErr(errorOrToExpected(MemoryBuffer::getFile(InputFile)));
  return ExitOnErr(parse(Buffer->getBuffer()));
}

static void runTest(StringRef InputFile) {
  outs() << "Running Tests: " << InputFile << "\n";
  json::Value Json = readJsonFromFile(InputFile);

  json::Object *Obj = Json.getAsObject();
  Array *TestArray = Obj->getArray("tests");
  // Even though we parsed the JSON, it can have a bad format, so check it.
  if (!TestArray)
    ExitOnErr(createStringError(
        llvm::inconvertibleErrorCode(),
        "invalid JSON schema in test file: " + InputFile + "\n"));

  const size_t Total = TestArray->size();
  size_t Success = 0;

  for (Value V : *TestArray) {
    auto TestData =
        ExitOnErr(TestData::createTestData(V.getAsObject(), InputFile));
    Template T(TestData.TemplateStr);
    registerPartials(TestData.Partials, T);

    std::string ActualStr;
    raw_string_ostream OS(ActualStr);
    T.render(*TestData.Data, OS);
    if (TestData.ExpectedStr == ActualStr)
      ++Success;
    else
      reportTestFailure(TestData, ActualStr);
  }

  outs() << "Result [" << Success << "/" << Total << "] succeeded\n";
}

int main(int argc, char **argv) {
  ExitOnErr.setBanner(std::string(argv[0]) + " error: ");
  cl::ParseCommandLineOptions(argc, argv);
  for (const auto &FileName : InputFiles)
    runTest(FileName);
  return 0;
}
