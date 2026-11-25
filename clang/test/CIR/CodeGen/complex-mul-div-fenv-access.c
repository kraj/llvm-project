// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -fclangir -emit-llvm %s -o %t-cir.ll
// RUN: FileCheck --input-file=%t-cir.ll %s --check-prefixes=FULL,LLVM-FULL \
// RUN:   --implicit-check-not="{{= f(add|sub|mul|div|cmp|pext|ptrunc) }}"
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -emit-llvm %s -o %t.ll
// RUN: FileCheck --input-file=%t.ll %s --check-prefixes=FULL,OGCG-FULL

// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -complex-range=improved -fclangir -emit-llvm %s -o %t-improved-cir.ll
// RUN: FileCheck --input-file=%t-improved-cir.ll %s --check-prefixes=IMPROVED,LLVM-IMPROVED \
// RUN:   --implicit-check-not="{{= f(add|sub|mul|div|cmp|pext|ptrunc) }}"
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -complex-range=improved -emit-llvm %s -o %t-improved.ll
// RUN: FileCheck --input-file=%t-improved.ll %s --check-prefixes=IMPROVED,OGCG-IMPROVED

// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -complex-range=promoted -fclangir -emit-llvm %s -o %t-promoted-cir.ll
// RUN: FileCheck --input-file=%t-promoted-cir.ll %s --check-prefix=LLVM-PROMOTED \
// RUN:   --implicit-check-not="{{= f(add|sub|mul|div|cmp|pext|ptrunc) }}"
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -complex-range=promoted -emit-llvm %s -o %t-promoted.ll
// RUN: FileCheck --input-file=%t-promoted.ll %s --check-prefix=OGCG-PROMOTED

// Complex multiplication and division under FENV_ACCESS. The arithmetic,
// comparisons and conversions of the expansion are constrained intrinsics.
// The FULL and IMPROVED lines are shared by ClangIR and classic codegen, so
// both emit the same intrinsics with the same metadata.

#pragma STDC FENV_ACCESS ON

_Complex float a, b, c;

void mul(void) { c = a * b; }

// FULL-LABEL: define{{.*}} void @mul()

// LLVM-FULL: %[[A:.*]] = load { float, float }, ptr @a
// LLVM-FULL: %[[B:.*]] = load { float, float }, ptr @b
// LLVM-FULL: %[[A_REAL:.*]] = extractvalue { float, float } %[[A]], 0
// LLVM-FULL: %[[A_IMAG:.*]] = extractvalue { float, float } %[[A]], 1
// LLVM-FULL: %[[B_REAL:.*]] = extractvalue { float, float } %[[B]], 0
// LLVM-FULL: %[[B_IMAG:.*]] = extractvalue { float, float } %[[B]], 1

// OGCG-FULL: %[[A_REAL:.*]] = load float, ptr @a
// OGCG-FULL: %[[A_IMAG:.*]] = load float, ptr getelementptr inbounds nuw (i8, ptr @a, i64 4)
// OGCG-FULL: %[[B_REAL:.*]] = load float, ptr @b
// OGCG-FULL: %[[B_IMAG:.*]] = load float, ptr getelementptr inbounds nuw (i8, ptr @b, i64 4)

// FULL: %[[AC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// FULL: %[[BD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// FULL: %[[AD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_IMAG]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// FULL: %[[BC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_REAL]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// FULL: %[[REAL:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[AC]], float %[[BD]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// FULL: %[[IMAG:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[AD]], float %[[BC]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// FULL: call i1 @llvm.experimental.constrained.fcmp.f32(float %[[REAL]], float %[[REAL]], metadata !"uno", metadata !"fpexcept.strict")
// FULL: call i1 @llvm.experimental.constrained.fcmp.f32(float %[[IMAG]], float %[[IMAG]], metadata !"uno", metadata !"fpexcept.strict")
// FULL: call {{.*}}<2 x float> @__mulsc3(float {{.*}}%[[A_REAL]], float {{.*}}%[[A_IMAG]], float {{.*}}%[[B_REAL]], float {{.*}}%[[B_IMAG]])

void div(void) { c = a / b; }

// With the full range, division is a call to the runtime library.

// FULL-LABEL: define{{.*}} void @div()
// FULL: call {{.*}}<2 x float> @__divsc3(

// With the improved range, division uses Smith's algorithm. The range check
// is a quiet comparison in both codegens. Its predicate differs.

// IMPROVED-LABEL: define{{.*}} void @div()

// LLVM-IMPROVED: %[[A:.*]] = load { float, float }, ptr @a
// LLVM-IMPROVED: %[[B:.*]] = load { float, float }, ptr @b
// LLVM-IMPROVED: %[[A_REAL:.*]] = extractvalue { float, float } %[[A]], 0
// LLVM-IMPROVED: %[[A_IMAG:.*]] = extractvalue { float, float } %[[A]], 1
// LLVM-IMPROVED: %[[B_REAL:.*]] = extractvalue { float, float } %[[B]], 0
// LLVM-IMPROVED: %[[B_IMAG:.*]] = extractvalue { float, float } %[[B]], 1

