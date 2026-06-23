//===-- precision_analysis_runtime.cpp - Precision Analysis Runtime ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements runtime for analyzing whether floating-point operations
// could be performed with lower precision while maintaining acceptable
// accuracy. It instruments FP operations, simulates them with lower precision,
// and compares results to determine if precision reduction is viable.
//
//===----------------------------------------------------------------------===//

#include "../instrumentor_runtime.h"

#include <atomic>
#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <mutex>

// Configuration: relative error threshold for "acceptable" lower precision
// A result is considered acceptable if |result_lower - result_higher| /
// |result_higher| < threshold
static constexpr double DEFAULT_RELATIVE_ERROR_THRESHOLD = 1e-3; // 0.1%

// Per-operation statistics - tracks separately by original precision
struct OperationStats {
  uint64_t total_count; // Total number of times this operation executed

  // Double-precision operations (started as double)
  uint64_t double_to_fp16;      // Double ops that can use fp16
  uint64_t double_to_float;     // Double ops that can use float (but not fp16)
  uint64_t double_needs_double; // Double ops that need double precision

  // Float-precision operations (started as float)
  uint64_t float_to_fp16;     // Float ops that can use fp16
  uint64_t float_needs_float; // Float ops that need float precision

  // Special values
  uint64_t
      input_special_values; // Times when inputs had special values (NaN, Inf)
  uint64_t double_lowering_special; // Double ops where lowering caused overflow
  uint64_t float_lowering_special;  // Float ops where lowering caused overflow
};

// Helper functions to get statistics map and mutex
// Using function-local statics ensures proper initialization order
// and avoids static destruction order fiasco.
//
// IMPORTANT: We use heap allocation (new) without delete to intentionally
// "leak" these objects. This ensures they remain valid when the destructor
// function runs at program exit, even if it runs after static destructors.
// For a profiling tool that runs once and exits, this is acceptable.
static std::map<int32_t, OperationStats> &get_operation_stats() {
  static std::map<int32_t, OperationStats> *stats =
      new std::map<int32_t, OperationStats>();
  return *stats;
}

static std::mutex &get_stats_mutex() {
  static std::mutex *mutex = new std::mutex();
  return *mutex;
}

enum {
  LLVM_OPCODE_FAdd = 15,
  LLVM_OPCODE_FSub = 17,
  LLVM_OPCODE_FMul = 19,
  LLVM_OPCODE_FDiv = 22,
  LLVM_OPCODE_FRem = 25,
  LLVM_OPCODE_FNeg = 13,
};

// Helper: Convert float to fp16 (IEEE 754 half precision) and back
// fp16 format: 1 sign bit, 5 exponent bits, 10 mantissa bits
static inline float simulate_fp16_precision(float value) {
  // Handle special cases
  if (std::isnan(value) || std::isinf(value)) {
    return value;
  }

  uint32_t bits;
  std::memcpy(&bits, &value, sizeof(float));

  uint32_t sign = bits & 0x80000000u;
  int32_t exponent = ((bits >> 23) & 0xFF) - 127;
  uint32_t mantissa = bits & 0x7FFFFFu;

  // fp16 range: exponent -14 to +15 (biased 1 to 30)
  // Underflow to zero
  if (exponent < -14) {
    return sign ? -0.0f : 0.0f;
  }

  // Overflow to infinity
  if (exponent > 15) {
    return sign ? -INFINITY : INFINITY;
  }

  // Round mantissa from 23 bits to 10 bits
  uint32_t fp16_mantissa = (mantissa + 0x1000u) >> 13;
  if (fp16_mantissa > 0x3FF) {
    // Rounding caused overflow
    fp16_mantissa = 0;
    exponent++;
    if (exponent > 15) {
      return sign ? -INFINITY : INFINITY;
    }
  }

  // Reconstruct float with reduced precision
  uint32_t fp16_exponent = (exponent + 127) & 0xFF;
  uint32_t result_bits = sign | (fp16_exponent << 23) | (fp16_mantissa << 13);

  float result;
  std::memcpy(&result, &result_bits, sizeof(float));
  return result;
}

// Helper: Check if value is special (NaN or Inf)
static inline bool is_special_value(double value) {
  return std::isnan(value) || std::isinf(value);
}

static inline bool is_special_value(float value) {
  return std::isnan(value) || std::isinf(value);
}

// Helper: Compute relative error
static inline double compute_relative_error(double reference, double test) {
  if (reference == 0.0) {
    return (test == 0.0) ? 0.0 : INFINITY;
  }
  return std::fabs((test - reference) / reference);
}

