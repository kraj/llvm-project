//===-- include/flang/Support/PluginDirectives.h ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Compiler directives defined by plugins:
//
//   !DIR$ prefix keyword [ ( arg [, arg]... ) ]
//   arg -> [ name = ] value,  value -> name | integer | character-literal
//
// A plugin loaded with `flang -fc1 -load` registers the directives it defines
// from a static initializer. The parser accepts the form above only for a
// registered prefix; semantics resolves name arguments to symbols and checks
// them against the registered argument kinds; lowering attaches the resolved
// directive to its subject (a procedure or a variable) as an MLIR attribute,
// for the plugin's own passes to interpret.
//
//===----------------------------------------------------------------------===//

#ifndef FORTRAN_SUPPORT_PLUGINDIRECTIVES_H_
#define FORTRAN_SUPPORT_PLUGINDIRECTIVES_H_

#include <string>
#include <string_view>
#include <vector>

namespace Fortran::common {

/// What a directive argument must be.
enum class PluginDirectiveArgKind {
  Procedure, ///< A name resolving to a procedure.
  Variable, ///< A name resolving to a variable.
  Integer, ///< An integer literal.
  String, ///< A character literal, or a name taken as its spelling.
};

struct PluginDirectiveArg {
  /// Empty for a positional argument.
  std::string keyword;
  PluginDirectiveArgKind kind;
  bool required{false};
};

/// What a directive applies to.
enum class PluginDirectiveSubject {
  /// A procedure: the first positional argument if it is given, otherwise
  /// the subprogram whose specification part holds the directive.
  Procedure,
  /// A variable: the first positional argument.
  Variable,
  /// Either, as for Procedure.
  Any,
};

struct PluginDirectiveSpec {
  std::string prefix; ///< Lower case, e.g. "enzyme".
  std::string keyword; ///< Lower case, e.g. "custom_rule".
  PluginDirectiveSubject subject{PluginDirectiveSubject::Procedure};
  /// The arguments after the (optional) positional subject.
  std::vector<PluginDirectiveArg> args;
};

/// Register a directive. Call from a static initializer in a plugin.
void registerPluginDirective(PluginDirectiveSpec spec);

/// Whether a plugin registered a directive with this (lower case) prefix.
bool isPluginDirectivePrefix(std::string_view prefix);

/// The registered directive, or null.
const PluginDirectiveSpec *lookupPluginDirective(
    std::string_view prefix, std::string_view keyword);

} // namespace Fortran::common

#endif // FORTRAN_SUPPORT_PLUGINDIRECTIVES_H_
