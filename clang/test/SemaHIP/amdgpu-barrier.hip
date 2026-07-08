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

// expected-note@+2 {{'TestSimple' is a named barrier wrapper around 'x', which implies 'final'}}
// expected-note@+1 {{'TestSimple' declared here}}
struct TestSimple {
  __amdgpu_named_workgroup_barrier_t x; // expected-note {{'x' declared here}}
};

struct TestArray{
  __amdgpu_named_workgroup_barrier_t y[2];
};

struct TestSugared {
  SugaredArray z[2];
};

struct TestSimpleWithStaticField {
  __amdgpu_named_workgroup_barrier_t x;
  static unsigned Harmless;
};

// Wrappers cannot have >1 field.
struct WrapperHasTooManyFields { // expected-note {{WrapperHasTooManyFields is not a named barrier wrapper because it has more than one field}}
  __amdgpu_named_workgroup_barrier_t x; // expected-error {{fields of type '__amdgpu_named_workgroup_barrier_t' are only allowed in named barrier wrappers}}
  int other;
};

// Wrappers cannot have base class
struct WrapperBase {
};

struct WrapperWithBase : public WrapperBase { // expected-note {{WrapperWithBase is not a named barrier wrapper because it inherits from 'WrapperBase'; named barrier wrappers may not have any bases}}
  __amdgpu_named_workgroup_barrier_t x; // expected-error {{fields of type '__amdgpu_named_workgroup_barrier_t' are only allowed in named barrier wrappers}}
};

// Wrappers imply final
struct NoWrapperInheritance : public TestSimple { }; // expected-error {{base 'TestSimple' is marked 'final'}}

// Wrappers of Wrappers have the same restrictions.
struct WrapperOfWrapperWithTooManyFields final { // expected-note {{WrapperOfWrapperWithTooManyFields is not a named barrier wrapper because it has more than one field}}
  TestSimple x; // expected-error {{fields of type 'TestSimple' are only allowed in named barrier wrappers}}
  int other;
};

static_assert(sizeof(__amdgpu_named_workgroup_barrier_t) == 16, "wrong size");
static_assert(alignof(__amdgpu_named_workgroup_barrier_t) == 4, "wrong alignment");
