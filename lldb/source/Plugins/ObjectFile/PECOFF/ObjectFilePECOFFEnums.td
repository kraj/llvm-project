include "../../../../include/lldb/Core/EnumeratorBase.td"

def abi_enums : EnumValues<"llvm::Triple::", [
  EnumValue<"UnknownEnvironment", "default",
    "Use default target (if it is Windows) or MSVC">,
  EnumValue<"MSVC", "msvc", "MSVC ABI">,
  EnumValue<"GNU", "gnu", "MinGW / Itanium ABI">,
]>;
