// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -Wno-unused-value -fclangir -ffp-contract=on -fexperimental-strict-floating-point -ffp-exception-behavior=strict -emit-cir %s -o %t-strict.cir
// RUN: FileCheck --input-file=%t-strict.cir %s -check-prefix=CIR
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -Wno-unused-value -fclangir -ffp-contract=on -fexperimental-strict-floating-point -ffp-exception-behavior=strict -emit-llvm %s -o %t-strict.ll
// RUN: FileCheck --input-file=%t-strict.ll %s -check-prefix=LLVM
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -Wno-unused-value -ffp-contract=on -fexperimental-strict-floating-point -ffp-exception-behavior=strict -emit-llvm %s -o %t-strict-ogcg.ll
// RUN: FileCheck --input-file=%t-strict-ogcg.ll %s -check-prefix=OGCG
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -Wno-unused-value -fclangir -ffp-contract=on -fexperimental-strict-floating-point -ffp-exception-behavior=strict -emit-cir -mmlir --mlir-print-ir-before=cir-canonicalize %s -o %t-strict-before.cir 2>&1 | FileCheck %s -check-prefix=CIR-BEFORE

void complex_add() {
  _Complex float a;
  _Complex float b;
  _Complex float c = a + b;
}

// CIR: %[[A_ADDR:.*]] = cir.alloca "a" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[B_ADDR:.*]] = cir.alloca "b" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[C_ADDR:.*]] = cir.alloca "c" {{.*}} init : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[TMP_A:.*]] = cir.load {{.*}} %[[A_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[TMP_B:.*]] = cir.load {{.*}} %[[B_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[RESULT:.*]] = cir.complex.fadd %[[TMP_A]], %[[TMP_B]] : !cir.complex<!cir.float> fenv<dynamic_rounding_mode = tonearest, except_mode = unknown, strict_except = true>
// CIR: cir.store {{.*}} %[[RESULT]], %[[C_ADDR]] : !cir.complex<!cir.float>, !cir.ptr<!cir.complex<!cir.float>>

// LLVM: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[TMP_A:.*]] = load { float, float }, ptr %[[A_ADDR]], align 4
// LLVM: %[[TMP_B:.*]] = load { float, float }, ptr %[[B_ADDR]], align 4
// LLVM: %[[A_REAL:.*]] = extractvalue { float, float } %[[TMP_A]], 0
// LLVM: %[[A_IMAG:.*]] = extractvalue { float, float } %[[TMP_A]], 1
// LLVM: %[[B_REAL:.*]] = extractvalue { float, float } %[[TMP_B]], 0
// LLVM: %[[B_IMAG:.*]] = extractvalue { float, float } %[[TMP_B]], 1
// LLVM: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[TMP_RESULT:.*]] = insertvalue { float, float } poison, float %[[RESULT_REAL]], 0
// LLVM: %[[RESULT:.*]] = insertvalue { float, float } %[[TMP_RESULT]], float %[[RESULT_IMAG]], 1
// LLVM: store { float, float } %[[RESULT]], ptr %[[C_ADDR]], align 4

// OGCG: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[A_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 0
// OGCG: %[[A_REAL:.*]] = load float, ptr %[[A_REAL_PTR]], align 4
// OGCG: %[[A_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 1
// OGCG: %[[A_IMAG:.*]] = load float, ptr %[[A_IMAG_PTR]], align 4
// OGCG: %[[B_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 0
// OGCG: %[[B_REAL:.*]] = load float, ptr %[[B_REAL_PTR]], align 4
// OGCG: %[[B_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 1
// OGCG: %[[B_IMAG:.*]] = load float, ptr %[[B_IMAG_PTR]], align 4
// OGCG: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict") #2
// OGCG: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict") #2
// OGCG: %[[C_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 0
// OGCG: %[[C_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 1
// OGCG: store float %[[RESULT_REAL]], ptr %[[C_REAL_PTR]], align 4
// OGCG: store float %[[RESULT_IMAG]], ptr %[[C_IMAG_PTR]], align 4

