//===- DataAccessProf.h - Data access profile format support ---------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains common definitions used in the reading and writing of
// data access profiles.
//
// For the original RFC of this pass please see
// https://discourse.llvm.org/t/rfc-profile-guided-static-data-partitioning/83744
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_PROFILEDATA_DATAACCESSPROF_H_
#define LLVM_PROFILEDATA_DATAACCESSPROF_H_

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ProfileData/InstrProf.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/StringSaver.h"

#include <cstdint>
#include <variant>

namespace llvm {

namespace data_access_prof {
// The location of data in the source code.
struct DataLocation {
  // The filename where the data is located.
  StringRef FileName;
  // The line number in the source code.
  uint32_t Line;
};

// The data access profiles for a symbol.
struct DataAccessProfRecord {
  // Represents a data symbol. The semantic comes in two forms: a symbol index
  // for symbol name if `IsStringLiteral` is false, or the hash of a string
  // content if `IsStringLiteral` is true. Required.
  uint64_t SymbolID;

  // The access count of symbol. Required.
  uint64_t AccessCount;

  // True iff this is a record for string literal (symbols with name pattern
  // `.str.*` in the symbol table). Required.
  bool IsStringLiteral;

  // The locations of data in the source code. Optional.
  llvm::SmallVector<DataLocation> Locations;
};

/// Encapsulates the data access profile data and the methods to operate on it.
/// This class provides profile look-up, serialization and deserialization.
class DataAccessProfData {
public:
  // SymbolID is either a string representing symbol name, or a uint64_t
  // representing the content hash of a string literal.
  using SymbolID = std::variant<StringRef, uint64_t>;
  // Use MapVector to keep input order of strings for serialization and
  // deserialization.
  using StringToIndexMap = llvm::MapVector<StringRef, uint64_t>;

  DataAccessProfData() : saver(Allocator) {}

  /// Serialize profile data to the output stream.
  /// Storage layout:
  /// - The encoded symbol names with sample counts.
  /// - The encoded symbol names with no samples.
  /// - The encoded string literal hashes with no samples.
  /// - Records.
  Error serialize(ProfOStream &OS) const;

  /// Deserialize this class from the given buffer.
  Error deserialize(const unsigned char *&Ptr);

  /// Returns a pointer of profile record for \p SymbolID, or nullptr if there
  /// isn't a record. Internally, this function will canonicalize the symbol
  /// name before the lookup.
  const DataAccessProfRecord *getProfileRecord(const SymbolID SymID) const;

  /// Methods to add symbolized data access profile. Returns error if duplicated
  /// symbol names or content hashes are seen. The user of this class should
  /// aggregate counters that corresponds to the same symbol name or with the
  /// same string literal hash before calling 'add*' methods.
  Error addSymbolizedDataAccessProfile(SymbolID SymbolID, uint64_t AccessCount);
  Error addSymbolizedDataAccessProfile(
      SymbolID SymbolID, uint64_t AccessCount,
      const llvm::SmallVector<DataLocation> &Locations);
  Error addKnownSymbolWithoutSamples(SymbolID SymbolID);


  // Returns a iterable StringRef for strings in the order they are added.
  auto getStrings() const {
    ArrayRef<std::pair<StringRef, uint64_t>> RefSymbolNames(
        StrToIndexMap.begin(), StrToIndexMap.end());
    return llvm::make_first_range(RefSymbolNames);
  }

  // Returns a vector view of the records.
  inline ArrayRef<DataAccessProfRecord> getRecords() const { return Records; }

private:
  /// Given \p Ptr pointing to the start of a blob of strings encoded by
  /// InstrProf.cpp:collectGlobalObjectNameStrings, decode the strings and add
  /// them to \p Map. Increment \p Ptr to the start of next payload. Returns
  /// error if any.
  Error deserializeNames(const unsigned char *&Ptr,
                         MapVector<StringRef, uint64_t> &Map);

  Error deserializeColdKnownSymbols(const unsigned char *&Ptr,
                                      SetVector<StringRef> &SetVector);
  
  Error deserializeColdKnownStringLiteralHashes(const unsigned char *&Ptr,
                                              SetVector<uint64_t> &SetVector);

  /// Given \p Ptr which points to the start of a blob of records, decode the
  /// records and store them. Increment \p Ptr to the start of the next payload.
  /// Returns error if any.
  Error deserializeRecords(const unsigned char *&Ptr);

  /// Returns the index for encoding for \p SymbolID.
  uint64_t getEncodedIndex(const SymbolID SymbolID) const;

  // Key is StringRef representing symbol or file, and value is index.
  StringToIndexMap StrToIndexMap;
  // Key is the canonicalzied symbol name, value is the profile record index.
  DenseMap<StringRef, size_t> SymbolToRecordIndex;
  // Key is the content hash of a string literal, value is profile record index.
  DenseMap<uint64_t, size_t> ContentHashToRecordIndexMap;
  // Stores the records with samples.
  llvm::SmallVector<DataAccessProfRecord> Records;
  llvm::SetVector<uint64_t> ColdKnownStringLiteralHashes;
  llvm::SetVector<StringRef> ColdKnownSymbols;
  // Keeps owned copies of the input strings.
  llvm::BumpPtrAllocator Allocator;
  llvm::UniqueStringSaver saver;
};

} // namespace data_access_prof
} // namespace llvm

#endif // LLVM_PROFILEDATA_DATAACCESSPROF_H_
