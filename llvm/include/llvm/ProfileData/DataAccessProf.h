#ifndef LLVM_PROFILEDATA_DATAACCESSPROF_H_
#define LLVM_PROFILEDATA_DATAACCESSPROF_H_

#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ProfileData/InstrProf.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Error.h"

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
  // Represents a data symbol. The semantic comes in two forms: the md5 hash of
  // the canonicalized symbol name if `IsStringLiteral` is false, or the hash of
  // a string content if `IsStringLiteral` is true. Required.
  uint64_t SymbolID;

  // The access count of symbol. Required.
  uint64_t AccessCount;

  bool IsStringLiteral;

  // The locations of data in the source code. Optional.
  llvm::SmallVector<DataLocation> Locations;
};

/// Encapsulates the data access profile data and the methods to operate on it.
/// It provides profile look-up using symbol ID, serialization and
/// deserialization.
class DataAccessProfData {
  Error deserializeNames(const unsigned char *&Ptr,
                         MapVector<StringRef, uint64_t> &Names);

  Error deserializeSymbolNames(const unsigned char *&Ptr,
                               DenseMap<uint64_t, StringRef> &Names);

  Error deserializeRecords(const unsigned char *&Ptr);

  StringRef saveStringToMap(llvm::MapVector<StringRef, uint64_t> &Map,
                            StringRef Str);
  uint64_t addSymbolName(StringRef SymbolName);
  StringRef addFileName(StringRef FileName);

  uint64_t getSymbolIndex(const std::variant<StringRef, uint64_t> &) const;

  uint64_t getFileNameIndex(StringRef FileName) const;

public:
  DataAccessProfData() : saver(Allocator) {}

  using StringToIndexMapVector = llvm::MapVector<StringRef, uint64_t>;
  StringToIndexMapVector SymbolNameIndexMap;
  StringToIndexMapVector FileNameIndexMap;

  llvm::SmallVector<DataAccessProfRecord> Records;

  // Keeps copies of the strings.
  llvm::BumpPtrAllocator Allocator;
  llvm::StringSaver saver;

  // Deserialize profiles to the given buffer.
  Error deserialize(const unsigned char *&Ptr);

  // Parse profile data from the given buffer.
  Error serialize(ProfOStream &OS) const;

  void addSymbolizedDataAccessProfile(StringRef SymbolName,
                                      uint64_t AccessCount);
  void addSymbolizedDataAccessProfile(uint64_t StringContentHash,
                                      uint64_t AccessCount);
  void addSymbolizedDataAccessProfile(
      StringRef SymbolName, uint64_t AccessCount,
      const llvm::SmallVector<DataLocation> &Locations);
  void addSymbolizedDataAccessProfile(
      uint64_t StringContentHash, uint64_t AccessCount,
      const llvm::SmallVector<DataLocation> &Locations);

  SmallVector<llvm::MapVector<StringRef, uint64_t>::value_type>
  getSymbolNames() const;

  SmallVector<llvm::MapVector<StringRef, uint64_t>::value_type>
  getFileNames() const;

  const SmallVector<DataAccessProfRecord> &getRecords() const;
};

} // namespace data_access_prof
} // namespace llvm

#endif // LLVM_PROFILEDATA_DATAACCESSPROF_H_
