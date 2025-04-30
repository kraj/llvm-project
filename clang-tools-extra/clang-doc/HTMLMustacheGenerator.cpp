//===-- HTMLMustacheGenerator.cpp - HTML Mustache Generator -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "Generators.h"
#include "Representation.h"
#include "support/File.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Mustache.h"

using namespace llvm;
using namespace llvm::json;
using namespace llvm::mustache;

namespace clang {
namespace doc {

class MustacheHTMLGenerator : public Generator {
public:
  static const char *Format;
  Error generateDocs(StringRef RootDir,
                     StringMap<std::unique_ptr<doc::Info>> Infos,
                     const ClangDocContext &CDCtx) override;
  Error createResources(ClangDocContext &CDCtx) override;
  Error generateDocForInfo(Info *I, raw_ostream &OS,
                           const ClangDocContext &CDCtx) override;
};

class MustacheTemplateFile : public Template {
public:
  static ErrorOr<std::unique_ptr<MustacheTemplateFile>>
  createMustacheFile(StringRef FileName) {
    ErrorOr<std::unique_ptr<MemoryBuffer>> BufferOrError =
        MemoryBuffer::getFile(FileName);
    if (auto EC = BufferOrError.getError())
      return EC;

    std::unique_ptr<MemoryBuffer> Buffer = std::move(BufferOrError.get());
    StringRef FileContent = Buffer->getBuffer();
    return std::make_unique<MustacheTemplateFile>(FileContent);
  }

  Error registerPartialFile(StringRef Name, StringRef FileName) {
    ErrorOr<std::unique_ptr<MemoryBuffer>> BufferOrError =
        MemoryBuffer::getFile(FileName);
    if (auto EC = BufferOrError.getError())
      return createFileError("cannot open file", EC);
    std::unique_ptr<MemoryBuffer> Buffer = std::move(BufferOrError.get());
    StringRef FileContent = Buffer->getBuffer();
    registerPartial(Name.str(), FileContent.str());
    return Error::success();
  }

  MustacheTemplateFile(StringRef TemplateStr) : Template(TemplateStr) {}
};
Error MustacheHTMLGenerator::generateDocs(
    StringRef RootDir, StringMap<std::unique_ptr<doc::Info>> Infos,
    const clang::doc::ClangDocContext &CDCtx) {
  return Error::success();
}
Error MustacheHTMLGenerator::generateDocForInfo(Info *I, raw_ostream &OS,
                                                const ClangDocContext &CDCtx) {
  return Error::success();
}
Error MustacheHTMLGenerator::createResources(ClangDocContext &CDCtx) {
  Error Err = Error::success();
  return Error::success();
}

const char *MustacheHTMLGenerator::Format = "mhtml";

static GeneratorRegistry::Add<MustacheHTMLGenerator>
    MHTML(MustacheHTMLGenerator::Format, "Generator for mustache HTML output.");

// This anchor is used to force the linker to link in the generated object
// file and thus register the generator.
volatile int MHTMLGeneratorAnchorSource = 0;

} // namespace doc
} // namespace clang
