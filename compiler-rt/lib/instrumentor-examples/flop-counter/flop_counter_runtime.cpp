//===-- flop_counter_runtime.cpp - FLOP Counter Runtime ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the runtime for counting floating-point operations.
// It hooks into instrumentation points inserted by the LLVM Instrumentor pass.
//
//===----------------------------------------------------------------------===//

#include "../instrumentor_runtime.h"

#include <atomic>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/// FLOP counter statistics
struct FlopCounterStats {
  uint64_t total_flops;
  uint64_t float_ops;    // 32-bit float operations
  uint64_t double_ops;   // 64-bit double operations
  uint64_t extended_ops; // 80/128-bit extended precision operations
  uint64_t vector_flops; // Total FLOPs from vector operations
  uint64_t add_ops;
  uint64_t mul_ops;
  uint64_t div_ops;
  uint64_t fma_ops;   // Fused multiply-add operations
  uint64_t other_ops; // sqrt, sin, cos, etc.
};

// Global statistics counters (thread-safe using atomics)
static std::atomic<uint64_t> g_total_flops{0};
static std::atomic<uint64_t> g_float_ops{0};
static std::atomic<uint64_t> g_double_ops{0};
static std::atomic<uint64_t> g_extended_ops{0};
static std::atomic<uint64_t> g_vector_flops{0};
static std::atomic<uint64_t> g_add_ops{0};
static std::atomic<uint64_t> g_mul_ops{0};
static std::atomic<uint64_t> g_div_ops{0};
static std::atomic<uint64_t> g_fma_ops{0};
static std::atomic<uint64_t> g_other_ops{0};

enum {
  LLVM_OPCODE_FAdd = 15,
  LLVM_OPCODE_FSub = 17,
  LLVM_OPCODE_FMul = 19,
  LLVM_OPCODE_FDiv = 22,
  LLVM_OPCODE_FRem = 25,
  LLVM_OPCODE_FNeg = 13,
};

static void __flop_counter_get_stats(struct FlopCounterStats *stats) {
  stats->total_flops = g_total_flops.load(std::memory_order_relaxed);
  stats->float_ops = g_float_ops.load(std::memory_order_relaxed);
  stats->double_ops = g_double_ops.load(std::memory_order_relaxed);
  stats->extended_ops = g_extended_ops.load(std::memory_order_relaxed);
  stats->vector_flops = g_vector_flops.load(std::memory_order_relaxed);
  stats->add_ops = g_add_ops.load(std::memory_order_relaxed);
  stats->mul_ops = g_mul_ops.load(std::memory_order_relaxed);
  stats->div_ops = g_div_ops.load(std::memory_order_relaxed);
  stats->fma_ops = g_fma_ops.load(std::memory_order_relaxed);
  stats->other_ops = g_other_ops.load(std::memory_order_relaxed);
}

extern "C" {

__attribute__((destructor(1000))) void
__flop_counter_post_module(char *module_name, char *target_triple, int32_t id) {
  struct FlopCounterStats stats;
  __flop_counter_get_stats(&stats);

  std::printf("\n");
  std::printf("=================================================\n");
  std::printf("           FLOP Counter Statistics\n");
  std::printf("=================================================\n");
  std::printf("Total FLOPs:              %20llu\n", stats.total_flops);
  std::printf("\n");
  std::printf("By Precision:\n");
  std::printf("  Single (float):         %20llu\n", stats.float_ops);
  std::printf("  Double (double):        %20llu\n", stats.double_ops);
  std::printf("  Extended (fp80/fp128):  %20llu\n", stats.extended_ops);
  std::printf("  Vector FLOPs:           %20llu\n", stats.vector_flops);
  std::printf("\n");
  std::printf("By Operation:\n");
  std::printf("  Addition/Subtraction:   %20llu\n", stats.add_ops);
  std::printf("  Multiplication:         %20llu\n", stats.mul_ops);
  std::printf("  Division:               %20llu\n", stats.div_ops);
  std::printf("  Fused Multiply-Add:     %20llu\n", stats.fma_ops);
  std::printf("  Other (sqrt, sin, ...): %20llu\n", stats.other_ops);
  std::printf("=================================================\n");
}

void __flop_counter_post_numeric(int32_t type_id, int32_t sub_type_id,
                                 int32_t size, int32_t opcode, int64_t left,
                                 int64_t right, int64_t result, int64_t flags,
                                 int32_t id) {
  bool IsVector = false;
  switch (type_id) {
  case FixedVectorTyID:
  case ScalableVectorTyID:
    IsVector = true;
    type_id = sub_type_id;
    break;
  default:
    break;
  };

  int32_t TypeSize = size;
  switch (type_id) {
  case HalfTyID:
  case BFloatTyID:
    TypeSize = 2;
    break;
  case FloatTyID:
    TypeSize = 4;
    break;
  case DoubleTyID:
    TypeSize = 8;
    break;
  case X86_FP80TyID:
  case FP128TyID:
  case PPC_FP128TyID:
    TypeSize = 16;
    break;
  default:
    break;
  };

  // Determine FLOP count based on whether it's a vector operation
  uint64_t flop_count = size / TypeSize;
  if (IsVector) {
    g_vector_flops.fetch_add(flop_count, std::memory_order_relaxed);
  } else {
    // Categorize by precision
    if (type_id == 2) {
      g_float_ops.fetch_add(1, std::memory_order_relaxed);
    } else if (type_id == 3) {
      g_double_ops.fetch_add(1, std::memory_order_relaxed);
    } else {
      g_extended_ops.fetch_add(1, std::memory_order_relaxed);
    }
  }

  // Categorize by operation type
  switch (opcode) {
  case LLVM_OPCODE_FAdd:
  case LLVM_OPCODE_FSub:
    g_add_ops.fetch_add(flop_count, std::memory_order_relaxed);
    break;
  case LLVM_OPCODE_FMul:
    g_mul_ops.fetch_add(flop_count, std::memory_order_relaxed);
    break;
  case LLVM_OPCODE_FDiv:
  case LLVM_OPCODE_FRem:
    g_div_ops.fetch_add(flop_count, std::memory_order_relaxed);
    break;
  default:
    g_other_ops.fetch_add(flop_count, std::memory_order_relaxed);
    break;
  }

  g_total_flops.fetch_add(flop_count, std::memory_order_relaxed);
}

void __flop_counter_post_numeric_ind(int32_t type_id, int32_t sub_type_id,
                                     int32_t size, int32_t opcode,
                                     int64_t *left_ptr, int64_t *right_ptr,
                                     int64_t *result_ptr, int64_t flags,
                                     int32_t id) {
  return __flop_counter_post_numeric(type_id, sub_type_id, size, opcode, 1, 1,
                                     0, flags, id);
}

} // extern "C"
