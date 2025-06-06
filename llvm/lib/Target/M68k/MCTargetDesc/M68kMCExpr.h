//===- M68k specific MC expression classes ----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The MCTargetExpr subclass describes a relocatable expression with a
// M68k-specific relocation specifier.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_M68K_MCTARGETDESC_M68KMCEXPR_H
#define LLVM_LIB_TARGET_M68K_MCTARGETDESC_M68KMCEXPR_H

#include "llvm/MC/MCExpr.h"

namespace llvm {

class M68kMCExpr : public MCSpecifierExpr {
public:
  enum Specifier {
    VK_None,

    VK_GOT = MCSymbolRefExpr::FirstTargetSpecifier,
    VK_GOTOFF,
    VK_GOTPCREL,
    VK_GOTTPOFF,
    VK_PLT,
    VK_TLSGD,
    VK_TLSLD,
    VK_TLSLDM,
    VK_TPOFF,
  };

protected:
  explicit M68kMCExpr(const MCExpr *Expr, Specifier S)
      : MCSpecifierExpr(Expr, S) {}

public:
  static const M68kMCExpr *create(const MCExpr *, Specifier, MCContext &);

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
};
} // namespace llvm

#endif
