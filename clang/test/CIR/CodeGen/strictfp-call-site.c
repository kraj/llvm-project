// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -fclangir -emit-cir %s -o %t.cir
// RUN: FileCheck --input-file=%t.cir %s -check-prefix=CIR
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -fclangir -emit-llvm %s -o %t-cir.ll
// RUN: FileCheck --input-file=%t-cir.ll %s -check-prefixes=LLVM,LLVMCIR
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -emit-llvm %s -o %t.ll
// RUN: FileCheck --input-file=%t.ll %s -check-prefixes=LLVM,OGCG

// LLVM requires every call in a function that uses constrained FP intrinsics
// to be strictfp, at the call site or on the callee. CIR keeps strictfp on the
// function only. The lowering to the LLVM dialect marks the calls in it,
// including the libcalls that LoweringPrepare creates.

void ext(void);
float extf(float);
struct S { int a[32]; };

void plain_direct(void) { ext(); }

// CIR: cir.func {{.*}}@plain_direct()
// CIR:   cir.call @ext() : () -> ()

// LLVM: define {{.*}}void @plain_direct() #[[PLAIN_FN:[0-9]+]]
// LLVM:   call void @ext(){{$}}

void plain_indirect(void (*fp)(void)) { fp(); }

// LLVM: define {{.*}}void @plain_indirect(ptr {{.*}}) #[[PLAIN_FN]]
// LLVM:   call void %{{.*}}(){{$}}

void plain_copy(struct S *d, struct S *s) { *d = *s; }

// LLVM: define {{.*}}void @plain_copy(ptr {{.*}}, ptr {{.*}}) #[[PLAIN_FN]]
// LLVM:   call void @llvm.memcpy.p0.p0.i64({{.*}}, i64 128, i1 false){{$}}

// Classic codegen marks inline assembly nounwind.
void plain_asm(void) { __asm__ volatile("nop"); }

// LLVM: define {{.*}}void @plain_asm() #[[PLAIN_FN]]
// LLVMCIR:   call void asm sideeffect "nop", "~{dirflag},~{fpsr},~{flags}"(){{$}}
// OGCG:   call void asm sideeffect "nop", "~{dirflag},~{fpsr},~{flags}"() #[[NOUNWIND_CALL:[0-9]+]]

#pragma STDC FENV_ACCESS ON

void strict_direct(void) { ext(); }

// The attribute stays on the function in CIR.
// CIR: cir.func {{.*}}@strict_direct() attributes {{{.*}}strictfp}
// CIR:   cir.call @ext() : () -> ()

// LLVM: define {{.*}}void @strict_direct() #[[STRICT_FN:[0-9]+]]
// LLVM:   call void @ext() #[[STRICT_CALL:[0-9]+]]

void strict_indirect(void (*fp)(void)) { fp(); }

// CIR: cir.func {{.*}}@strict_indirect({{.*}}) attributes {{{.*}}strictfp}
// CIR:   cir.call %{{.*}}() : (!cir.ptr<!cir.func<()>>) -> ()

// LLVM: define {{.*}}void @strict_indirect(ptr {{.*}}) #[[STRICT_FN]]
// LLVM:   call void %{{.*}}() #[[STRICT_CALL]]

// A call next to a constrained operation.
float strict_mixed(float a) { return extf(a + 1.0f); }

// LLVM: define {{.*}}float @strict_mixed(float {{.*}}) #[[STRICT_FN]]
// LLVM:   call float @llvm.experimental.constrained.fadd.f32({{.*}}) #[[STRICT_CALL]]
// LLVM:   call float @extf(float {{.*}}) #[[STRICT_CALL]]

// An intrinsic that has no constrained variant. The LLVM dialect's named
// intrinsic ops have no call-site attributes, so ClangIR leaves it unmarked.
float strict_builtin(float a) { return __builtin_fabsf(a); }

// LLVM: define {{.*}}float @strict_builtin(float {{.*}}) #[[STRICT_FN]]
// LLVMCIR:   call float @llvm.fabs.f32(float %{{.*}}){{$}}
// OGCG:   call float @llvm.fabs.f32(float %{{.*}}) #[[STRICT_CALL]]

// An intrinsic that the lowering creates for an operation that is not a call.
// It is a named intrinsic op too.
void strict_copy(struct S *d, struct S *s) { *d = *s; }

// CIR: cir.func {{.*}}@strict_copy({{.*}}) attributes {{{.*}}strictfp}
// CIR:   cir.copy

// LLVM: define {{.*}}void @strict_copy(ptr {{.*}}, ptr {{.*}}) #[[STRICT_FN]]
// LLVMCIR:   call void @llvm.memcpy.p0.p0.i64({{.*}}, i64 128, i1 false){{$}}
// OGCG:   call void @llvm.memcpy.p0.p0.i64({{.*}}, i64 128, i1 false) #[[STRICT_CALL]]

// A libcall that LoweringPrepare creates. Classic codegen also marks it
// nounwind.
_Complex float strict_complex_mul(_Complex float a, _Complex float b) {
  return a * b;
}

// LLVM: define {{.*}}<2 x float> @strict_complex_mul(
// LLVMCIR:   call <2 x float> @__mulsc3({{.*}}) #[[STRICT_CALL]]
// OGCG:   call <2 x float> @__mulsc3({{.*}}) #[[STRICT_NOUNWIND_CALL:[0-9]+]]

// Inline assembly. Classic codegen also marks it nounwind.
void strict_asm(void) { __asm__ volatile("nop"); }

// CIR: cir.func {{.*}}@strict_asm() attributes {{{.*}}strictfp}
// CIR:   cir.asm(x86_att,

// LLVM: define {{.*}}void @strict_asm() #[[STRICT_FN]]
// LLVMCIR:   call void asm sideeffect "nop", "~{dirflag},~{fpsr},~{flags}"() #[[STRICT_CALL]]
// OGCG:   call void asm sideeffect "nop", "~{dirflag},~{fpsr},~{flags}"() #[[STRICT_NOUNWIND_CALL]]

#pragma STDC FENV_ACCESS OFF

// A call to a strictfp function from a function that is not strictfp.
void plain_calls_strict(void) { strict_direct(); }

// LLVM: define {{.*}}void @plain_calls_strict() #[[PLAIN_FN]]
// LLVM:   call void @strict_direct(){{$}}

// LLVM-DAG: attributes #[[STRICT_FN]] = { {{.*}}strictfp
// LLVM-DAG: attributes #[[STRICT_CALL]] = { strictfp }
// OGCG-DAG: attributes #[[NOUNWIND_CALL]] = { nounwind }
// OGCG-DAG: attributes #[[STRICT_NOUNWIND_CALL]] = { nounwind strictfp }
