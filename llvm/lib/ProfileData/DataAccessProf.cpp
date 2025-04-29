#include "llvm/ProfileData/DataAccessProf.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ProfileData/InstrProf.h"
#include "llvm/Support/Compression.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include <sys/types.h>

namespace llvm {
namespace data_access_prof {
StringRef
DataAccessProfData::saveStringToMap(MapVector<StringRef, uint64_t> &Map,
                                    StringRef Str) {
  auto It = Map.find(Str);
  if (It != Map.end())
    return It->first;
  StringRef savedStr = saver.save(Str);
  Map.insert({savedStr, Map.size()});
  return savedStr;
}

uint64_t DataAccessProfData::addSymbolName(StringRef SymbolName) {
  auto It = SymbolNameIndexMap.find(SymbolName);
  if (It != SymbolNameIndexMap.end())
    return It->second;
  StringRef savedSymName = saver.save(SymbolName);
  auto [Iter, Inserted] =
      SymbolNameIndexMap.insert({savedSymName, SymbolNameIndexMap.size()});
  return Iter->second;
}

StringRef DataAccessProfData::addFileName(StringRef FileName) {
  return saveStringToMap(FileNameIndexMap, FileName);
}

void DataAccessProfData::addSymbolizedDataAccessProfile(StringRef SymbolName,
                                                        uint64_t AccessCount) {
  StringRef CanonicalSymName = InstrProfSymtab::getCanonicalName(SymbolName);
  Records.push_back(DataAccessProfRecord{addSymbolName(CanonicalSymName),
                                         AccessCount,
                                         /*IsStringLiteral=*/false});
}

void DataAccessProfData::addSymbolizedDataAccessProfile(
    uint64_t StringContentHash, uint64_t AccessCount) {
  Records.push_back(DataAccessProfRecord{StringContentHash, AccessCount,
                                         /*IsStringLiteral=*/true});
}

void DataAccessProfData::addSymbolizedDataAccessProfile(
    StringRef SymbolName, uint64_t AccessCount,
    const llvm::SmallVector<DataLocation> &Locations) {
  addSymbolizedDataAccessProfile(SymbolName, AccessCount);

  auto &Record = Records.back();
  for (const auto &Location : Locations) {
    Record.Locations.push_back(
        {addFileName(saver.save(Location.FileName)), Location.Line});
  }
}

void DataAccessProfData::addSymbolizedDataAccessProfile(
    uint64_t StringContentHash, uint64_t AccessCount,
    const llvm::SmallVector<DataLocation> &Locations) {
  addSymbolizedDataAccessProfile(StringContentHash, AccessCount);

  auto &Record = Records.back();
  for (const auto &Location : Locations) {
    Record.Locations.push_back(
        {addFileName(saver.save(Location.FileName)), Location.Line});
  }
}

SmallVector<std::pair<StringRef, uint64_t>>
DataAccessProfData::getSymbolNames() const {
  return llvm::to_vector(
      llvm::make_range(SymbolNameIndexMap.begin(), SymbolNameIndexMap.end()));
}

SmallVector<std::pair<StringRef, uint64_t>>
DataAccessProfData::getFileNames() const {
  return llvm::to_vector(
      llvm::make_range(FileNameIndexMap.begin(), FileNameIndexMap.end()));
}

const SmallVector<DataAccessProfRecord> &
DataAccessProfData::getRecords() const {
  return Records;
}

Error DataAccessProfData::deserialize(const unsigned char *&Ptr) {
  if (Error E = deserializeNames(Ptr, SymbolNameIndexMap))
    return E;
  if (Error E = deserializeNames(Ptr, FileNameIndexMap))
    return E;
  if (Error E = deserializeRecords(Ptr))
    return E;
  return Error::success();
}

static Error writeStrings(
    ProfOStream &OS,
    const llvm::SmallVector<std::pair<llvm::StringRef, uint64_t>> &Strings) {
  std::vector<std::string> StringStrs;
  for (auto [String, Index] : Strings) {
    StringStrs.push_back(String.str());
  }
  std::string CompressedStrings;
  if (!Strings.empty())
    if (Error E = collectGlobalObjectNameStrings(
            StringStrs, compression::zlib::isAvailable(), CompressedStrings))
      return E;
  const uint64_t CompressedStringLen = CompressedStrings.length();
  // Record the length of compressed string.
  OS.write(CompressedStringLen);
  // Write the chars in compressed strings.
  for (auto &c : CompressedStrings)
    OS.writeByte(static_cast<uint8_t>(c));
  // Pad up to a multiple of 8.
  // InstrProfReader could read bytes according to 'CompressedStringLen'.
  const uint64_t PaddedLength = alignTo(CompressedStringLen, 8);
  for (uint64_t K = CompressedStringLen; K < PaddedLength; K++)
    OS.writeByte(0);
  return Error::success();
}

uint64_t DataAccessProfData::getSymbolIndex(
    const std::variant<StringRef, uint64_t> &SymbolID) const {
  if (std::holds_alternative<uint64_t>(SymbolID)) {
    return std::get<uint64_t>(SymbolID);
  }
  StringRef SymbolName = std::get<StringRef>(SymbolID);
  return SymbolNameIndexMap.find(SymbolName)->second;
}

uint64_t DataAccessProfData::getFileNameIndex(StringRef FileName) const {
  return FileNameIndexMap.find(FileName.str())->second;
}

Error DataAccessProfData::serialize(ProfOStream &OS) const {
  if (Error E = writeStrings(OS, getSymbolNames()))
    return E;
  if (Error E = writeStrings(OS, getFileNames()))
    return E;
  OS.write((uint64_t)(Records.size()));
  for (const auto &Rec : Records) {
    OS.write(getSymbolIndex(Rec.SymbolID));
    OS.writeByte(Rec.IsStringLiteral);
    OS.write(Rec.AccessCount);
    OS.write(Rec.Locations.size());
    for (const auto &Loc : Rec.Locations) {
      OS.write(getFileNameIndex(Loc.FileName));
      OS.write32(Loc.Line);
    }
  }
  return Error::success();
}

Error DataAccessProfData::deserializeNames(
    const unsigned char *&Ptr, MapVector<StringRef, uint64_t> &NameIndexMap) {
  uint64_t SymbolNameLen =
      support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);

