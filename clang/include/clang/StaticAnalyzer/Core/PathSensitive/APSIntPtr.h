//== APSIntPtr.h - Wrapper for APSInt objects owned separately -*- C++ -*--==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_APSIntPtr_H
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_APSIntPtr_H

#include "llvm/ADT/APSInt.h"

namespace clang::ento {
class BasicValueFactory;

/// A safe wrapper around APSInt objects allocated and owned by
/// \c BasicValueFactory. This just wraps a common llvm::APSInt.
class APSIntPtr {
  using APSInt = llvm::APSInt;

public:
  APSIntPtr() = delete;
  APSIntPtr(const APSIntPtr &) = default;
  APSIntPtr &operator=(const APSIntPtr &) & = default;
  ~APSIntPtr() = default;

  /// You should not use this API.
  /// If do, ensure that the \p Ptr not going to dangle.
  /// Prefer using \c BasicValueFactory::getValue() to get an APSIntPtr object.
  static APSIntPtr unsafeConstructor(const APSInt *Ptr) {
    return APSIntPtr(Ptr);
  }

  const APSInt *get() const { return Ptr; }
  /*implicit*/ operator const APSInt &() const { return *get(); }

  APSInt operator-() const { return -*Ptr; }
  APSInt operator~() const { return ~*Ptr; }

#define DEFINE_OPERATOR(OP)                                                    \
  bool operator OP(APSIntPtr Other) const { return (*Ptr)OP(*Other.Ptr); }
  DEFINE_OPERATOR(>)
  DEFINE_OPERATOR(>=)
  DEFINE_OPERATOR(<)
  DEFINE_OPERATOR(<=)
  DEFINE_OPERATOR(==)
  DEFINE_OPERATOR(!=)
#undef DEFINE_OPERATOR

  const APSInt &operator*() const { return *Ptr; }
  const APSInt *operator->() const { return Ptr; }

private:
  friend class BasicValueFactory;
  explicit APSIntPtr(const APSInt *Ptr) : Ptr(Ptr) {}

  /// Owned by \c BasicValueFactory.
  const APSInt *Ptr;
};

} // namespace clang::ento

#endif // LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_APSIntPtr_H
