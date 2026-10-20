//===- SourceMgrDiagnosticVerifier.cpp -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/SourceMgrDiagnosticVerifier.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

SourceMgrDiagnosticVerifier::SourceMgrDiagnosticVerifier(
    const SourceMgr &Mgr, raw_ostream &OS, ArrayRef<StringRef> Prefixes)
    : Mgr(Mgr), OS(OS) {
  assert(!Prefixes.empty() && "must have at least one prefix");
  std::string PrefixAlt;
  raw_string_ostream PrefixOS(PrefixAlt);
  ListSeparator Sep("|");
  for (StringRef Prefix : Prefixes) {
    assert(!Prefix.empty() && "prefixes must be non-empty");
    PrefixOS << Sep << Regex::escape(Prefix);
  }
  // Don't match 'expected' inside a longer word such as 'unexpected'.
  Expected = Regex("(^|[^[:alnum:]_-])(" + PrefixAlt +
                   ")-(error|note|remark|warning)(-re)?"
                   " *(@([+-][0-9]+|above|below|unknown))? *{{(.*)}}$");

  for (unsigned I = 0, E = Mgr.getNumBuffers(); I != E; ++I)
    (void)computeExpectedDiags(Mgr.getMemoryBuffer(I + 1));
}

/// Given a diagnostic kind, return a human readable string for it.
static StringRef getDiagKindStr(SourceMgr::DiagKind Kind) {
  switch (Kind) {
  case SourceMgr::DK_Note:
    return "note";
  case SourceMgr::DK_Warning:
    return "warning";
  case SourceMgr::DK_Error:
    return "error";
  case SourceMgr::DK_Remark:
    return "remark";
  }
  llvm_unreachable("Unknown SourceMgr::DiagKind");
}

void SourceMgrDiagnosticVerifier::printDiag(const SMDiagnostic &Diag) const {
  Mgr.printIncludeStackForDiagnostic(Diag.getLoc(), OS);
  Diag.print(nullptr, OS);
}

bool SourceMgrDiagnosticVerifier::emitError(const ExpectedDiag &Diag,
                                            const Twine &Msg) {
  OK = false;
  SMRange Range;
  ArrayRef<SMRange> Ranges;
  if (Diag.FileLoc.isValid()) {
    Range =
        SMRange(Diag.FileLoc, SMLoc::getFromPointer(Diag.FileLoc.getPointer() +
                                                    Diag.Substring.size()));
    Ranges = Range;
  }
  printDiag(Mgr.GetMessage(Diag.FileLoc, SourceMgr::DK_Error, Msg, Ranges));
  return false;
}

bool SourceMgrDiagnosticVerifier::ExpectedDiag::match(StringRef Str) const {
  // If this isn't a regex diagnostic, we simply check if the string was
  // contained.
  if (SubstringRegex)
    return SubstringRegex->match(Str);
  return Str.contains(Substring);
}

bool SourceMgrDiagnosticVerifier::computeRegex(ExpectedDiag &Diag) {
  std::string RegexStr;
  raw_string_ostream RegexOS(RegexStr);
  StringRef StrToProcess = Diag.Substring;
  while (!StrToProcess.empty()) {
    // Find the next regex block.
    size_t RegexIt = StrToProcess.find("{{");
    if (RegexIt == StringRef::npos) {
      RegexOS << Regex::escape(StrToProcess);
      break;
    }
    RegexOS << Regex::escape(StrToProcess.take_front(RegexIt));
    StrToProcess = StrToProcess.drop_front(RegexIt + 2);

    // Find the end of the regex block.
    size_t RegexEndIt = StrToProcess.find("}}");
    if (RegexEndIt == StringRef::npos)
      return emitError(Diag, "found start of regex with no end '}}'");
    StringRef RegexBlock = StrToProcess.take_front(RegexEndIt);

    // Validate that the regex is actually valid.
    std::string RegexError;
    if (!Regex(RegexBlock).isValid(RegexError))
      return emitError(Diag, "invalid regex: " + RegexError);

    RegexOS << '(' << RegexBlock << ')';
    StrToProcess = StrToProcess.drop_front(RegexEndIt + 2);
  }
  Diag.SubstringRegex = Regex(RegexStr);
  return true;
}

