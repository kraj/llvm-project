//===- SourceMgrDiagnosticVerifier.h ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines a utility for verifying that diagnostics reported through
// a SourceMgr match 'expected-<kind>' comments in the source, for
// implementing '-verify'-style diagnostic tests on top of a plain
// llvm::SourceMgr.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_SOURCEMGRDIAGNOSTICVERIFIER_H
#define LLVM_SUPPORT_SOURCEMGRDIAGNOSTICVERIFIER_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/SourceMgr.h"
#include <optional>
#include <string>

namespace llvm {

class MemoryBuffer;
class raw_ostream;

/// Scans SourceMgr source buffers for 'expected-<kind>' comments (e.g.
/// 'expected-error {{message}}') and verifies that the diagnostics reported
/// to it via \c process match them, and that every expected diagnostic is
/// matched at least once.
class LLVM_ABI SourceMgrDiagnosticVerifier {
public:
  /// The result of matching a single actual diagnostic against the expected
  /// diagnostics recorded for its location.
  enum class MatchResult {
    /// The diagnostic matched an expected diagnostic (kind and text).
    Matched,
    /// The diagnostic's text matched an expected diagnostic, but its kind
    /// didn't; a message about this has already been printed.
    NearMiss,
    /// The diagnostic did not match any expected diagnostic.
    Unexpected,
    /// The diagnostic did not match, but \p ReportUnexpected was false, so
    /// nothing was printed and nothing needs to be done.
    Ignored,
  };

  /// The location of an actual diagnostic.
  struct Location {
    /// The buffer containing the diagnostic, or null if it is in a file that
    /// is not known to the SourceMgr (such a diagnostic never matches).
    const MemoryBuffer *Buf;
    /// The 1-based line number of the diagnostic.
    unsigned LineNo;
    /// The 1-based column number of the diagnostic.
    unsigned ColNo;
  };

  /// Scans all buffers currently registered with \p Mgr for expected
  /// diagnostics. Buffers added later are scanned the first time a
  /// diagnostic is reported in them. Verifier errors (near misses, unmatched
  /// expected diagnostics, malformed directives) are printed to \p OS,
  /// bypassing \p Mgr's diagnostic handler.
  ///
  /// \p Prefixes are the directive prefixes to recognize (e.g. "expected"
  /// for 'expected-error'); they must be non-empty. If \p CommentPrefixes is
  /// non-empty, directives are only recognized after the first occurrence of
  /// one of these strings on a line, so that text outside of comments is
  /// ignored.
  SourceMgrDiagnosticVerifier(const SourceMgr &Mgr, raw_ostream &OS,
                              ArrayRef<StringRef> Prefixes = {"expected"},
                              ArrayRef<StringRef> CommentPrefixes = {});

  /// Matches a diagnostic against the expected diagnostics for \p Loc, or
  /// against the '@unknown' expected diagnostics if \p Loc is std::nullopt.
  /// Prints a message on a near miss. Reporting an unexpected diagnostic is
  /// left to the caller. If \p ReportUnexpected is false, a diagnostic that
  /// does not match is ignored instead of failing verification.
  MatchResult process(SourceMgr::DiagKind Kind, std::optional<Location> Loc,
                      StringRef Message, bool ReportUnexpected = true);

  /// Like the overload above, but takes the location from \p Diag and also
  /// prints \p Diag if it is unexpected.
  MatchResult process(const SMDiagnostic &Diag, bool ReportUnexpected = true);

  /// Reports any expected diagnostic that was never matched by a call to
  /// \c process. Returns whether verification succeeded overall, i.e. no
  /// diagnostic mismatches were recorded either here or by \c process.
  bool verify();

private:
  /// A single diagnostic expected via an 'expected-<kind>' comment.
  struct ExpectedDiag {
    ExpectedDiag(SourceMgr::DiagKind Kind, unsigned LineNo, SMLoc FileLoc,
                 StringRef Substring)
        : Kind(Kind), LineNo(LineNo), FileLoc(FileLoc), Substring(Substring) {}

    /// Returns true if this diagnostic matches the given message.
    LLVM_ABI bool match(StringRef Str) const;

    /// The severity of the diagnostic expected.
    SourceMgr::DiagKind Kind;
    /// The line number the expected diagnostic should be on.
    unsigned LineNo;
    /// The column the expected diagnostic should be at, or 0 if any column
    /// matches.
    unsigned ColNo = 0;
    /// The location of the expected diagnostic within the input file.
    SMLoc FileLoc;
    /// A flag indicating if the expected diagnostic has been matched yet.
    bool Matched = false;
    /// The substring that is expected to be within the diagnostic.
    StringRef Substring;
    /// An optional regex matcher, if the expected diagnostic substring was a
    /// regex string.
    std::optional<Regex> SubstringRegex;
  };

  /// Prints \p Diag to \c OS, bypassing the SourceMgr's diagnostic handler so
  /// that a verifier installed as that handler does not re-enter itself.
  void printDiag(const SMDiagnostic &Diag) const;

  /// Records a verification failure, prints \p Msg at \p Diag's location and
  /// returns false.
  bool emitError(const ExpectedDiag &Diag, const Twine &Msg);

  /// Computes the regex matcher for a '-re' diagnostic's substring. Returns
  /// false and prints a message on error.
  bool computeRegex(ExpectedDiag &Diag);

  /// Scans \p Buf for expected diagnostics, if it has not been scanned yet,
  /// and returns its list.
  MutableArrayRef<ExpectedDiag> computeExpectedDiags(const MemoryBuffer *Buf);

  const SourceMgr &Mgr;
  raw_ostream &OS;

  /// Regex used to recognize '<prefix>-<kind>' comments.
  Regex Expected;

  /// The strings that start a comment, see the constructor.
  SmallVector<std::string, 0> CommentPrefixes;

  /// The expected diagnostics for each buffer that has been scanned so far.
  DenseMap<const MemoryBuffer *, SmallVector<ExpectedDiag, 2>>
      ExpectedDiagsPerBuffer;

  /// The expected diagnostics with an '@unknown' location.
  SmallVector<ExpectedDiag, 2> ExpectedUnknownLocDiags;

  /// Whether any diagnostic mismatch has been recorded so far.
  bool OK = true;
};

} // namespace llvm

#endif // LLVM_SUPPORT_SOURCEMGRDIAGNOSTICVERIFIER_H
