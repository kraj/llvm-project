//===- PrivateName.h - Private name obfuscation for ODS ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Helpers for ODS-driven obfuscation and stripping of "private" dialect,
// operation, attribute, type, and pass names. The mlir-tblgen tool driver
// configures the helper via the command-line flags `--mlir-obfuscate-private`,
// `--mlir-obfuscation-salt`, and `--mlir-strip-private-pass-metadata`.
//
//===----------------------------------------------------------------------===//

#ifndef MLIR_TABLEGEN_PRIVATENAME_H_
#define MLIR_TABLEGEN_PRIVATENAME_H_

#include "mlir/Support/LLVM.h"
#include "llvm/ADT/StringRef.h"

#include <string>

namespace mlir {
namespace tblgen {

/// Configuration setters. The mlir-tblgen tool driver calls these after
/// parsing the command line; downstream tools that embed this library can
/// also set these directly.
void setObfuscatePrivateNames(bool enabled);
void setStripPrivatePassMetadata(bool enabled);

/// Sets the SipHash key used for obfuscation. `hexSalt` is parsed as up to
/// 32 hex characters (16 bytes); missing nibbles are zero-padded; extra
/// nibbles are truncated. A leading "0x"/"0X" is allowed.
void setObfuscationSalt(StringRef hexSalt);

/// Returns true if private-name obfuscation is enabled.
bool obfuscatePrivateNamesEnabled();

/// Returns true if private-pass metadata stripping is enabled.
bool stripPrivatePassMetadataEnabled();

/// Returns the obfuscated form of `name`, computed deterministically from
/// the configured salt using SipHash-2-4-64 truncated to 60 bits and base32
/// encoded with a leading underscore (so the result is a valid identifier
/// and a valid MLIR mnemonic). The returned StringRef is stable for the
/// lifetime of the process.
StringRef obfuscatePrivateName(StringRef name);

/// Returns either `name` (when not private or obfuscation is disabled) or
/// `obfuscatePrivateName(name)`.
inline StringRef maybeObfuscate(StringRef name, bool isPrivate) {
  if (!isPrivate || !obfuscatePrivateNamesEnabled())
    return name;
  return obfuscatePrivateName(name);
}

/// For a dotted name "dialect.mnemonic", obfuscates the dialect prefix and
/// the mnemonic suffix independently and rejoins them with a dot. This keeps
/// runtime parsing of the dialect-prefix in `OperationName` working. Names
/// without a '.' are obfuscated as-is.
std::string maybeObfuscateDotted(StringRef name, bool isPrivate);

} // namespace tblgen
} // namespace mlir

#endif // MLIR_TABLEGEN_PRIVATENAME_H_