MutableArrayRef<SourceMgrDiagnosticVerifier::ExpectedDiag>
SourceMgrDiagnosticVerifier::computeExpectedDiags(const MemoryBuffer *Buf) {
  // If the buffer is invalid, return an empty list.
  if (!Buf)
    return {};
  auto &ExpectedDiags = ExpectedDiagsPerFile[Buf->getBufferIdentifier()];

  // The number of the last line that did not correlate to a designator.
  unsigned LastNonDesignatorLine = 0;

  // The indices of designators that apply to the next non designator line.
  SmallVector<unsigned, 1> DesignatorsForNextLine;

  // Scan the file for <prefix>-* designators.
  SmallVector<StringRef, 100> Lines;
  Buf->getBuffer().split(Lines, '\n');
  for (unsigned LineNo = 0, E = Lines.size(); LineNo < E; ++LineNo) {
    StringRef Line = Lines[LineNo].rtrim();

    // Every directive contains '{{'; check that first to avoid running the
    // regex on most lines.
    SmallVector<StringRef, 8> Matches;
    if (!Line.contains("{{") || !Expected.match(Line, &Matches)) {
      // Check for designators that apply to this line.
      if (!DesignatorsForNextLine.empty()) {
        for (unsigned DiagIndex : DesignatorsForNextLine)
          ExpectedDiags[DiagIndex].LineNo = LineNo + 1;
        DesignatorsForNextLine.clear();
      }
      LastNonDesignatorLine = LineNo;
      continue;
    }

    // Point to the start of <prefix>-*.
    SMLoc ExpectedStart = SMLoc::getFromPointer(Matches[2].data());

    SourceMgr::DiagKind Kind;
    if (Matches[3] == "error")
      Kind = SourceMgr::DK_Error;
    else if (Matches[3] == "warning")
      Kind = SourceMgr::DK_Warning;
    else if (Matches[3] == "remark")
      Kind = SourceMgr::DK_Remark;
    else {
      assert(Matches[3] == "note");
      Kind = SourceMgr::DK_Note;
    }
    ExpectedDiag Record(Kind, LineNo + 1, ExpectedStart, Matches[7]);

    // Check to see if this is a regex match, i.e. it includes the `-re`.
    if (!Matches[4].empty() && !computeRegex(Record))
      continue;

    StringRef OffsetMatch = Matches[5];
    if (!OffsetMatch.empty()) {
      OffsetMatch = OffsetMatch.drop_front(1);

      // Get the integer value without the @ and +/- prefix.
      if (OffsetMatch[0] == '+' || OffsetMatch[0] == '-') {
        int Offset;
        OffsetMatch.drop_front().getAsInteger(0, Offset);

        if (OffsetMatch.front() == '+')
          Record.LineNo += Offset;
        else
          Record.LineNo -= Offset;
      } else if (OffsetMatch.consume_front("unknown")) {
        // This is matching unknown locations.
        Record.FileLoc = SMLoc();
        ExpectedUnknownLocDiags.emplace_back(std::move(Record));
        continue;
      } else if (OffsetMatch.consume_front("above")) {
        // If the designator applies 'above' we add it to the last non
        // designator line.
        Record.LineNo = LastNonDesignatorLine + 1;
      } else {
        // Otherwise, this is a 'below' designator and applies to the next
        // non-designator line.
        assert(OffsetMatch.consume_front("below"));
        DesignatorsForNextLine.push_back(ExpectedDiags.size());

        // Set the line number to the last in the case that this designator
        // ends up dangling.
        Record.LineNo = E;
      }
    }
    ExpectedDiags.emplace_back(std::move(Record));
  }
  return ExpectedDiags;
}

SourceMgrDiagnosticVerifier::MatchResult
SourceMgrDiagnosticVerifier::process(SourceMgr::DiagKind Kind,
                                     std::optional<Location> Loc,
                                     StringRef Message, bool ReportUnexpected) {
  MutableArrayRef<ExpectedDiag> Diags;
  if (Loc) {
    // If the buffer couldn't be resolved, `Diags` stays empty: a diagnostic
    // with a location in an unknown file can never match anything.
    if (Loc->Buf) {
      auto It = ExpectedDiagsPerFile.find(Loc->Buf->getBufferIdentifier());
      if (It != ExpectedDiagsPerFile.end())
        Diags = It->second;
      else
        Diags = computeExpectedDiags(Loc->Buf);
    }
  } else {
    Diags = ExpectedUnknownLocDiags;
  }

  // Search for a matching expected diagnostic.
  // If we find something that is close then emit a more specific error.
  ExpectedDiag *NearMiss = nullptr;

  // If this was an expected error, remember that we saw it and return.
  for (auto &E : Diags) {
    // File line must match (unless it's an unknown location).
    if (Loc && E.LineNo != Loc->LineNo)
      continue;
    if (E.match(Message)) {
      if (E.Kind == Kind) {
        E.Matched = true;
        return MatchResult::Matched;
      }

      // If this only differs based on the diagnostic kind, then consider it
      // to be a near miss.
      NearMiss = &E;
    }
  }

  if (!ReportUnexpected)
    return MatchResult::Ignored;

  OK = false;

  // Otherwise, emit an error for the near miss.
  if (NearMiss) {
    printDiag(Mgr.GetMessage(NearMiss->FileLoc, SourceMgr::DK_Error,
                             "'" + getDiagKindStr(Kind) +
                                 "' diagnostic emitted when expecting a '" +
                                 getDiagKindStr(NearMiss->Kind) + "'"));
    return MatchResult::NearMiss;
  }
  return MatchResult::Unexpected;
}

SourceMgrDiagnosticVerifier::MatchResult
SourceMgrDiagnosticVerifier::process(const SMDiagnostic &Diag,
                                     bool ReportUnexpected) {
  std::optional<Location> Loc;
  if (Diag.getLoc().isValid()) {
    const MemoryBuffer *Buf = nullptr;
    if (unsigned ID = Mgr.FindBufferContainingLoc(Diag.getLoc()))
      Buf = Mgr.getMemoryBuffer(ID);
    Loc = Location{Buf, static_cast<unsigned>(Diag.getLineNo())};
  }
  MatchResult Result =
      process(Diag.getKind(), Loc, Diag.getMessage(), ReportUnexpected);
  if (Result == MatchResult::Unexpected)
    printDiag(Diag);
  return Result;
}

bool SourceMgrDiagnosticVerifier::verify() {
  // Verify that all expected errors were seen.
  auto CheckExpectedDiags = [&](const ExpectedDiag &Diag) {
    if (!Diag.Matched)
      emitError(Diag, "expected " + getDiagKindStr(Diag.Kind) + " \"" +
                          Diag.Substring + "\" was not produced");
  };
  for (auto &ExpectedDiagsPair : ExpectedDiagsPerFile)
    for (const ExpectedDiag &Diag : ExpectedDiagsPair.second)
      CheckExpectedDiags(Diag);
  for (const ExpectedDiag &Diag : ExpectedUnknownLocDiags)
    CheckExpectedDiags(Diag);
  ExpectedDiagsPerFile.clear();
  return OK;
}
