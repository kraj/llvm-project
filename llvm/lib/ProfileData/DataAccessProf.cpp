#include "llvm/ProfileData/DataAccessProf.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ProfileData/InstrProf.h"
#include "llvm/Support/Compression.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/raw_ostream.h"
#include <sys/types.h>

namespace llvm {
namespace data_access_prof {

/// If \p Map has an entry keyed by \p Str, returns the entry iterator.
/// Otherwise, creates an owned copy of \p Str, adds a map entry for it and
/// returns the iterator.
static MapVector<StringRef, uint64_t>::iterator
saveStringToMap(MapVector<StringRef, uint64_t> &Map,
                llvm::UniqueStringSaver &saver, StringRef Str) {
  auto [Iter, Inserted] = Map.try_emplace(saver.save(Str), Map.size());
  return Iter;
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
  auto It = SymbolToRecordIndex.find(SymbolName);
  if (It == SymbolToRecordIndex.end())
    return nullptr;
  return &Records[It->second];
}

Error DataAccessProfData::addSymbolizedDataAccessProfile(StringRef SymbolName,
                                                         uint64_t AccessCount) {
  if (SymbolName.empty())
    return make_error<StringError>("Empty symbol name",
                                   llvm::errc::invalid_argument);

  StringRef CanonicalSymName = InstrProfSymtab::getCanonicalName(SymbolName);
  const uint64_t SymIndex =
      saveStringToMap(StrToIndexMap, saver, CanonicalSymName)->second;

  auto [Iter, Inserted] =
      SymbolToRecordIndex.try_emplace(CanonicalSymName, Records.size());
  if (!Inserted)
    return make_error<StringError>(
        "Duplicate symbol added. User of DataAccessProfData should "
        "aggregate count for the same symbol. ",
        llvm::errc::invalid_argument);
  Records.push_back(DataAccessProfRecord{SymIndex, AccessCount,
                                         /*IsStringLiteral=*/false});

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
        {saveStringToMap(StrToIndexMap, saver, Location.FileName)->first,
         Location.Line});
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
        {saveStringToMap(StrToIndexMap, saver, Location.FileName)->first,
         Location.Line});
  }
  return Error::success();
}

Error DataAccessProfData::deserialize(const unsigned char *&Ptr) {
  if (Error E = deserializeNames(Ptr, StrToIndexMap))
    return E;
  return deserializeRecords(Ptr);
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

uint64_t DataAccessProfData::getEncodedIndex(const SymbolID SymbolID) const {
  if (std::holds_alternative<uint64_t>(SymbolID))
    return std::get<uint64_t>(SymbolID);

  return StrToIndexMap.find(std::get<StringRef>(SymbolID))->second;
}

Error DataAccessProfData::serialize(ProfOStream &OS) const {
  if (Error E = writeStrings(OS, getStrings()))
    return E;
  OS.write((uint64_t)(Records.size()));
  for (const auto &Rec : Records) {
    OS.write(getEncodedIndex(Rec.SymbolID));
    OS.writeByte(Rec.IsStringLiteral);
    OS.write(Rec.AccessCount);
    OS.write(Rec.Locations.size());
    for (const auto &Loc : Rec.Locations) {
      OS.write(getEncodedIndex(Loc.FileName));
      OS.write32(Loc.Line);
    }
  }
  return Error::success();
}

Error DataAccessProfData::deserializeNames(
    const unsigned char *&Ptr, MapVector<StringRef, uint64_t> &NameIndexMap) {
  uint64_t Len =
      support::endian::readNext<uint64_t, llvm::endianness::little>(Ptr);

  std::function<Error(StringRef)> addName = [&](StringRef Name) {
    saveStringToMap(NameIndexMap, saver, Name);
    return Error::success();
  };
  if (Error E =
          readAndDecodeStrings(StringRef((const char *)Ptr, Len), addName))
    return E;

  Ptr += alignTo(Len, 8);
  return Error::success();
}

Error DataAccessProfData::deserializeRecords(const unsigned char *&Ptr) {
  SmallVector<StringRef> Strings = llvm::to_vector(getStrings());

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
    } else if (Error E = addSymbolizedDataAccessProfile(Strings[SymbolID],
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
      Record.Locations.push_back({Strings[FileNameIndex], Line});
    }
  }
  return Error::success();
}
} // namespace data_access_prof
} // namespace llvm