void complex_sub() {
  _Complex float a;
  _Complex float b;
  _Complex float c = a - b;
}

// CIR: %[[A_ADDR:.*]] = cir.alloca "a" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[B_ADDR:.*]] = cir.alloca "b" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[C_ADDR:.*]] = cir.alloca "c" {{.*}} init : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[TMP_A:.*]] = cir.load {{.*}} %[[A_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[TMP_B:.*]] = cir.load {{.*}} %[[B_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[RESULT:.*]] = cir.complex.fsub %[[TMP_A]], %[[TMP_B]] : !cir.complex<!cir.float> fenv<dynamic_rounding_mode = tonearest, except_mode = unknown, strict_except = true>
// CIR: cir.store {{.*}} %[[RESULT]], %[[C_ADDR]] : !cir.complex<!cir.float>, !cir.ptr<!cir.complex<!cir.float>>

// LLVM: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[TMP_A:.*]] = load { float, float }, ptr %[[A_ADDR]], align 4
// LLVM: %[[TMP_B:.*]] = load { float, float }, ptr %[[B_ADDR]], align 4
// LLVM: %[[A_REAL:.*]] = extractvalue { float, float } %[[TMP_A]], 0
// LLVM: %[[A_IMAG:.*]] = extractvalue { float, float } %[[TMP_A]], 1
// LLVM: %[[B_REAL:.*]] = extractvalue { float, float } %[[TMP_B]], 0
// LLVM: %[[B_IMAG:.*]] = extractvalue { float, float } %[[TMP_B]], 1
// LLVM: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[TMP_RESULT:.*]] = insertvalue { float, float } poison, float %[[RESULT_REAL]], 0
// LLVM: %[[RESULT:.*]] = insertvalue { float, float } %[[TMP_RESULT]], float %[[RESULT_IMAG]], 1
// LLVM: store { float, float } %[[RESULT]], ptr %[[C_ADDR]], align 4

// OGCG: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[A_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 0
// OGCG: %[[A_REAL:.*]] = load float, ptr %[[A_REAL_PTR]], align 4
// OGCG: %[[A_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 1
// OGCG: %[[A_IMAG:.*]] = load float, ptr %[[A_IMAG_PTR]], align 4
// OGCG: %[[B_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 0
// OGCG: %[[B_REAL:.*]] = load float, ptr %[[B_REAL_PTR]], align 4
// OGCG: %[[B_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 1
// OGCG: %[[B_IMAG:.*]] = load float, ptr %[[B_IMAG_PTR]], align 4
// OGCG: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict") #2
// OGCG: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict") #2
// OGCG: %[[C_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 0
// OGCG: %[[C_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 1
// OGCG: store float %[[RESULT_REAL]], ptr %[[C_REAL_PTR]], align 4
// OGCG: store float %[[RESULT_IMAG]], ptr %[[C_IMAG_PTR]], align 4

// With CX_LIMITED_RANGE, multiplication and division expand into scalar
// arithmetic only. The NaN check, the range check and the runtime library
// calls of the other ranges are tested in complex-mul-div-fenv-access.c.
//
// LoweringPrepare does the expansion, so the CIR prefix sees the scalar
// operations. Each of them carries the attribute of the complex operation.

void complex_mul() {
#pragma STDC CX_LIMITED_RANGE ON
  _Complex float a;
  _Complex float b;
  _Complex float c = a * b;
}

// CIR-BEFORE: cir.func {{.*}}@_Z11complex_mulv()
// CIR-BEFORE: %{{.*}} = cir.complex.fmul %{{.*}}, %{{.*}} range(basic) : !cir.complex<!cir.float> fenv<{{.*}}except_mode = unknown, strict_except = true>

