// Verifies that --mlir-obfuscate-private and --mlir-strip-private-pass-metadata
// rewrite or drop pass metadata for passes marked `let isPrivate = 1;`.
//
// RUN: mlir-tblgen -gen-pass-decls -name=Test -I %S/../../include %s \
// RUN:   | FileCheck %s --check-prefix=BASELINE
// RUN: mlir-tblgen -gen-pass-decls -name=Test -I %S/../../include %s \
// RUN:     --mlir-obfuscate-private \
// RUN:   | FileCheck %s --check-prefix=OBFUSCATED
// RUN: mlir-tblgen -gen-pass-decls -name=Test -I %S/../../include %s \
// RUN:     --mlir-obfuscate-private --mlir-strip-private-pass-metadata \
// RUN:   | FileCheck %s --check-prefix=STRIPPED
// RUN: mlir-tblgen -gen-pass-capi-header -prefix=Test -I %S/../../include %s \
// RUN:     --mlir-obfuscate-private --mlir-strip-private-pass-metadata \
// RUN:   | FileCheck %s --check-prefix=CAPI

include "mlir/Pass/PassBase.td"

def PublicTestPass : Pass<"public-test-pass"> {
  let summary = "Public pass summary";
}

def PrivateTestPass : Pass<"private-test-pass"> {
  let summary = "Private pass summary";
  let isPrivate = 1;
  let options = [
    Option<"opt", "private-opt", "int", "0", "Private option description.">
  ];
}

// Per-pass blocks emerge in alphabetical order (Private then Public), and
// registration helpers follow the same order.
//
// Baseline (no flags): names and descriptions are emitted verbatim, both
// passes are registered.
// BASELINE: getArgument() const override { return "private-test-pass"; }
// BASELINE: getDescription() const override { return R"PD(Private pass summary)PD"; }
// BASELINE: getName() const override { return "PrivateTestPass"; }
// BASELINE: ::llvm::cl::desc(R"PO(Private option description.)PO")
// BASELINE: getArgument() const override { return "public-test-pass"; }
// BASELINE: getDescription() const override { return R"PD(Public pass summary)PD"; }
// BASELINE: getName() const override { return "PublicTestPass"; }
// BASELINE: inline void registerPrivateTestPass()
// BASELINE: inline void registerPublicTestPass()

// Obfuscated only: public pass unchanged; private pass keeps its description
// and registration, but its argument and name are opaque.
// OBFUSCATED: getArgument() const override { return "_{{[a-z0-9]+}}"; }
// OBFUSCATED: getDescription() const override { return R"PD(Private pass summary)PD"; }
// OBFUSCATED: getName() const override { return "_{{[a-z0-9]+}}"; }
// OBFUSCATED: getArgument() const override { return "public-test-pass"; }
// OBFUSCATED: getName() const override { return "PublicTestPass"; }
// OBFUSCATED-NOT: return "private-test-pass"
// OBFUSCATED-NOT: return "PrivateTestPass"
// OBFUSCATED: inline void registerPrivateTestPass()
// OBFUSCATED: inline void registerPublicTestPass()

// Stripped + obfuscated: public unchanged; private description, option
// description, argument string, and registration helper are all gone.
// STRIPPED: getArgument() const override { return ""; }
// STRIPPED: getDescription() const override { return R"PD()PD"; }
// STRIPPED: getName() const override { return "_{{[a-z0-9]+}}"; }
// STRIPPED: ::llvm::cl::desc(R"PO()PO")
// STRIPPED: getArgument() const override { return "public-test-pass"; }
// STRIPPED: getName() const override { return "PublicTestPass"; }
// STRIPPED-NOT: Private pass summary
// STRIPPED-NOT: Private option description.
// STRIPPED-NOT: registerPrivateTestPass
// STRIPPED: inline void registerPublicTestPass()

// With stripping, the public C-API entry points are still emitted while
// the private ones disappear from the generated header.
// CAPI: mlirCreateTestPublicTestPass
// CAPI: mlirRegisterTestPublicTestPass
// CAPI-NOT: mlirCreateTestPrivateTestPass
// CAPI-NOT: mlirRegisterTestPrivateTestPass
