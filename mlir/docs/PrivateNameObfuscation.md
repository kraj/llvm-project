# Private Name Obfuscation and Stripping

[TOC]

This page documents an opt-in TableGen + CMake mechanism for replacing the
human-readable names of dialects, operations, attributes, types, and passes
in a built MLIR binary with deterministic opaque identifiers, and for
dropping pass description / CLI registration metadata. The intent is to make
it harder to reverse-engineer a release binary while preserving a
fully-readable internal/test build from the same source tree.

## What gets obfuscated

When `-DMLIR_ENABLE_PRIVATE_NAME_OBFUSCATION=ON` is set and a TableGen
definition declares `let isPrivate = 1;`, mlir-tblgen emits the following
literals using a deterministic 12-character base32 hash of the original
identifier (computed with SipHash-2-4-64 and a build-time salt) instead of
the original spelling:

| Source                     | Affected literal(s)                                          |
| -------------------------- | ------------------------------------------------------------ |
| `Dialect`                  | `getDialectNamespace()`, dialect constructor                 |
| `Op`                       | `getOperationName()`, adaptor `odsOpName`, error prefixes    |
| `AttrDef`, `TypeDef`       | `getMnemonic()`, `name`, `dialectName`, alias `getAlias`     |
| `PassBase` / `Pass`        | `getArgument()`, `getArgumentName()`, `getName()`, `getPassName()` |

The hash is split at the dot for op / attribute / type names, so the
dialect prefix and the mnemonic are obfuscated independently and the
runtime helper `OperationName::getDialectNamespace()` (which splits at the
first `.`) keeps working.

Hashes are **deterministic** for a given salt, so all translation units
within one build agree on the obfuscated spelling. Pattern matching,
`ConversionTarget`, the bytecode reader/writer (which uses whatever
`getDialectNamespace()` / `getOperationName()` return), and dialect
registration all keep working without source changes.

Private ops also behave as if no `assemblyFormat` or
`hasCustomAssemblyFormat` was specified when private-name obfuscation is
enabled. ODS does not generate the custom/declarative `parse` and `print`
methods for those ops. They still print in generic form, using the
obfuscated operation name, and their custom textual syntax is rejected.

## What gets stripped (passes only)

When `-DMLIR_STRIP_PASS_METADATA=ON` is also set, passes marked
`isPrivate` additionally lose:

* `getDescription()` — emitted as the empty string.
* The argument key returned by `getArgument()` / `getArgumentName()` —
  emitted as the empty string.
* Per-option `cl::desc(...)` text — emitted as the empty string.
* Per-statistic description text — emitted as the empty string.
* The per-pass `register{PassName}()` and `register{PassName}Pass()`
  helpers, plus inclusion in the `register{Group}Passes()` aggregator.
* The per-pass C-API entry points
  `mlirCreate{Group}{PassName}` / `mlirRegister{Group}{PassName}`.

A private + stripped pass can still be constructed from C++ via the
generated `create{PassName}()` factory. It cannot be invoked via a textual
pass pipeline (`--pass-pipeline=...`), via the per-pass CLI flag, or via
the C API.

## Build configuration

```cmake
# Public/test/dev build: no obfuscation, no stripping (default).
cmake -G Ninja path/to/llvm \
  -DLLVM_ENABLE_PROJECTS=mlir

# Release build: obfuscate names of every TableGen item marked
# `isPrivate`, and strip CLI/CAPI exposure for private passes.
cmake -G Ninja path/to/llvm \
  -DLLVM_ENABLE_PROJECTS=mlir \
  -DMLIR_ENABLE_PRIVATE_NAME_OBFUSCATION=ON \
  -DMLIR_STRIP_PASS_METADATA=ON \
  -DMLIR_PRIVATE_NAME_SALT=<32-hex-char salt>
```

If `MLIR_PRIVATE_NAME_SALT` is left empty when obfuscation is enabled,
CMake generates a random 16-byte salt at first configure and stores it in
`${CMAKE_BINARY_DIR}/mlir-obfuscation.salt`. Subsequent reconfigurations
of the same build directory reuse that file. Wiping the build directory
generates a new salt.

The salt feeds directly into the `mlir-tblgen` command line as
`--mlir-obfuscation-salt=<hex>` and is **not** embedded in the resulting
binary. Bytecode produced by a build with one salt cannot be read by a
build with a different salt, so pin the salt across releases that need
binary compatibility.

## Marking items as private

```tablegen
def MyDialect : Dialect {
  let name = "mydialect";
  let cppNamespace = "::my";
  let isPrivate = 1;        // dialect, plus all of its ops/attrs/types
}

def MyOp : Op<MyDialect, "do_thing", []>;       // inherits isPrivate = 1
def MyOtherOp : Op<MyDialect, "do_thing2", []> {
  let isPrivate = 0;        // override: keep this op visible
}

def MyPass : Pass<"my-pass"> {
  let isPrivate = 1;        // hash the argument and name, drop description
                            // and CLI/CAPI registration when stripping
}
```

`AttrDef` and `TypeDef` inherit `isPrivate` from their owning dialect by
default and can be overridden the same way as ops.

## Caveats

* Hand-written code that compares `op->getName().getStringRef()` against a
  spelled-out op name (e.g. `== "mydialect.do_thing"`) breaks under
  obfuscation. Migrate such checks to `isa<my::DoThingOp>()` (which uses
  TypeID and is unaffected) or compare against
  `my::DoThingOp::getOperationName()` (which is itself obfuscated, so the
  comparison still works).
* Diagnostics and verifier messages naturally print the obfuscated names
  because they use runtime `getName()` / `getDialect()->getNamespace()`
  calls. The English skeleton text around the names is not stripped.
* Python bindings emitted by `gen-python-op-bindings` are not adjusted by
  this mechanism. Do not generate Python bindings for private dialects or
  ops.
* `LLVM_DEBUG`, `LDBG`, statistics (`LLVM_ENABLE_STATS`), and `--debug`
  paths are already removed from a release build (`NDEBUG`); they don't
  need separate handling.

## Audit checklist for downstream trees

Before enabling private-name obfuscation in a downstream compiler, audit for
hand-written string comparisons and textual pipeline dependencies. These
patterns should generally be rewritten to use TypeID-based APIs, concrete op
classes, or the generated `::getOperationName()` accessors:

```sh
rg 'getName\\(\\)\\.getStringRef\\(\\).*==|==.*getName\\(\\)\\.getStringRef\\(\\)' path/to/downstream
rg 'OperationName\\("[^"]+"' path/to/downstream
rg 'RegisteredOperationName::lookup\\("[^"]+"' path/to/downstream
rg 'getOrLoadDialect\\("[^"]+"' path/to/downstream
rg 'PassInfo::lookup\\("[^"]+"' path/to/downstream
rg 'parsePassPipeline|--pass-pipeline|register.*Passes' path/to/downstream
```

Comparisons against generated names, such as
`my::DoThingOp::getOperationName()`, remain valid because the generated
method returns the obfuscated spelling in release builds.
