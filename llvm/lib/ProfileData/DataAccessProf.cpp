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
  assert(!FileName.empty() && "File name should not be empty");
  return saveStringToMap(FileNameIndexMap, FileName);
}

const DataAccessProfRecord *
DataAccessProfData::getProfileRecord(const SymbolID SymbolID) const {
  if (std::holds_alternative<uint64_t>(SymbolID)) {
    auto It = ContentHashToRecordIndexMap.find(std::get<uint64_t>(SymbolID));
    if (It == ContentHashToRecordIndexMap.end())
      return nullptr;
    return &Records[It->second];
  }
  StringRef SymbolName =
      InstrProfSymtab::getCanonicalName(std::get<StringRef>(SymbolID));
  auto It = SymbolNameIndexMap.find(SymbolName);
  if (It == SymbolNameIndexMap.end())
    return nullptr;
  return &Records[It->second];
}

Error DataAccessProfData::addSymbolizedDataAccessProfile(StringRef SymbolName,
                                                         uint64_t AccessCount) {
  if (SymbolName.empty())
    return make_error<StringError>("Empty symbol name",
                                   llvm::errc::invalid_argument);
  StringRef CanonicalSymName = InstrProfSymtab::getCanonicalName(SymbolName);
  const uint64_t SymIndex = addSymbolName(CanonicalSymName);

  if (SymbolIndexToRecordIndexMap.count(SymIndex) > 0)
    return make_error<StringError>(
        "Duplicate symbol added. User of DataAccessProfData should "
        "aggregate count for the same symbol. ",
        llvm::errc::invalid_argument);

  Records.push_back(DataAccessProfRecord{SymIndex, AccessCount,
                                         /*IsStringLiteral=*/false});
  SymbolIndexToRecordIndexMap[SymIndex] = Records.size() - 1;
  return Error::success();
}

Error DataAccessProfData::addSymbolizedDataAccessProfile(
    uint64_t StringContentHash, uint64_t AccessCount) {
  if (ContentHashToRecordIndexMap.count(StringContentHash) > 0)
    return make_error<StringError>(
        "Duplicate string literal added. User of DataAccessProfData should "
        "aggregate count for the same string literal. ",
        llvm::errc::invalid_argument);

  Records.push_back(DataAccessProfRecord{StringContentHash, AccessCount,
                                         /*IsStringLiteral=*/true});
  ContentHashToRecordIndexMap[StringContentHash] = Records.size() - 1;
  return Error::success();
}

Error DataAccessProfData::addSymbolizedDataAccessProfile(
    StringRef SymbolName, uint64_t AccessCount,
    const llvm::SmallVector<DataLocation> &Locations) {

  if (Error E = addSymbolizedDataAccessProfile(SymbolName, AccessCount))
    return E;

  auto &Record = Records.back();
  for (const auto &Location : Locations) {
    Record.Locations.push_back(
        {addFileName(saver.save(Location.FileName)), Location.Line});
  }
  return Error::success();
}

Error DataAccessProfData::addSymbolizedDataAccessProfile(
    uint64_t StringContentHash, uint64_t AccessCount,
    const llvm::SmallVector<DataLocation> &Locations) {
  if (Error E = addSymbolizedDataAccessProfile(StringContentHash, AccessCount))
    return E;

  auto &Record = Records.back();
  for (const auto &Location : Locations) {
    Record.Locations.push_back(
        {addFileName(saver.save(Location.FileName)), Location.Line});
  }
  return Error::success();
}

ArrayRef<DataAccessProfRecord> DataAccessProfData::getRecords() const {
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

template <typename StringRefIterators>
static Error writeStrings(ProfOStream &OS, StringRefIterators Strings) {
  std::vector<std::string> StringStrs;
  for (StringRef String : Strings)
    StringStrs.push_back(String.str());

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

uint64_t DataAccessProfData::getSymbolIndex(const SymbolID SymbolID) const {
  if (std::holds_alternative<uint64_t>(SymbolID))
    return std::get<uint64_t>(SymbolID);

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
  SmallVector<StringRef> SymbolNames = llvm::to_vector(getSymbolNames());
  SmallVector<StringRef> FileNames = llvm::to_vector(getFileNames());

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
      if (Error E = addSymbolizedDataAccessProfile(SymbolID, AccessCount))
        return E;
    } else if (Error E = addSymbolizedDataAccessProfile(SymbolNames[SymbolID],
                                                        AccessCount))
      return E;

    auto &Record = Records.back();

    uint64_t NumLocations =
        support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);

    Record.Locations.reserve(NumLocations);
    for (uint64_t J = 0; J < NumLocations; ++J) {
      uint64_t FileNameIndex =
          support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);
      uint32_t Line =
          support::endian::readNext<uint32_t, llvm::endianness::little>(Ptr);
      Record.Locations.push_back({FileNames[FileNameIndex], Line});
    }
  }
  return Error::success();
}
} // namespace data_access_prof
} // namespace llvm
