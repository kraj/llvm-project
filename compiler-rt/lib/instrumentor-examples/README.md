# Instrumentor Examples

This directory contains example runtime libraries that demonstrate how to use
the LLVM Instrumentor pass for various profiling and analysis tasks.

## Overview

The LLVM Instrumentor is a configurable instrumentation pass that allows you to
insert runtime calls at various program points (e.g., function entry/exit,
memory operations, floating-point operations). Each example in this directory
provides:

1. A runtime library that implements the instrumentation callbacks
2. An instrumentor configuration JSON file
3. Tests demonstrating usage

## Examples

### FLOP Counter (`flop-counter/`)

A floating-point operation counter that tracks and reports the number of FLOPs
executed by a program.

**Features:**
- Counts FLOPs by precision (float, double, extended)
- Tracks vector operations
- Categorizes operations by type (add, mul, div, FMA, etc.)
- TODO: Handles intrinsic calls (sqrt, sin, cos, etc.)

**Usage:**
```bash
# Compile your program with instrumentor
clang -O2 -mllvm -enable-instrumentor -mllvm -instrumentor-read-config-files=conf.json -lclang_rt.flop_counter -o your_program

# Run it
./your_program
# At program exit, FLOP statistics will be printed
```

### Precision Analysis (`precision-analysis/`)

Analyzes the minimum floating-point precision needed for each operation while
maintaining acceptable accuracy.

**Features:**
- Per-operation precision requirement analysis
- Multi-level precision checking:
  - **Double operations**: Checks Float first, then FP16 if Float works
  - **Float operations**: Checks FP16
- Tracks relative error with configurable threshold (default: 0.1%)
- Distinguishes input special values from lowering-induced overflow/underflow
- Reports which operations can use FP16, which need Float, and which need Double
- IEEE 754 half-precision (fp16) software emulation
- Provides detailed recommendations for precision optimization

**Usage:**
```bash
# Compile your program with instrumentor
clang -O2 -mllvm -enable-instrumentor -mllvm -instrumentor-read-config-files=precision_analysis_config.json -lclang_rt.precision_analysis -o your_program

# Run it
./your_program
# At program exit, precision analysis results will be printed
```

## Building

The instrumentor examples are built as part of the compiler-rt build:

```bash
cmake -S llvm -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DLLVM_ENABLE_PROJECTS="clang;compiler-rt"
ninja -C build
```

The runtime libraries will be installed in:
- Darwin: `lib/clang/<version>/lib/darwin/libclang_rt.<example>_osx.a`
- Linux: `lib/clang/<version>/lib/linux/libclang_rt.<example>-<arch>.a`

Configuration files will be installed in `share/llvm/instrumentor-configs/`.

## Adding New Examples

To add a new instrumentor example:

1. Create a new directory under `compiler-rt/lib/instrumentor-examples/`
2. Add your runtime implementation (`.cpp` and `.h` files)
3. Create an instrumentor configuration JSON file
4. Add a `CMakeLists.txt` (see `flop-counter/CMakeLists.txt` as a template)
5. Update `compiler-rt/lib/instrumentor-examples/CMakeLists.txt` to include your subdirectory
6. Add tests in `compiler-rt/test/instrumentor-examples/`

## Resources

- [Instrumentor Documentation](../../../llvm/docs/Instrumentor.rst)
- [Instrumentor Runtime Headers](../../../llvm/utils/instrumentor_runtime.h)
- [Configuration Wizard](../../../llvm/utils/instrumentor-config-wizard.py)
