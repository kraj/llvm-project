; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mattr=+sve -force-streaming-compatible < %s | FileCheck %s
; RUN: llc -mattr=+sme -force-streaming < %s | FileCheck %s
; RUN: llc -force-streaming-compatible < %s | FileCheck %s --check-prefix=NONEON-NOSVE

target triple = "aarch64-unknown-linux-gnu"

define half @s32_to_f16(i32 %x) {
; CHECK-LABEL: s32_to_f16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov s0, w0
; CHECK-NEXT:    ptrue p0.s
; CHECK-NEXT:    scvtf z0.h, p0/m, z0.s
; CHECK-NEXT:    // kill: def $h0 killed $h0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: s32_to_f16:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    scvtf s0, w0
; NONEON-NOSVE-NEXT:    fcvt h0, s0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = sitofp i32 %x to half
  ret half %cvt
}

define float @s32_to_f32(i32 %x) {
; CHECK-LABEL: s32_to_f32:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov s0, w0
; CHECK-NEXT:    ptrue p0.s
; CHECK-NEXT:    scvtf z0.s, p0/m, z0.s
; CHECK-NEXT:    // kill: def $s0 killed $s0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: s32_to_f32:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    scvtf s0, w0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = sitofp i32 %x to float
  ret float %cvt
}

define double @s32_to_f64(i32 %x) {
; CHECK-LABEL: s32_to_f64:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    scvtf d0, w0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: s32_to_f64:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    scvtf d0, w0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = sitofp i32 %x to double
  ret double %cvt
}

define half @u32_to_f16(i32 %x) {
; CHECK-LABEL: u32_to_f16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov s0, w0
; CHECK-NEXT:    ptrue p0.s
; CHECK-NEXT:    ucvtf z0.h, p0/m, z0.s
; CHECK-NEXT:    // kill: def $h0 killed $h0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: u32_to_f16:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    ucvtf s0, w0
; NONEON-NOSVE-NEXT:    fcvt h0, s0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = uitofp i32 %x to half
  ret half %cvt
}

define float @u32_to_f32(i32 %x) {
; CHECK-LABEL: u32_to_f32:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov s0, w0
; CHECK-NEXT:    ptrue p0.s
; CHECK-NEXT:    ucvtf z0.s, p0/m, z0.s
; CHECK-NEXT:    // kill: def $s0 killed $s0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: u32_to_f32:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    ucvtf s0, w0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = uitofp i32 %x to float
  ret float %cvt
}

define double @u32_to_f64(i32 %x) {
; CHECK-LABEL: u32_to_f64:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    ucvtf d0, w0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: u32_to_f64:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    ucvtf d0, w0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = uitofp i32 %x to double
  ret double %cvt
}

define half @s64_to_f16(i64 %x) {
; CHECK-LABEL: s64_to_f16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov d0, x0
; CHECK-NEXT:    ptrue p0.d
; CHECK-NEXT:    scvtf z0.h, p0/m, z0.d
; CHECK-NEXT:    // kill: def $h0 killed $h0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: s64_to_f16:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    scvtf s0, x0
; NONEON-NOSVE-NEXT:    fcvt h0, s0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = sitofp i64 %x to half
  ret half %cvt
}

define float @s64_to_f32(i64 %x) {
; CHECK-LABEL: s64_to_f32:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov d0, x0
; CHECK-NEXT:    ptrue p0.d
; CHECK-NEXT:    scvtf z0.s, p0/m, z0.d
; CHECK-NEXT:    // kill: def $s0 killed $s0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: s64_to_f32:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    scvtf s0, x0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = sitofp i64 %x to float
  ret float %cvt
}

define double @s64_to_f64(i64 %x) {
; CHECK-LABEL: s64_to_f64:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov d0, x0
; CHECK-NEXT:    ptrue p0.d
; CHECK-NEXT:    scvtf z0.d, p0/m, z0.d
; CHECK-NEXT:    // kill: def $d0 killed $d0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: s64_to_f64:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    scvtf d0, x0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = sitofp i64 %x to double
  ret double %cvt
}

define half @u64_to_f16(i64 %x) {
; CHECK-LABEL: u64_to_f16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov d0, x0
; CHECK-NEXT:    ptrue p0.d
; CHECK-NEXT:    ucvtf z0.h, p0/m, z0.d
; CHECK-NEXT:    // kill: def $h0 killed $h0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: u64_to_f16:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    ucvtf s0, x0
; NONEON-NOSVE-NEXT:    fcvt h0, s0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = uitofp i64 %x to half
  ret half %cvt
}

define float @u64_to_f32(i64 %x) {
; CHECK-LABEL: u64_to_f32:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov d0, x0
; CHECK-NEXT:    ptrue p0.d
; CHECK-NEXT:    ucvtf z0.s, p0/m, z0.d
; CHECK-NEXT:    // kill: def $s0 killed $s0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: u64_to_f32:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    ucvtf s0, x0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = uitofp i64 %x to float
  ret float %cvt
}

define double @u64_to_f64(i64 %x) {
; CHECK-LABEL: u64_to_f64:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    fmov d0, x0
; CHECK-NEXT:    ptrue p0.d
; CHECK-NEXT:    ucvtf z0.d, p0/m, z0.d
; CHECK-NEXT:    // kill: def $d0 killed $d0 killed $z0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: u64_to_f64:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    ucvtf d0, x0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = uitofp i64 %x to double
  ret double %cvt
}

define float @strict_convert_signed(i32 %x) {
; CHECK-LABEL: strict_convert_signed:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    scvtf s0, w0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: strict_convert_signed:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    scvtf s0, w0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = call float @llvm.experimental.constrained.sitofp.f32.i32(i32 %x, metadata !"round.tonearest", metadata !"fpexcept.strict") #0
  ret float %cvt
}

define float @strict_convert_unsigned(i64 %x) {
; CHECK-LABEL: strict_convert_unsigned:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    ucvtf s0, x0
; CHECK-NEXT:    ret
;
; NONEON-NOSVE-LABEL: strict_convert_unsigned:
; NONEON-NOSVE:       // %bb.0: // %entry
; NONEON-NOSVE-NEXT:    ucvtf s0, x0
; NONEON-NOSVE-NEXT:    ret
entry:
  %cvt = call float @llvm.experimental.constrained.uitofp.f32.i64(i64 %x, metadata !"round.tonearest", metadata !"fpexcept.strict") #0
  ret float %cvt
}

attributes #0 = { strictfp }