// Helper: Perform operation with lower precision (double → float)
static double simulate_lower_precision_op(int32_t opcode, double left,
                                          double right) {
  float left_f = static_cast<float>(left);
  float right_f = static_cast<float>(right);
  float result_f = 0.0f;

  switch (opcode) {
  case LLVM_OPCODE_FAdd:
    result_f = left_f + right_f;
    break;
  case LLVM_OPCODE_FSub:
    result_f = left_f - right_f;
    break;
  case LLVM_OPCODE_FMul:
    result_f = left_f * right_f;
    break;
  case LLVM_OPCODE_FDiv:
    result_f = left_f / right_f;
    break;
  case LLVM_OPCODE_FRem:
    result_f = std::fmod(left_f, right_f);
    break;
  case LLVM_OPCODE_FNeg:
    result_f = -left_f;
    break;
  default:
    // For unknown operations, assume lower precision is not ok
    return NAN;
  }

  return static_cast<double>(result_f);
}

// Helper: Perform operation with fp16 precision (float → fp16)
static float simulate_fp16_op(int32_t opcode, float left, float right) {
  float left_fp16 = simulate_fp16_precision(left);
  float right_fp16 = simulate_fp16_precision(right);
  float result_fp16 = 0.0f;

  switch (opcode) {
  case LLVM_OPCODE_FAdd:
    result_fp16 = left_fp16 + right_fp16;
    break;
  case LLVM_OPCODE_FSub:
    result_fp16 = left_fp16 - right_fp16;
    break;
  case LLVM_OPCODE_FMul:
    result_fp16 = left_fp16 * right_fp16;
    break;
  case LLVM_OPCODE_FDiv:
    result_fp16 = left_fp16 / right_fp16;
    break;
  case LLVM_OPCODE_FRem:
    result_fp16 = std::fmod(left_fp16, right_fp16);
    break;
  case LLVM_OPCODE_FNeg:
    result_fp16 = -left_fp16;
    break;
  default:
    return NAN;
  }

  // Apply fp16 precision to result as well
  return simulate_fp16_precision(result_fp16);
}

// Analyze a double-precision operation
// Check if float precision would suffice, and if so, also check if fp16 would
// work
static void analyze_double_operation(int32_t opcode, double left, double right,
                                     double result, int32_t id) {
  std::lock_guard<std::mutex> lock(get_stats_mutex());

  OperationStats &stats = get_operation_stats()[id];
  stats.total_count++;

  // Check for special values in inputs or result
  if (is_special_value(result) || is_special_value(left) ||
      is_special_value(right)) {
    stats.input_special_values++;
    return;
  }

  // First, try double → float
  double float_result = simulate_lower_precision_op(opcode, left, right);

  // Check if lowering to float created special values (overflow/underflow)
  if (is_special_value(float_result)) {
    stats.double_needs_double++;     // Float doesn't work, need to keep double
    stats.double_lowering_special++; // Record that overflow occurred
    return;
  }

  // Compare double vs float results
  double float_error = compute_relative_error(result, float_result);

  if (float_error >= DEFAULT_RELATIVE_ERROR_THRESHOLD) {
    // Float precision is not sufficient, need double
    stats.double_needs_double++;
    return;
  }

  // Float precision is acceptable. Now check if fp16 would also work.
  // Convert operands to float, then simulate fp16 operation
  float left_f = static_cast<float>(left);
  float right_f = static_cast<float>(right);
  float result_f = static_cast<float>(result);

  float fp16_result = simulate_fp16_op(opcode, left_f, right_f);

  // Check if lowering to fp16 created special values
  if (is_special_value(fp16_result)) {
    // fp16 causes overflow/underflow, but float works (double → float)
    stats.double_to_float++;         // Float is the lowest we can go
    stats.double_lowering_special++; // Record that fp16 overflow occurred
    return;
  }

  // Compare float vs fp16 results
  double fp16_error = compute_relative_error(static_cast<double>(result_f),
                                             static_cast<double>(fp16_result));

  if (fp16_error < DEFAULT_RELATIVE_ERROR_THRESHOLD) {
    // fp16 precision is sufficient (double → fp16)
    stats.double_to_fp16++;
  } else {
    // Need float precision but not double (double → float)
    stats.double_to_float++;
  }
}