// OGCG-IMPROVED: %[[A_REAL:.*]] = load float, ptr @a
// OGCG-IMPROVED: %[[A_IMAG:.*]] = load float, ptr getelementptr inbounds nuw (i8, ptr @a, i64 4)
// OGCG-IMPROVED: %[[B_REAL:.*]] = load float, ptr @b
// OGCG-IMPROVED: %[[B_IMAG:.*]] = load float, ptr getelementptr inbounds nuw (i8, ptr @b, i64 4)

// IMPROVED: %[[ABS_C:.*]] = call float @llvm.fabs.f32(float %[[B_REAL]])
// IMPROVED: %[[ABS_D:.*]] = call float @llvm.fabs.f32(float %[[B_IMAG]])
// LLVM-IMPROVED: call i1 @llvm.experimental.constrained.fcmp.f32(float %[[ABS_C]], float %[[ABS_D]], metadata !"oge", metadata !"fpexcept.strict")
// OGCG-IMPROVED: call i1 @llvm.experimental.constrained.fcmp.f32(float %[[ABS_C]], float %[[ABS_D]], metadata !"ugt", metadata !"fpexcept.strict")

// IMPROVED: %[[R1:.*]] = call float @llvm.experimental.constrained.fdiv.f32(float %[[B_IMAG]], float %[[B_REAL]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[RD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[R1]], float %[[B_IMAG]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[TMP1:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[B_REAL]], float %[[RD]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[BR1:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[R1]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[ABR:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[A_REAL]], float %[[BR1]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: call float @llvm.experimental.constrained.fdiv.f32(float %[[ABR]], float %[[TMP1]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[AR1:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[R1]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[BAR:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[A_IMAG]], float %[[AR1]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: call float @llvm.experimental.constrained.fdiv.f32(float %[[BAR]], float %[[TMP1]], metadata !"round.dynamic", metadata !"fpexcept.strict")

// IMPROVED: %[[R2:.*]] = call float @llvm.experimental.constrained.fdiv.f32(float %[[B_REAL]], float %[[B_IMAG]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[RC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[R2]], float %[[B_REAL]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[TMP2:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[B_IMAG]], float %[[RC]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[AR2:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[R2]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[ARB:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[AR2]], float %[[A_IMAG]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: call float @llvm.experimental.constrained.fdiv.f32(float %[[ARB]], float %[[TMP2]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[BR2:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[R2]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: %[[BRA:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[BR2]], float %[[A_REAL]], metadata !"round.dynamic", metadata !"fpexcept.strict")
// IMPROVED: call float @llvm.experimental.constrained.fdiv.f32(float %[[BRA]], float %[[TMP2]], metadata !"round.dynamic", metadata !"fpexcept.strict")

// With the promoted range, division is done in a wider type. Classic codegen
// converts the operands and the result outside the scope of the division's
// FP options, so its conversions get the default environment of the
// function. ClangIR converts as part of the division and gives the
// conversions its environment, which assumes less.

// LLVM-PROMOTED-LABEL: define{{.*}} void @div()
// LLVM-PROMOTED-COUNT-4: call double @llvm.experimental.constrained.fpext.f64.f32(float %{{.*}}, metadata !"fpexcept.strict")
// LLVM-PROMOTED-COUNT-4: call double @llvm.experimental.constrained.fmul.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// LLVM-PROMOTED-COUNT-2: call double @llvm.experimental.constrained.fadd.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// LLVM-PROMOTED: call double @llvm.experimental.constrained.fdiv.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// LLVM-PROMOTED-COUNT-2: call double @llvm.experimental.constrained.fmul.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// LLVM-PROMOTED: call double @llvm.experimental.constrained.fsub.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// LLVM-PROMOTED: call double @llvm.experimental.constrained.fdiv.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// LLVM-PROMOTED-COUNT-2: call float @llvm.experimental.constrained.fptrunc.f32.f64(double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")

// OGCG-PROMOTED-LABEL: define{{.*}} void @div()
// OGCG-PROMOTED-COUNT-4: call double @llvm.experimental.constrained.fpext.f64.f32(float %{{.*}}, metadata !"fpexcept.ignore")
// OGCG-PROMOTED-COUNT-2: call double @llvm.experimental.constrained.fmul.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// OGCG-PROMOTED: call double @llvm.experimental.constrained.fadd.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// OGCG-PROMOTED-COUNT-2: call double @llvm.experimental.constrained.fmul.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// OGCG-PROMOTED: call double @llvm.experimental.constrained.fadd.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// OGCG-PROMOTED-COUNT-2: call double @llvm.experimental.constrained.fmul.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// OGCG-PROMOTED: call double @llvm.experimental.constrained.fsub.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// OGCG-PROMOTED-COUNT-2: call double @llvm.experimental.constrained.fdiv.f64(double %{{.*}}, double %{{.*}}, metadata !"round.dynamic", metadata !"fpexcept.strict")
// OGCG-PROMOTED-COUNT-2: call float @llvm.experimental.constrained.fptrunc.f32.f64(double %{{.*}}, metadata !"round.tonearest", metadata !"fpexcept.ignore")
