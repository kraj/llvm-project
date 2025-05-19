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

class DataAccessPerfReader {
public:
  DataAccessPerfReader(ProfiledBinary *Binary, ArrayRef<StringRef> PerfTraces,
                       std::optional<int32_t> PID) : Binary(Binary), PerfTraces(PerfTraces), PIDFilter(PID) {}

  // Entry of the reader to parse multiple perf traces
  void parsePerfTraces();

  auto getAddressToCount() const {
    return AddressToCount.getArrayRef();
  }

  void print() const {
    for (const auto &Entry : AddressToCount) {
      dbgs() << "Address: " << format("0x%llx", Entry.first)
             << ", Count: " << Entry.second << "\n";
    }
  }

private:
  void parsePerfTrace(StringRef PerfTrace);

  MapVector<uint64_t, uint64_t> AddressToCount;


  ProfiledBinary *Binary = nullptr;
  ArrayRef<StringRef> PerfTraces;
  std::optional<int32_t> PIDFilter;
};

} // namespace llvm

#endif // LLVM_TOOLS_LLVM_PROFGEN_DATAACCESSPERFREADER_H
