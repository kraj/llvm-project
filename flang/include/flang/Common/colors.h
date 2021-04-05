//===-- include/flang/Parser/colors.h ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Defines colors used for diagnostics.
//
//===----------------------------------------------------------------------===//

#ifndef FORTRAN_PARSER_COLORS_H_
#define FORTRAN_PARSER_COLORS_H_

#include "llvm/Support/raw_ostream.h"

namespace Fortran::common {

static constexpr enum llvm::raw_ostream::Colors noteColor =
    llvm::raw_ostream::BLACK;
static constexpr enum llvm::raw_ostream::Colors remarkColor =
    llvm::raw_ostream::BLUE;
static constexpr enum llvm::raw_ostream::Colors warningColor =
    llvm::raw_ostream::MAGENTA;
static constexpr enum llvm::raw_ostream::Colors errorColor = llvm::raw_ostream::RED;
static constexpr enum llvm::raw_ostream::Colors fatalColor = llvm::raw_ostream::RED;
// Used for changing only the bold attribute.
static constexpr enum llvm::raw_ostream::Colors savedColor =
    llvm::raw_ostream::SAVEDCOLOR;
    
} // namespace Fortran::common

#endif