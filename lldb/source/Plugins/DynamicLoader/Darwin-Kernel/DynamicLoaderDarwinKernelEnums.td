include "../../../../include/lldb/Core/EnumeratorBase.td"

def kaslr_kernel_scan_enum_values : EnumValues<"eKASLRScan", [
  EnumValue<"None", "none",
    "Do not read memory looking for a Darwin kernel when attaching.">,
  EnumValue<"LowgloAddresses", "basic",
    "Check for the Darwin kernel's load addr in the lowglo page "
    "(boot-args=debug) only.">,
  EnumValue<"NearPC", "fast-scan",
    "Scan near the pc value on attach to find the Darwin kernel's load "
    "address.">,
  EnumValue<"ExhaustiveScan", "exhaustive-scan",
    "Scan through the entire potential address range of Darwin kernel "
    "(only on 32-bit targets).">,
]>;
