include "../../../../include/lldb/Core/EnumeratorBase.td"

def pdb_reader_enums : EnumValues<"ePDBReader", [
  EnumValue<"Default", "default",
    "Use native PDB reader unless LLDB_USE_NATIVE_PDB_READER environment is "
    "set to 0">,
  EnumValue<"DIA", "dia", "Use DIA PDB reader">,
  EnumValue<"Native", "native", "Use native PDB reader">,
]>;
