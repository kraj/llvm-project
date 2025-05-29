//===-- DataAccessPerfReader.h - perfscript reader for data access profiles -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TOOLS_LLVM_PROFGEN_DATAACCESSPERFREADER_H
#define LLVM_TOOLS_LLVM_PROFGEN_DATAACCESSPERFREADER_H

#include "PerfReader.h"
#include "ProfiledBinary.h"
#include "llvm/ADT/MapVector.h"

namespace llvm {

class DataAccessPerfReader : public PerfScriptReader {
public:
  class DataSegment {
  public:
    uint64_t FileOffset;
    uint64_t VirtualAddress;
  };
  DataAccessPerfReader(ProfiledBinary *Binary, StringRef PerfTrace,
                       std::optional<int32_t> PID)
      : PerfScriptReader(Binary, PerfTrace, PID), PerfTraceFilename(PerfTrace) {
    hackMMapEventAndDataSegment(MMap, DataSegment, *Binary);
  }

  // The MMapEvent is hard-coded as a hack to illustrate the change.
  static void
  hackMMapEventAndDataSegment(PerfScriptReader::MMapEvent &MMap,
                              DataSegment &DataSegment,
                              const ProfiledBinary &ProfiledBinary) {
    // The PERF_RECORD_MMAP2 event is
    // 0 0x4e8 [0xa0]: PERF_RECORD_MMAP2 1849842/1849842:
    // [0x55d977426000(0x1000) @ 0x1000 fd:01 20869534 0]: r--p /path/to/binary
    MMap.PID = 1849842; // Example PID
    MMap.BinaryPath = ProfiledBinary.getPath();
    MMap.Address = 0x55d977426000;
    MMap.Size = 0x1000;
    MMap.Offset = 0x1000; // File Offset in the binary.

    // TODO: Set binary fields to do address canonicalization, and compute
    // static data address range.
    DataSegment.FileOffset =
        0x1180; // The byte offset of the segment start in the binary.
    DataSegment.VirtualAddress =
        0x3180; // The virtual address of the segment start in the binary.
  }

  uint64_t canonicalizeDataAddress(uint64_t Address,
                                   const ProfiledBinary &ProfiledBinary,
                                   const PerfScriptReader::MMapEvent &MMap,
                                   const DataSegment &DataSegment) {
    // virtual-addr = segment.virtual-addr (0x3180) + (runtime-addr -
    // map.adddress - segment.file-offset (0x1180) + map.file-offset (0x1000))
    return DataSegment.VirtualAddress +
           (Address - MMap.Address - DataSegment.FileOffset + MMap.Offset);
  }

  // Entry of the reader to parse multiple perf traces
  void parsePerfTraces() override;

  auto getAddressToCount() const {
    return AddressToCount.getArrayRef();
  }

  void print() const {
    auto addrCountArray = AddressToCount.getArrayRef();
    std::vector<std::pair<uint64_t, uint64_t>> SortedEntries(
        addrCountArray.begin(), addrCountArray.end());
    llvm::sort(SortedEntries, [](const auto &A, const auto &B) {
      return A.second > B.second;
    });
    for (const auto &Entry : SortedEntries) {
      if (Entry.second == 0)
        continue; // Skip entries with zero count
      dbgs() << "Address: " << format("0x%llx", Entry.first)
             << ", Count: " << Entry.second << "\n";
    }
  }

private:
  void parsePerfTrace(StringRef PerfTrace);

  MapVector<uint64_t, uint64_t> AddressToCount;

  StringRef PerfTraceFilename;

  PerfScriptReader::MMapEvent MMap;
  DataSegment DataSegment;
};

} // namespace llvm

#endif // LLVM_TOOLS_LLVM_PROFGEN_DATAACCESSPERFREADER_H
