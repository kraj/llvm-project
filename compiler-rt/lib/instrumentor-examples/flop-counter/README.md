# FLOP Counter

A runtime library for counting floating-point operations in programs using the LLVM Instrumentor pass.

## Features

- **Precision Tracking**: Separates counts for single (float), double, and extended precision operations
- **Operation Categorization**: Tracks adds, multiplications, divisions, FMA operations, and others (sqrt, sin, cos, etc.)
- **Vector Support**: Counts FLOPs in vector operations by estimating element count
- **Thread-Safe**: Uses atomic operations for counter updates
- **Low Overhead**: Minimal runtime overhead for counting
- **Automatic Reporting**: Prints statistics at program exit

## Usage

### Basic Example

```c
#include <stdio.h>
#include <math.h>

double compute(double a, double b) {
  return sqrt(a * a + b * b);
}

int main() {
  double result = compute(3.0, 4.0);
  printf("Result: %f\n", result);
  return 0;
}
```

Compile with:
```bash
clang -O2 -finstrumentor=flop_counter_config.json example.c \
      -lclang_rt.flop_counter -o example
```

Run:
```bash
./example
```

Output:
```
Result: 5.000000

=================================================
           FLOP Counter Statistics
=================================================
Total FLOPs:                             3
...
```

## Implementation Details

### Instrumentation Points

The FLOP counter instruments:

1. **Binary FP Operations**: `fadd`, `fsub`, `fmul`, `fdiv`, `frem`
2. **Unary FP Operations**: `fneg`
3. **FP Intrinsics**: `llvm.fma`, `llvm.sqrt`, `llvm.sin`, `llvm.cos`, etc.

### FLOP Counting Rules

- **Regular operations**: 1 FLOP per operation
- **FMA (Fused Multiply-Add)**: 2 FLOPs (multiply + add)
- **Vector operations**: Counted per element (estimated from type size)
- **Intrinsics**: 1 FLOP each (sqrt, sin, cos, etc.)

### Configuration

The `flop_counter_config.json` file configures the instrumentor to:
- Insert callbacks after floating-point binary/unary operations
- Insert callbacks after intrinsic function calls
- Pass result values, sizes, type IDs, and opcodes to the runtime
- Filter to only instrument FP math operations

## Runtime API

The runtime provides these functions (though you typically don't call them directly):

```c
void __flop_counter_init(void);
void __flop_counter_finalize(void);
void __flop_counter_get_stats(struct FlopCounterStats *stats);
void __flop_counter_reset(void);
```

### Environment Variables

- `FLOP_COUNTER_VERBOSE=1`: Enable verbose logging during initialization

## Limitations

- **Vector element count estimation**: Uses type size heuristics; may not be exact for all vector types
- **No per-function breakdown**: Only provides aggregate counts
- **Optimization effects**: Compiler optimizations may eliminate or combine operations

## Future Enhancements

Potential improvements:
- Per-function FLOP counts using location information
- Configurable output format (JSON, CSV, etc.)
- Integration with performance counters for validation
- Support for custom FLOP costs (e.g., division = 4 FLOPs)
