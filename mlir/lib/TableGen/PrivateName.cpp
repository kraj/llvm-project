//===- PrivateName.cpp - Private name obfuscation for ODS -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mlir/TableGen/PrivateName.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/SipHash.h"

#include <array>
#include <cstdint>
#include <string>

using namespace mlir;
using namespace mlir::tblgen;

namespace {
/// Lazy state for private-name obfuscation. Wrapped in a function-local
/// static so we don't introduce any global constructors (the MLIRTableGen
/// library is built with `-Werror=global-constructors` in some
/// configurations).
struct State {
  bool obfuscate = false;
  bool strip = false;
  std::array<uint8_t, 16> salt{};
  llvm::StringMap<std::string> cache;
};

State &state() {
  static State s;
  return s;
}

uint8_t hexDigit(char c, bool &ok) {
  if (c >= '0' && c <= '9')
    return static_cast<uint8_t>(c - '0');
  if (c >= 'a' && c <= 'f')
    return static_cast<uint8_t>(10 + (c - 'a'));
  if (c >= 'A' && c <= 'F')
    return static_cast<uint8_t>(10 + (c - 'A'));
  ok = false;
  return 0;
}

/// Base32 alphabet (RFC 4648 lowercase variant). Each character encodes 5
/// bits. The result is restricted to `[a-z0-9]` so the obfuscated mnemonic
/// is a valid C++ identifier suffix and a valid MLIR mnemonic.
constexpr char kBase32Alphabet[] = "abcdefghijklmnopqrstuvwxyz012345";

/// Encodes the low `numChars * 5` bits of `hash` as base32. The result is
/// always prefixed with `_` so that the encoded form is a valid bare
/// identifier (which must start with a letter or underscore).
std::string encodeBase32(uint64_t hash, unsigned numChars) {
  std::string out;
  out.reserve(1 + numChars);
  out.push_back('_');
  for (unsigned i = 0; i < numChars; ++i) {
    out.push_back(kBase32Alphabet[hash & 0x1f]);
    hash >>= 5;
  }
  return out;
}
} // namespace

void mlir::tblgen::setObfuscatePrivateNames(bool enabled) {
  state().obfuscate = enabled;
}

void mlir::tblgen::setStripPrivatePassMetadata(bool enabled) {
  state().strip = enabled;
}

void mlir::tblgen::setObfuscationSalt(StringRef hex) {
  if (hex.starts_with("0x") || hex.starts_with("0X"))
    hex = hex.drop_front(2);

  auto &s = state();
  s.salt.fill(0);

  bool ok = true;
  for (size_t i = 0, e = hex.size(); i < e && (i / 2) < s.salt.size(); ++i) {
    uint8_t nibble = hexDigit(hex[i], ok);
    if ((i % 2) == 0)
      s.salt[i / 2] = static_cast<uint8_t>(nibble << 4);
    else
      s.salt[i / 2] = static_cast<uint8_t>(s.salt[i / 2] | nibble);
  }
  if (!ok)
    llvm::report_fatal_error(
        "--mlir-obfuscation-salt must be a hex-encoded string");

  // Salt change invalidates any previously cached obfuscations.
  s.cache.clear();
}

bool mlir::tblgen::obfuscatePrivateNamesEnabled() { return state().obfuscate; }

bool mlir::tblgen::stripPrivatePassMetadataEnabled() { return state().strip; }

StringRef mlir::tblgen::obfuscatePrivateName(StringRef name) {
  if (name.empty())
    return name;

  auto &s = state();
  if (auto it = s.cache.find(name); it != s.cache.end())
    return it->second;

  uint8_t out[8] = {};
  uint8_t (&saltKey)[16] =
      *reinterpret_cast<uint8_t (*)[16]>(s.salt.data());
  llvm::getSipHash_2_4_64(
      llvm::ArrayRef<uint8_t>(reinterpret_cast<const uint8_t *>(name.data()),
                              name.size()),
      saltKey, out);
  uint64_t hash = 0;
  for (unsigned i = 0; i < 8; ++i)
    hash |= static_cast<uint64_t>(out[i]) << (i * 8);

  // 12 chars * 5 bits = 60 bits of entropy. Plenty of headroom against
  // collisions for the typical few-thousand-mnemonic dialect.
  std::string obf = encodeBase32(hash, 12);
  auto inserted = s.cache.try_emplace(name, std::move(obf));
  return inserted.first->second;
}

std::string mlir::tblgen::maybeObfuscateDotted(StringRef name, bool isPrivate) {
  if (!isPrivate || !obfuscatePrivateNamesEnabled())
    return std::string(name);
  size_t dot = name.find('.');
  if (dot == StringRef::npos)
    return obfuscatePrivateName(name).str();
  StringRef dialect = name.substr(0, dot);
  StringRef rest = name.substr(dot + 1);
  std::string result = obfuscatePrivateName(dialect).str();
  result.push_back('.');
  result += obfuscatePrivateName(rest).str();
  return result;
}