// CIR: cir.func {{.*}}@_Z11complex_mulv()
// CIR: %[[A_ADDR:.*]] = cir.alloca "a" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[B_ADDR:.*]] = cir.alloca "b" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[C_ADDR:.*]] = cir.alloca "c" {{.*}} init : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[TMP_A:.*]] = cir.load {{.*}} %[[A_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[TMP_B:.*]] = cir.load {{.*}} %[[B_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[A_REAL:.*]] = cir.complex.real %[[TMP_A]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[A_IMAG:.*]] = cir.complex.imag %[[TMP_A]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[B_REAL:.*]] = cir.complex.real %[[TMP_B]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[B_IMAG:.*]] = cir.complex.imag %[[TMP_B]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[MUL_AC:.*]] = cir.fmul %[[A_REAL]], %[[B_REAL]] : !cir.float [[FENV:fenv<.*except_mode = unknown, strict_except = true>]]
// CIR: %[[MUL_BD:.*]] = cir.fmul %[[A_IMAG]], %[[B_IMAG]] : !cir.float [[FENV]]
// CIR: %[[MUL_AD:.*]] = cir.fmul %[[A_REAL]], %[[B_IMAG]] : !cir.float [[FENV]]
// CIR: %[[MUL_BC:.*]] = cir.fmul %[[A_IMAG]], %[[B_REAL]] : !cir.float [[FENV]]
// CIR: %[[RESULT_REAL:.*]] = cir.fsub %[[MUL_AC]], %[[MUL_BD]] : !cir.float [[FENV]]
// CIR: %[[RESULT_IMAG:.*]] = cir.fadd %[[MUL_AD]], %[[MUL_BC]] : !cir.float [[FENV]]
// CIR: %[[RESULT:.*]] = cir.complex.create %[[RESULT_REAL]], %[[RESULT_IMAG]] : !cir.float -> !cir.complex<!cir.float>
// CIR: cir.store {{.*}} %[[RESULT]], %[[C_ADDR]] : !cir.complex<!cir.float>, !cir.ptr<!cir.complex<!cir.float>>

// LLVM: define {{.*}} @_Z11complex_mulv()
// LLVM: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[TMP_A:.*]] = load { float, float }, ptr %[[A_ADDR]], align 4
// LLVM: %[[TMP_B:.*]] = load { float, float }, ptr %[[B_ADDR]], align 4
// LLVM: %[[A_REAL:.*]] = extractvalue { float, float } %[[TMP_A]], 0
// LLVM: %[[A_IMAG:.*]] = extractvalue { float, float } %[[TMP_A]], 1
// LLVM: %[[B_REAL:.*]] = extractvalue { float, float } %[[TMP_B]], 0
// LLVM: %[[B_IMAG:.*]] = extractvalue { float, float } %[[TMP_B]], 1
// LLVM: %[[MUL_AC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_BD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_AD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_BC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[MUL_AC]], float %[[MUL_BD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[MUL_AD]], float %[[MUL_BC]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[TMP_RESULT:.*]] = insertvalue { float, float } {{.*}}, float %[[RESULT_REAL]], 0
// LLVM: %[[RESULT:.*]] = insertvalue { float, float } %[[TMP_RESULT]], float %[[RESULT_IMAG]], 1
// LLVM: store { float, float } %[[RESULT]], ptr %[[C_ADDR]], align 4

// OGCG: define {{.*}} @_Z11complex_mulv()
// OGCG: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[A_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 0
// OGCG: %[[A_REAL:.*]] = load float, ptr %[[A_REAL_PTR]], align 4
// OGCG: %[[A_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 1
// OGCG: %[[A_IMAG:.*]] = load float, ptr %[[A_IMAG_PTR]], align 4
// OGCG: %[[B_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 0
// OGCG: %[[B_REAL:.*]] = load float, ptr %[[B_REAL_PTR]], align 4
// OGCG: %[[B_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 1
// OGCG: %[[B_IMAG:.*]] = load float, ptr %[[B_IMAG_PTR]], align 4
// OGCG: %[[MUL_AC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_BD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_AD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_BC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[MUL_AC]], float %[[MUL_BD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[MUL_AD]], float %[[MUL_BC]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[C_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 0
// OGCG: %[[C_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 1
// OGCG: store float %[[RESULT_REAL]], ptr %[[C_REAL_PTR]], align 4
// OGCG: store float %[[RESULT_IMAG]], ptr %[[C_IMAG_PTR]], align 4

