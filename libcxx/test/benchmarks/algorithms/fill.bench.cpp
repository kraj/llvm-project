//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

#include <algorithm>
#include <benchmark/benchmark.h>
#include <vector>

static void bm_fill_n_vector_bool(benchmark::State& state) {
  std::vector<bool> vec1(state.range());
  for (auto _ : state) {
    benchmark::DoNotOptimize(vec1);
    benchmark::DoNotOptimize(std::fill_n(vec1.begin(), vec1.size(), false));
  }
}
BENCHMARK(bm_fill_n_vector_bool)->DenseRange(1, 8)->Range(16, 1 << 20);

static void bm_ranges_fill_n_vector_bool(benchmark::State& state) {
  std::vector<bool> vec1(state.range());
  for (auto _ : state) {
    benchmark::DoNotOptimize(vec1);
    benchmark::DoNotOptimize(std::ranges::fill_n(vec1.begin(), vec1.size(), false));
  }
}
BENCHMARK(bm_ranges_fill_n_vector_bool)->DenseRange(1, 8)->Range(16, 1 << 20);

static void bm_fill_vector_bool(benchmark::State& state) {
  std::vector<bool> vec1(state.range());
  for (auto _ : state) {
    benchmark::DoNotOptimize(vec1);
    std::fill(vec1.begin(), vec1.end(), false);
  }
}
BENCHMARK(bm_fill_vector_bool)->DenseRange(1, 8)->Range(16, 1 << 20);

static void bm_ranges_fill_vector_bool(benchmark::State& state) {
  std::vector<bool> vec1(state.range());
  for (auto _ : state) {
    benchmark::DoNotOptimize(vec1);
    benchmark::DoNotOptimize(std::ranges::fill(vec1, false));
  }
}
BENCHMARK(bm_ranges_fill_vector_bool)->DenseRange(1, 8)->Range(16, 1 << 20);

BENCHMARK_MAIN();
