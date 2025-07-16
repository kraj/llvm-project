//===- LifetimeSafety.h - C++ Lifetime Safety Analysis -*----------- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the entry point for a dataflow-based static analysis
// that checks for C++ lifetime violations.
//
// The analysis is based on the concepts of "origins" and "loans" to track
// pointer lifetimes and detect issues like use-after-free and dangling
// pointers. See the RFC for more details:
// https://discourse.llvm.org/t/rfc-intra-procedural-lifetime-analysis-in-clang/86291
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_ANALYSIS_ANALYSES_LIFETIMESAFETY_H
#define LLVM_CLANG_ANALYSIS_ANALYSES_LIFETIMESAFETY_H
#include "clang/AST/DeclBase.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Analysis/CFG.h"
#include "llvm/ADT/ImmutableSet.h"
#include "llvm/ADT/StringMap.h"
#include <memory>

namespace clang::lifetimes {
namespace internal {
// Forward declarations of internal types.
class Fact;
class FactManager;
class LoanPropagationAnalysis;
struct LifetimeFactory;

/// A generic, type-safe wrapper for an ID, distinguished by its `Tag` type.
/// Used for giving ID to loans and origins.
template <typename Tag> struct ID {
  uint32_t Value = 0;

  bool operator==(const ID<Tag> &Other) const { return Value == Other.Value; }
  bool operator!=(const ID<Tag> &Other) const { return !(*this == Other); }
  bool operator<(const ID<Tag> &Other) const { return Value < Other.Value; }
  ID<Tag> operator++(int) {
    ID<Tag> Tmp = *this;
    ++Value;
    return Tmp;
  }
  void Profile(llvm::FoldingSetNodeID &IDBuilder) const {
    IDBuilder.AddInteger(Value);
  }
};

using LoanID = ID<struct LoanTag>;
using OriginID = ID<struct OriginTag>;

// Using LLVM's immutable collections is efficient for dataflow analysis
// as it avoids deep copies during state transitions.
// TODO(opt): Consider using a bitset to represent the set of loans.
using LoanSet = llvm::ImmutableSet<LoanID>;
using OriginSet = llvm::ImmutableSet<OriginID>;

using ProgramPoint = std::pair<const CFGBlock *, const Fact *>;

/// Running the lifetime safety analysis and querying its results. It
/// encapsulates the various dataflow analyses.
class LifetimeSafetyAnalysis {
public:
  LifetimeSafetyAnalysis(AnalysisDeclContext &AC);
  ~LifetimeSafetyAnalysis();

  void run();

  /// Returns the set of loans an origin holds at a specific program point.
  LoanSet getLoansAtPoint(OriginID OID, ProgramPoint PP) const;

  /// Finds the OriginID for a given declaration.
  /// Returns a null optional if not found.
  std::optional<OriginID> getOriginIDForDecl(const ValueDecl *D) const;

  /// Finds the LoanID for a loan created on a specific variable.
  /// Returns a null optional if not found.
  std::optional<LoanID> getLoanIDForVar(const VarDecl *VD) const;

  llvm::StringMap<ProgramPoint> getTestPoints() const;

private:
  AnalysisDeclContext &AC;
  std::unique_ptr<LifetimeFactory> Factory;
  std::unique_ptr<FactManager> FactMgr;
  std::unique_ptr<LoanPropagationAnalysis> LoanPropagation;
};
} // namespace internal

/// The main entry point for the analysis.
void runLifetimeSafetyAnalysis(AnalysisDeclContext &AC);

} // namespace clang::lifetimes

#endif // LLVM_CLANG_ANALYSIS_ANALYSES_LIFETIMESAFETY_H