void complex_div() {
#pragma STDC CX_LIMITED_RANGE ON
  _Complex float a;
  _Complex float b;
  _Complex float c = a / b;
}

// CIR-BEFORE: cir.func {{.*}}@_Z11complex_divv()
// CIR-BEFORE: %{{.*}} = cir.complex.fdiv %{{.*}}, %{{.*}} range(basic) : !cir.complex<!cir.float> fenv<{{.*}}except_mode = unknown, strict_except = true>

// CIR: cir.func {{.*}}@_Z11complex_divv()
// CIR: %[[A_ADDR:.*]] = cir.alloca "a" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[B_ADDR:.*]] = cir.alloca "b" {{.*}} : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[C_ADDR:.*]] = cir.alloca "c" {{.*}} init : !cir.ptr<!cir.complex<!cir.float>>
// CIR: %[[TMP_A:.*]] = cir.load {{.*}} %[[A_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[TMP_B:.*]] = cir.load {{.*}} %[[B_ADDR]] : !cir.ptr<!cir.complex<!cir.float>>, !cir.complex<!cir.float>
// CIR: %[[A_REAL:.*]] = cir.complex.real %[[TMP_A]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[A_IMAG:.*]] = cir.complex.imag %[[TMP_A]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[B_REAL:.*]] = cir.complex.real %[[TMP_B]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[B_IMAG:.*]] = cir.complex.imag %[[TMP_B]] : !cir.complex<!cir.float> -> !cir.float
// CIR: %[[MUL_AC:.*]] = cir.fmul %[[A_REAL]], %[[B_REAL]] : !cir.float [[FENV:fenv<.*except_mode = unknown, strict_except = true>]]
// CIR: %[[MUL_BD:.*]] = cir.fmul %[[A_IMAG]], %[[B_IMAG]] : !cir.float [[FENV]]
// CIR: %[[MUL_CC:.*]] = cir.fmul %[[B_REAL]], %[[B_REAL]] : !cir.float [[FENV]]
// CIR: %[[MUL_DD:.*]] = cir.fmul %[[B_IMAG]], %[[B_IMAG]] : !cir.float [[FENV]]
// CIR: %[[ADD_ACBD:.*]] = cir.fadd %[[MUL_AC]], %[[MUL_BD]] : !cir.float [[FENV]]
// CIR: %[[ADD_CCDD:.*]] = cir.fadd %[[MUL_CC]], %[[MUL_DD]] : !cir.float [[FENV]]
// CIR: %[[RESULT_REAL:.*]] = cir.fdiv %[[ADD_ACBD]], %[[ADD_CCDD]] : !cir.float [[FENV]]
// CIR: %[[MUL_BC:.*]] = cir.fmul %[[A_IMAG]], %[[B_REAL]] : !cir.float [[FENV]]
// CIR: %[[MUL_AD:.*]] = cir.fmul %[[A_REAL]], %[[B_IMAG]] : !cir.float [[FENV]]
// CIR: %[[SUB_BCAD:.*]] = cir.fsub %[[MUL_BC]], %[[MUL_AD]] : !cir.float [[FENV]]
// CIR: %[[RESULT_IMAG:.*]] = cir.fdiv %[[SUB_BCAD]], %[[ADD_CCDD]] : !cir.float [[FENV]]
// CIR: %[[RESULT:.*]] = cir.complex.create %[[RESULT_REAL]], %[[RESULT_IMAG]] : !cir.float -> !cir.complex<!cir.float>
// CIR: cir.store {{.*}} %[[RESULT]], %[[C_ADDR]] : !cir.complex<!cir.float>, !cir.ptr<!cir.complex<!cir.float>>