// Analyze a float-precision operation (check if half precision would work)
static void analyze_float_operation(int32_t opcode, float left, float right,
                                    float result, int32_t id) {
  std::lock_guard<std::mutex> lock(get_stats_mutex());

  OperationStats &stats = get_operation_stats()[id];
  stats.total_count++;

  // Check for special values in inputs or result
  if (is_special_value(result) || is_special_value(left) ||
      is_special_value(right)) {
    stats.input_special_values++;
    return;
  }

  // Simulate operation with fp16 precision
  float lower_precision_result = simulate_fp16_op(opcode, left, right);

  // Check if lowering precision created special values (overflow/underflow to
  // inf)
  if (is_special_value(lower_precision_result)) {
    stats.float_needs_float++;      // FP16 doesn't work, need to keep float
    stats.float_lowering_special++; // Record that overflow occurred
    return;
  }

  // Compare results
  double relative_error = compute_relative_error(
      static_cast<double>(result), static_cast<double>(lower_precision_result));

  if (relative_error < DEFAULT_RELATIVE_ERROR_THRESHOLD) {
    // fp16 precision is sufficient (float → fp16)
    stats.float_to_fp16++;
  } else {
    // Need to keep float precision (float → float)
    stats.float_needs_float++;
  }
}

extern "C" {

__attribute__((destructor(1000))) void
__precision_analysis_post_module(char *module_name, char *target_triple,
                                 int32_t id) {
  std::printf("\n");
  std::printf("================================================================"
              "==========\n");
  std::printf("            Floating-Point Precision Analysis Results\n");
  std::printf("================================================================"
              "==========\n");
  std::printf(
      "This analysis checks minimum precision needed (error < %.2f%%):\n",
      DEFAULT_RELATIVE_ERROR_THRESHOLD * 100);
  std::printf("  - Double operations: Try Float, then FP16 if Float works\n");
  std::printf("  - Float operations: Try FP16\n");
  std::printf("================================================================"
              "==========\n\n");

  std::map<int32_t, OperationStats> &operation_stats = get_operation_stats();

  if (operation_stats.empty()) {
    std::printf("No operations analyzed.\n");
    std::printf("=============================================================="
                "============\n");
    return;
  }

  uint64_t total_ops = 0;
  uint64_t total_double_to_fp16 = 0;
  uint64_t total_double_to_float = 0;
  uint64_t total_double_needs_double = 0;
  uint64_t total_float_to_fp16 = 0;
  uint64_t total_float_needs_float = 0;
  uint64_t total_input_special = 0;
  uint64_t total_double_lowering_special = 0;
  uint64_t total_float_lowering_special = 0;

  std::printf("Per-Operation Results:\n");
  std::printf("%-5s %8s %9s %8s %6s %9s %6s %8s %7s %7s\n", "Op ID", "Total",
              "D->FP16", "D->F32", "D->D", "F->FP16", "F->F", "InpNaN",
              "D-OvFl", "F-OvFl");
  std::printf(
      "-------------------------------------------------------------------"
      "-------------\n");

  for (const auto &entry : operation_stats) {
    int32_t op_id = entry.first;
    const OperationStats &stats = entry.second;

    total_ops += stats.total_count;
    total_double_to_fp16 += stats.double_to_fp16;
    total_double_to_float += stats.double_to_float;
    total_double_needs_double += stats.double_needs_double;
    total_float_to_fp16 += stats.float_to_fp16;
    total_float_needs_float += stats.float_needs_float;
    total_input_special += stats.input_special_values;
    total_double_lowering_special += stats.double_lowering_special;
    total_float_lowering_special += stats.float_lowering_special;

    std::printf("%-5d %8llu %9llu %8llu %6llu %9llu %6llu %8llu %7llu %7llu\n",
                op_id, stats.total_count, stats.double_to_fp16,
                stats.double_to_float, stats.double_needs_double,
                stats.float_to_fp16, stats.float_needs_float,
                stats.input_special_values, stats.double_lowering_special,
                stats.float_lowering_special);
  }

  std::printf(
      "-------------------------------------------------------------------"
      "-------------\n");
  std::printf("%-5s %8llu %9llu %8llu %6llu %9llu %6llu %8llu %7llu %7llu\n",
              "TOTAL", total_ops, total_double_to_fp16, total_double_to_float,
              total_double_needs_double, total_float_to_fp16,
              total_float_needs_float, total_input_special,
              total_double_lowering_special, total_float_lowering_special);

  std::printf("\n");
  std::printf("Column Legend:\n");
  std::printf("  D->FP16:  Double ops that can use FP16 (16-bit)\n");
  std::printf(
      "  D->F32:   Double ops that can use Float (32-bit) but not FP16\n");
  std::printf("  D->D:     Double ops that require Double (64-bit)\n");
  std::printf("  F->FP16:  Float ops that can use FP16 (16-bit)\n");
  std::printf("  F->F:     Float ops that must stay Float (32-bit)\n");
  std::printf("  InpNaN:   Operations with NaN/Inf in inputs or result\n");
  std::printf("  D-OvFl:   Double ops where lowering caused overflow\n");
  std::printf("  F-OvFl:   Float ops where lowering to FP16 caused overflow\n");

  uint64_t total_double_ops =
      total_double_to_fp16 + total_double_to_float + total_double_needs_double;
  uint64_t total_float_ops = total_float_to_fp16 + total_float_needs_float;
  uint64_t analyzed_total = total_double_ops + total_float_ops;

  std::printf("\n");
  std::printf("================================================================"
              "==========\n");
  std::printf("Summary by Original Precision:\n");
  std::printf("================================================================"
              "==========\n");

  if (total_double_ops > 0) {
    std::printf("\nDOUBLE Operations (started as 64-bit double):\n");
    std::printf("  Total:                              %llu\n",
                total_double_ops);
    std::printf("  Can reduce to FP16 (16-bit):        %llu (%.1f%%)\n",
                total_double_to_fp16,
                100.0 * total_double_to_fp16 / total_double_ops);
    std::printf("  Can reduce to Float (32-bit):       %llu (%.1f%%)\n",
                total_double_to_float,
                100.0 * total_double_to_float / total_double_ops);
    std::printf("  Must keep Double (64-bit):          %llu (%.1f%%)\n",
                total_double_needs_double,
                100.0 * total_double_needs_double / total_double_ops);

    uint64_t double_convertible = total_double_to_fp16 + total_double_to_float;
    std::printf("  → Total convertible to lower:       %llu (%.1f%%)\n",
                double_convertible,
                100.0 * double_convertible / total_double_ops);
  }

  if (total_float_ops > 0) {
    std::printf("\nFLOAT Operations (started as 32-bit float):\n");
    std::printf("  Total:                              %llu\n",
                total_float_ops);
    std::printf("  Can reduce to FP16 (16-bit):        %llu (%.1f%%)\n",
                total_float_to_fp16,
                100.0 * total_float_to_fp16 / total_float_ops);
    std::printf("  Must keep Float (32-bit):           %llu (%.1f%%)\n",
                total_float_needs_float,
                100.0 * total_float_needs_float / total_float_ops);
  }

  std::printf("\nOVERALL Statistics:\n");
  std::printf("  Total analyzed operations:          %llu\n", analyzed_total);
  std::printf("  Operations with input NaN/Inf:      %llu\n",
              total_input_special);
  std::printf("  Double ops causing overflow:        %llu\n",
              total_double_lowering_special);
  std::printf("  Float ops causing overflow:         %llu\n",
              total_float_lowering_special);

  if (analyzed_total > 0) {
    uint64_t total_to_fp16 = total_double_to_fp16 + total_float_to_fp16;
    std::printf("\n  ALL operations reducible to FP16:   %llu (%.1f%%)\n",
                total_to_fp16, 100.0 * total_to_fp16 / analyzed_total);
  }

  // Provide recommendations based on results
  std::printf("\n=============================================================="
              "============\n");
  std::printf("Recommendations:\n");
  std::printf("================================================================"
              "==========\n");

  if (total_double_ops > 0) {
    // Include overflow operations in total for realistic assessment
    uint64_t total_double_with_overflow =
        total_double_ops + total_double_lowering_special;
    double double_to_lower = 100.0 *
                             (total_double_to_fp16 + total_double_to_float) /
                             total_double_with_overflow;
    double overflow_pct =
        100.0 * total_double_lowering_special / total_double_with_overflow;

    std::printf("\nFor DOUBLE operations:\n");
    std::printf("  Analyzed: %llu (%.1f%% overflow, not convertible)\n",
                total_double_with_overflow, overflow_pct);

    if (double_to_lower > 80.0) {
      std::printf(
          "  ✓ %.1f%% can use lower precision - strong conversion candidate\n",
          double_to_lower);
      if (total_double_to_fp16 > total_double_to_float) {
        std::printf("  ✓ Many can go directly to FP16 - consider aggressive "
                    "downcasting\n");
      } else {
        std::printf(
            "  ✓ Most need Float - consider using f32 instead of f64\n");
      }
      if (total_double_lowering_special > 0 && overflow_pct > 5.0) {
        std::printf(
            "  ⚠ %.1f%% overflow - may need value scaling/normalization\n",
            overflow_pct);
      }
    } else if (double_to_lower > 50.0) {
      std::printf(
          "  ~ %.1f%% can use lower precision - mixed precision recommended\n",
          double_to_lower);
      if (total_double_lowering_special > 0) {
        std::printf("  ⚠ %.1f%% overflow - limits conversion opportunities\n",
                    overflow_pct);
      }
    } else {
      std::printf("  ✗ Only %.1f%% can use lower precision - keep double\n",
                  double_to_lower);
      if (total_double_lowering_special > total_double_needs_double) {
        std::printf("  ! Most failures due to overflow (%.1f%%) rather than "
                    "accuracy (%llu ops)\n",
                    overflow_pct, total_double_needs_double);
        std::printf("  → Problem is value range, not precision\n");
      }
    }
  }

  if (total_float_ops > 0) {
    // Include overflow operations in total for realistic assessment
    uint64_t total_float_with_overflow =
        total_float_ops + total_float_lowering_special;
    double float_to_fp16_pct =
        100.0 * total_float_to_fp16 / total_float_with_overflow;
    double float_overflow_pct =
        100.0 * total_float_lowering_special / total_float_with_overflow;

    std::printf("\nFor FLOAT operations:\n");
    std::printf("  Analyzed: %llu (%.1f%% overflow to FP16)\n",
                total_float_with_overflow, float_overflow_pct);

    if (float_to_fp16_pct > 80.0) {
      std::printf(
          "  ✓ %.1f%% can use FP16 - strong FP16 conversion candidate\n",
          float_to_fp16_pct);
      if (total_float_lowering_special > 0 && float_overflow_pct > 5.0) {
        std::printf("  ⚠ %.1f%% overflow (values exceed FP16 range ±65504)\n",
                    float_overflow_pct);
      }
    } else if (float_to_fp16_pct > 50.0) {
      std::printf("  ~ %.1f%% can use FP16 - selective FP16 use recommended\n",
                  float_to_fp16_pct);
      if (total_float_lowering_special > 0) {
        std::printf("  ⚠ %.1f%% overflow - limits FP16 opportunities\n",
                    float_overflow_pct);
      }
    } else {
      std::printf("  ✗ Only %.1f%% can use FP16 - keep float\n",
                  float_to_fp16_pct);
      if (total_float_lowering_special > total_float_needs_float) {
        std::printf("  ! Most failures due to FP16 overflow (%.1f%%) rather "
                    "than accuracy (%llu ops)\n",
                    float_overflow_pct, total_float_needs_float);
        std::printf("  → Problem: Values exceed FP16 range (±65504)\n");
        std::printf("  → Solution: Scale values or use Float\n");
      }
    }
  }

  std::printf("================================================================"
              "==========\n");
}

void __precision_analysis_post_numeric(int32_t type_id, int32_t sub_type_id,
                                       int32_t size, int32_t opcode,
                                       int64_t left, int64_t right,
                                       int64_t result, int64_t flags,
                                       int32_t id) {
  // Handle vector types by looking at sub_type_id
  bool is_vector = false;
  int32_t element_type_id = type_id;

  switch (type_id) {
  case FixedVectorTyID:
  case ScalableVectorTyID:
    is_vector = true;
    element_type_id = sub_type_id;
    break;
  default:
    break;
  }

  // For vector operations, we'd need to extract each element
  // For now, skip vector operations (they're more complex)
  if (is_vector) {
    return;
  }

  // Analyze based on type
  if (element_type_id == DoubleTyID) {
    // Double precision operation - check if float would suffice
    double left_val = *reinterpret_cast<double *>(&left);
    double right_val = *reinterpret_cast<double *>(&right);
    double result_val = *reinterpret_cast<double *>(&result);

    analyze_double_operation(opcode, left_val, right_val, result_val, id);
  } else if (element_type_id == FloatTyID) {
    // Float precision operation - could check if half would suffice
    float left_val = *reinterpret_cast<float *>(&left);
    float right_val = *reinterpret_cast<float *>(&right);
    float result_val = *reinterpret_cast<float *>(&result);

    analyze_float_operation(opcode, left_val, right_val, result_val, id);
  }
  // Skip other types (half, bfloat, extended precision)
}

void __precision_analysis_post_numeric_ind(int32_t type_id, int32_t sub_type_id,
                                           int32_t size, int32_t opcode,
                                           int64_t *left_ptr,
                                           int64_t *right_ptr,
                                           int64_t *result_ptr, int64_t flags,
                                           int32_t id) {}

} // extern "C"
