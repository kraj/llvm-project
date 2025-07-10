//===-- Symbol.td - Symbol definitions for Offload ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains Offload API definitions related to the symbol handle.
//
//===----------------------------------------------------------------------===//

def : Enum {
  let name = "ol_symbol_kind_t";
  let desc = "The kind of a symbol";
  let etors =[
    Etor<"KERNEL", "a kernel object">,
    Etor<"GLOBAL_VARIABLE", "a global variable">,
  ];
}

def : Function {
    let name = "olGetGlobalVariable";
    let desc = "Get a global variable named `GlobalName` in the given program.";
    let details = [
        "Symbol handles are owned by the program, so do not need to be destroyed."
    ];
    let params = [
        Param<"ol_program_handle_t", "Program", "handle of the program", PARAM_IN>,
        Param<"const char*", "GlobalName", "name of the global variable in the program", PARAM_IN>,
        Param<"ol_symbol_handle_t*", "Global", "output pointer for the fetched global", PARAM_OUT>
    ];
    let returns = [];
}

def : Enum {
  let name = "ol_symbol_info_t";
  let desc = "Supported symbol info.";
  let is_typed = 1;
  let etors = [
    TaggedEtor<"KIND", "ol_symbol_kind_t", "The kind of this symbol.">
  ];
}

def : Function {
  let name = "olGetSymbolInfo";
  let desc = "Queries the given property of the symbol.";
  let details = [
    "`olGetSymbolInfoSize` can be used to query the storage size "
    "required for the given query."
  ];
  let params = [
    Param<"ol_symbol_handle_t", "Event", "handle of the symbol", PARAM_IN>,
    Param<"ol_symbol_info_t", "PropName", "type of the info to retrieve", PARAM_IN>,
    Param<"size_t", "PropSize", "the number of bytes pointed to by PropValue.", PARAM_IN>,
    TypeTaggedParam<"void*", "PropValue", "array of bytes holding the info. "
      "If PropSize is not equal to or greater to the real number of bytes needed to return the info "
      "then the OL_ERRC_INVALID_SIZE error is returned and PropValue is not used.", PARAM_OUT,
      TypeInfo<"PropName" , "PropSize">>
  ];
  let returns = [
    Return<"OL_ERRC_INVALID_SIZE", [
      "`PropSize == 0`",
      "If `PropSize` is less than the real number of bytes needed to return the info."
    ]>,
    Return<"OL_ERRC_SYMBOL_KIND", [
      "If the requested info isn't applicable to the type of symbol."
    ]>,
    Return<"OL_ERRC_INVALID_SYMBOL">
  ];
}

def : Function {
  let name = "olGetSymbolInfoSize";
  let desc = "Returns the storage size of the given symbol query.";
  let details = [];
  let params = [
    Param<"ol_symbol_handle_t", "Event", "handle of the symbol", PARAM_IN>,
    Param<"ol_symbol_info_t", "PropName", "type of the info to query", PARAM_IN>,
    Param<"size_t*", "PropSizeRet", "pointer to the number of bytes required to store the query", PARAM_OUT>
  ];
  let returns = [
    Return<"OL_ERRC_INVALID_SYMBOL">,
    Return<"OL_ERRC_SYMBOL_KIND", [
      "If the requested info isn't applicable to the type of symbol."
    ]>,
  ];
}