// LLVM: define {{.*}} @_Z11complex_divv()
// LLVM: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// LLVM: %[[TMP_A:.*]] = load { float, float }, ptr %[[A_ADDR]], align 4
// LLVM: %[[TMP_B:.*]] = load { float, float }, ptr %[[B_ADDR]], align 4
// LLVM: %[[A_REAL:.*]] = extractvalue { float, float } %[[TMP_A]], 0
// LLVM: %[[A_IMAG:.*]] = extractvalue { float, float } %[[TMP_A]], 1
// LLVM: %[[B_REAL:.*]] = extractvalue { float, float } %[[TMP_B]], 0
// LLVM: %[[B_IMAG:.*]] = extractvalue { float, float } %[[TMP_B]], 1
// LLVM: %[[MUL_AC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_BD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_CC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[B_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_DD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[B_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[ADD_ACBD:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[MUL_AC]], float %[[MUL_BD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[ADD_CCDD:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[MUL_CC]], float %[[MUL_DD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fdiv.f32(float %[[ADD_ACBD]], float %[[ADD_CCDD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_BC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[MUL_AD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[SUB_BCAD:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[MUL_BC]], float %[[MUL_AD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fdiv.f32(float %[[SUB_BCAD]], float %[[ADD_CCDD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// LLVM: %[[TMP_RESULT:.*]] = insertvalue { float, float } {{.*}}, float %[[RESULT_REAL]], 0
// LLVM: %[[RESULT:.*]] = insertvalue { float, float } %[[TMP_RESULT]], float %[[RESULT_IMAG]], 1
// LLVM: store { float, float } %[[RESULT]], ptr %[[C_ADDR]], align 4

// OGCG: define {{.*}} @_Z11complex_divv()
// OGCG: %[[A_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[B_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[C_ADDR:.*]] = alloca { float, float }, align 4
// OGCG: %[[A_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 0
// OGCG: %[[A_REAL:.*]] = load float, ptr %[[A_REAL_PTR]], align 4
// OGCG: %[[A_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[A_ADDR]], i32 0, i32 1
// OGCG: %[[A_IMAG:.*]] = load float, ptr %[[A_IMAG_PTR]], align 4
// OGCG: %[[B_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 0
// OGCG: %[[B_REAL:.*]] = load float, ptr %[[B_REAL_PTR]], align 4
// OGCG: %[[B_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[B_ADDR]], i32 0, i32 1
// OGCG: %[[B_IMAG:.*]] = load float, ptr %[[B_IMAG_PTR]], align 4
// OGCG: %[[MUL_AC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_BD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[ADD_ACBD:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[MUL_AC]], float %[[MUL_BD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_CC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[B_REAL]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_DD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[B_IMAG]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[ADD_CCDD:.*]] = call float @llvm.experimental.constrained.fadd.f32(float %[[MUL_CC]], float %[[MUL_DD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_BC:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_IMAG]], float %[[B_REAL]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[MUL_AD:.*]] = call float @llvm.experimental.constrained.fmul.f32(float %[[A_REAL]], float %[[B_IMAG]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[SUB_BCAD:.*]] = call float @llvm.experimental.constrained.fsub.f32(float %[[MUL_BC]], float %[[MUL_AD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[RESULT_REAL:.*]] = call float @llvm.experimental.constrained.fdiv.f32(float %[[ADD_ACBD]], float %[[ADD_CCDD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[RESULT_IMAG:.*]] = call float @llvm.experimental.constrained.fdiv.f32(float %[[SUB_BCAD]], float %[[ADD_CCDD]], metadata !"round.tonearest", metadata !"fpexcept.strict")
// OGCG: %[[C_REAL_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 0
// OGCG: %[[C_IMAG_PTR:.*]] = getelementptr inbounds nuw { float, float }, ptr %[[C_ADDR]], i32 0, i32 1
// OGCG: store float %[[RESULT_REAL]], ptr %[[C_REAL_PTR]], align 4
// OGCG: store float %[[RESULT_IMAG]], ptr %[[C_IMAG_PTR]], align 4
