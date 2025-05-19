#include "DataAccessPerfReader.h"
#include "ErrorHandling.h"
#include "llvm/Support/LineIterator.h"
#include "llvm/Support/MemoryBuffer.h"

constexpr static const char* PerfRecordSamplePattern = 
        ".*?PERF_RECORD_SAMPLE\\([^)]*\\):\\s*" // Match up to and including "PERF_RECORD_SAMPLE(...): "
        "(\\d+)/(\\d+):\\s*"                  // Capture group 1 (process-id) & 2 (thread-id), followed by colon and space
        "(0x[0-9a-fA-F]+)"                    // Capture group 3: Hex value 2 (e.g., 0x254fb6)
        "\\s+period:\\s*\\d+\\s+addr:\\s*"    // Match " period: <number> addr: "
        "(0x[0-9a-fA-F]+)";                    // Capture group 4: Address (e.g., 0x7ffd241eb718)

static llvm::Regex PerfRecordSampleRegex(PerfRecordSamplePattern);

namespace llvm {

void DataAccessPerfReader::parsePerfTraces() {
  for (StringRef PerfTrace : PerfTraces) {
    parsePerfTrace(PerfTrace);
  }
}

// Ignore mmap events.
void DataAccessPerfReader::parsePerfTrace(StringRef PerfTrace) {
  auto BufferOrErr = MemoryBuffer::getFile(PerfTrace);
  std::error_code EC = BufferOrErr.getError();
  if (EC)
    exitWithError("Failed to open perf trace file: " + PerfTrace);

  SmallVector<StringRef, 4> Matches;
  line_iterator LineIt(*BufferOrErr.get(), true);
  for (; !LineIt.is_at_eof(); ++LineIt) {
    StringRef Line = *LineIt;
    
    if (PerfRecordSampleRegex.match(Line, &Matches)) {
      errs() << "Matched size: " << Matches.size() << "\n";

      // Check if the PID matches the filter.
      int32_t PID = std::stoi(Matches[0].str());
      if (PIDFilter && *PIDFilter != PID) {
        continue;
      }

      // Extract the address and count.
      uint64_t DataAddress = std::stoull(Matches[3].str(), nullptr, 16);
      uint64_t IP = std::stoull(Matches[2].str(), nullptr, 16);

      AddressToCount[DataAddress] += 1;
    }
  }
}


}  // namespace llvm