  StringRef MaybeCompressedSymbolNames((const char *)Ptr, SymbolNameLen);

  std::function<Error(StringRef)> addName = [&](StringRef Name) {
    saveStringToMap(NameIndexMap, Name);
    return Error::success();
  };
  if (Error E = readAndDecodeStrings(MaybeCompressedSymbolNames, addName))
    return E;

  Ptr += alignTo(SymbolNameLen, 8);
  return Error::success();
}

Error DataAccessProfData::deserializeRecords(const unsigned char *&Ptr) {
  auto SymbolNames = this->getSymbolNames();
  auto FileNames = this->getFileNames();

  uint64_t NumRecords =
      support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);

  for (uint64_t I = 0; I < NumRecords; ++I) {
    uint64_t SymbolID =
        support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);

    bool IsStringLiteral =
        support::endian::readNext<uint8_t, llvm::endianness::little>(Ptr);

    uint64_t AccessCount =
        support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);
    if (IsStringLiteral) {
      addSymbolizedDataAccessProfile(SymbolID, AccessCount);
    } else {
      addSymbolizedDataAccessProfile(SymbolNames[SymbolID].first, AccessCount);
    }
    auto &Record = Records.back();

    uint64_t NumLocations =
        support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);

    Record.Locations.reserve(NumLocations);
    for (uint64_t J = 0; J < NumLocations; ++J) {
      uint64_t FileNameIndex =
          support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);
      uint32_t Line =
          support::endian::readNext<uint32_t, llvm::endianness::little>(Ptr);
      Record.Locations.push_back({FileNames[FileNameIndex].first, Line});
    }
  }
  return Error::success();
}
} // namespace data_access_prof
} // namespace llvm
