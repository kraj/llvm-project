// REQUIRES: amdgpu-registered-target
// RUN: %clang_cc1 -fsyntax-only -verify -triple amdgcn -Wno-unused-value %s
// RUN: %clang_cc1 -fsyntax-only -verify -triple x86_64 -aux-triple amdgcn -Wno-unused-value %s

#define __device__ __attribute__((device))

__device__ void foo() {
  int n = 100;
  __amdgpu_named_workgroup_barrier_t v = 0; // expected-error {{cannot initialize a variable of type '__amdgpu_named_workgroup_barrier_t' with an rvalue of type 'int'}}
  static_cast<__amdgpu_named_workgroup_barrier_t>(n); // expected-error {{static_cast from 'int' to '__amdgpu_named_workgroup_barrier_t' is not allowed}}
  dynamic_cast<__amdgpu_named_workgroup_barrier_t>(n); // expected-error {{invalid target type '__amdgpu_named_workgroup_barrier_t' for dynamic_cast; target type must be a reference or pointer type to a defined class}}
  reinterpret_cast<__amdgpu_named_workgroup_barrier_t>(n); // expected-error {{reinterpret_cast from 'int' to '__amdgpu_named_workgroup_barrier_t' is not allowed}}
  int c(v); // expected-error {{cannot initialize a variable of type 'int' with an lvalue of type '__amdgpu_named_workgroup_barrier_t'}}
  __amdgpu_named_workgroup_barrier_t k;
  int *ip = (int *)k; // expected-error {{cannot cast from type '__amdgpu_named_workgroup_barrier_t' to pointer type 'int *'}}
  void *vp = (void *)k; // expected-error {{cannot cast from type '__amdgpu_named_workgroup_barrier_t' to pointer type 'void *'}}
}

using SugaredArray = __amdgpu_named_workgroup_barrier_t[2];

struct TestSimple final {
  __amdgpu_named_workgroup_barrier_t x;
};

struct TestArray final {
  __amdgpu_named_workgroup_barrier_t y[2];
};

struct TestSugared final {
  SugaredArray z[2];
};

// Wrappers cannot have >1 field.
struct WrapperHasTooManyFields final { // expected-note {{WrapperHasTooManyFields is not a trivial wrapper because it has more than one field}}
  __amdgpu_named_workgroup_barrier_t x; // expected-error {{'__amdgpu_named_workgroup_barrier_t' is only allowed as a field if the struct is a trivial wrapper around it}}
  int other;
};

struct WrapperIsNotFinal { // expected-note {{WrapperIsNotFinal is not a trivial wrapper because it is not marked 'final'}}
  __amdgpu_named_workgroup_barrier_t x; // expected-error {{'__amdgpu_named_workgroup_barrier_t' is only allowed as a field if the struct is a trivial wrapper around it}}
};

// Wrappers cannot have base class
struct WrapperBase {
};

struct WrapperWithBase final : public WrapperBase { // expected-note {{WrapperWithBase is not a trivial wrapper because it inherits from 'WrapperBase'}}
  __amdgpu_named_workgroup_barrier_t x; // expected-error {{'__amdgpu_named_workgroup_barrier_t' is only allowed as a field if the struct is a trivial wrapper around it}}
};


static_assert(sizeof(__amdgpu_named_workgroup_barrier_t) == 16, "wrong size");
static_assert(alignof(__amdgpu_named_workgroup_barrier_t) == 4, "wrong alignment");
