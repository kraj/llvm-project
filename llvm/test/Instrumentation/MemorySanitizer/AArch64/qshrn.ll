; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 4
; RUN: opt < %s -passes=msan -S | FileCheck %s
;
; Forked from llvm/test/CodeGen/AArch64/qshrn.ll
;
; Heuristically (but correctly) handled: llvm.smax, llvm.smin, llvm.umin

target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64--linux-android9001"

define <4 x i16> @NarrowAShrI32By5(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowAShrI32By5(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <4 x i32> [[TMP1]], splat (i32 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <4 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.sqxtn.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = ashr <4 x i32> %x, <i32 5, i32 5, i32 5, i32 5>
  %r = tail call <4 x i16> @llvm.aarch64.neon.sqxtn.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}

define <4 x i16> @NarrowAShrU32By5(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowAShrU32By5(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <4 x i32> [[TMP1]], splat (i32 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <4 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.uqxtn.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = ashr <4 x i32> %x, <i32 5, i32 5, i32 5, i32 5>
  %r = tail call <4 x i16> @llvm.aarch64.neon.uqxtn.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}

define <4 x i16> @NarrowAShrI32By5ToU16(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowAShrI32By5ToU16(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <4 x i32> [[TMP1]], splat (i32 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <4 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.sqxtun.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = ashr <4 x i32> %x, <i32 5, i32 5, i32 5, i32 5>
  %r = tail call <4 x i16> @llvm.aarch64.neon.sqxtun.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}

define <4 x i16> @NarrowLShrI32By5(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowLShrI32By5(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <4 x i32> [[TMP1]], splat (i32 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <4 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.sqxtn.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = lshr <4 x i32> %x, <i32 5, i32 5, i32 5, i32 5>
  %r = tail call <4 x i16> @llvm.aarch64.neon.sqxtn.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}

define <4 x i16> @NarrowLShrU32By5(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowLShrU32By5(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <4 x i32> [[TMP1]], splat (i32 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <4 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.uqxtn.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = lshr <4 x i32> %x, <i32 5, i32 5, i32 5, i32 5>
  %r = tail call <4 x i16> @llvm.aarch64.neon.uqxtn.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}

define <4 x i16> @NarrowLShrI32By5ToU16(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowLShrI32By5ToU16(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <4 x i32> [[TMP1]], splat (i32 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <4 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.sqxtun.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = lshr <4 x i32> %x, <i32 5, i32 5, i32 5, i32 5>
  %r = tail call <4 x i16> @llvm.aarch64.neon.sqxtun.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}


define <2 x i32> @NarrowAShri64By5(<2 x i64> %x) #0 {
; CHECK-LABEL: define <2 x i32> @NarrowAShri64By5(
; CHECK-SAME: <2 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <2 x i64> [[TMP1]], splat (i64 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <2 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <2 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <2 x i64> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP5:%.*]] = trunc <2 x i64> [[_MSPROP]] to <2 x i32>
; CHECK-NEXT:    [[R:%.*]] = tail call <2 x i32> @llvm.aarch64.neon.sqxtn.v2i32(<2 x i64> [[S]])
; CHECK-NEXT:    store <2 x i32> [[TMP5]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %s = ashr <2 x i64> %x, <i64 5, i64 5>
  %r = tail call <2 x i32> @llvm.aarch64.neon.sqxtn.v2i32(<2 x i64> %s)
  ret <2 x i32> %r
}

define <2 x i32> @NarrowAShrU64By5(<2 x i64> %x) #0 {
; CHECK-LABEL: define <2 x i32> @NarrowAShrU64By5(
; CHECK-SAME: <2 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <2 x i64> [[TMP1]], splat (i64 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <2 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <2 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <2 x i64> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP5:%.*]] = trunc <2 x i64> [[_MSPROP]] to <2 x i32>
; CHECK-NEXT:    [[R:%.*]] = tail call <2 x i32> @llvm.aarch64.neon.uqxtn.v2i32(<2 x i64> [[S]])
; CHECK-NEXT:    store <2 x i32> [[TMP5]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %s = ashr <2 x i64> %x, <i64 5, i64 5>
  %r = tail call <2 x i32> @llvm.aarch64.neon.uqxtn.v2i32(<2 x i64> %s)
  ret <2 x i32> %r
}

define <2 x i32> @NarrowAShri64By5ToU32(<2 x i64> %x) #0 {
; CHECK-LABEL: define <2 x i32> @NarrowAShri64By5ToU32(
; CHECK-SAME: <2 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <2 x i64> [[TMP1]], splat (i64 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <2 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <2 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <2 x i64> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP5:%.*]] = trunc <2 x i64> [[_MSPROP]] to <2 x i32>
; CHECK-NEXT:    [[R:%.*]] = tail call <2 x i32> @llvm.aarch64.neon.sqxtun.v2i32(<2 x i64> [[S]])
; CHECK-NEXT:    store <2 x i32> [[TMP5]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %s = ashr <2 x i64> %x, <i64 5, i64 5>
  %r = tail call <2 x i32> @llvm.aarch64.neon.sqxtun.v2i32(<2 x i64> %s)
  ret <2 x i32> %r
}

define <2 x i32> @NarrowLShri64By5(<2 x i64> %x) #0 {
; CHECK-LABEL: define <2 x i32> @NarrowLShri64By5(
; CHECK-SAME: <2 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <2 x i64> [[TMP1]], splat (i64 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <2 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <2 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <2 x i64> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP5:%.*]] = trunc <2 x i64> [[_MSPROP]] to <2 x i32>
; CHECK-NEXT:    [[R:%.*]] = tail call <2 x i32> @llvm.aarch64.neon.sqxtn.v2i32(<2 x i64> [[S]])
; CHECK-NEXT:    store <2 x i32> [[TMP5]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %s = lshr <2 x i64> %x, <i64 5, i64 5>
  %r = tail call <2 x i32> @llvm.aarch64.neon.sqxtn.v2i32(<2 x i64> %s)
  ret <2 x i32> %r
}

define <2 x i32> @NarrowLShrU64By5(<2 x i64> %x) #0 {
; CHECK-LABEL: define <2 x i32> @NarrowLShrU64By5(
; CHECK-SAME: <2 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <2 x i64> [[TMP1]], splat (i64 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <2 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <2 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <2 x i64> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP5:%.*]] = trunc <2 x i64> [[_MSPROP]] to <2 x i32>
; CHECK-NEXT:    [[R:%.*]] = tail call <2 x i32> @llvm.aarch64.neon.uqxtn.v2i32(<2 x i64> [[S]])
; CHECK-NEXT:    store <2 x i32> [[TMP5]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %s = lshr <2 x i64> %x, <i64 5, i64 5>
  %r = tail call <2 x i32> @llvm.aarch64.neon.uqxtn.v2i32(<2 x i64> %s)
  ret <2 x i32> %r
}

define <2 x i32> @NarrowLShri64By5ToU32(<2 x i64> %x) #0 {
; CHECK-LABEL: define <2 x i32> @NarrowLShri64By5ToU32(
; CHECK-SAME: <2 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <2 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <2 x i64> [[TMP1]], splat (i64 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <2 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <2 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <2 x i64> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP5:%.*]] = trunc <2 x i64> [[_MSPROP]] to <2 x i32>
; CHECK-NEXT:    [[R:%.*]] = tail call <2 x i32> @llvm.aarch64.neon.sqxtun.v2i32(<2 x i64> [[S]])
; CHECK-NEXT:    store <2 x i32> [[TMP5]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %s = lshr <2 x i64> %x, <i64 5, i64 5>
  %r = tail call <2 x i32> @llvm.aarch64.neon.sqxtun.v2i32(<2 x i64> %s)
  ret <2 x i32> %r
}


define <8 x i8> @NarrowAShri16By5(<8 x i16> %x) #0 {
; CHECK-LABEL: define <8 x i8> @NarrowAShri16By5(
; CHECK-SAME: <8 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <8 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <8 x i16> [[TMP1]], splat (i16 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <8 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <8 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i16> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <8 x i16> [[_MSPROP]] to <8 x i8>
; CHECK-NEXT:    [[R:%.*]] = tail call <8 x i8> @llvm.aarch64.neon.sqxtn.v8i8(<8 x i16> [[S]])
; CHECK-NEXT:    store <8 x i8> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i8> [[R]]
;
  %s = ashr <8 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %r = tail call <8 x i8> @llvm.aarch64.neon.sqxtn.v8i8(<8 x i16> %s)
  ret <8 x i8> %r
}

define <8 x i8> @NarrowAShrU16By5(<8 x i16> %x) #0 {
; CHECK-LABEL: define <8 x i8> @NarrowAShrU16By5(
; CHECK-SAME: <8 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <8 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <8 x i16> [[TMP1]], splat (i16 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <8 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <8 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i16> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <8 x i16> [[_MSPROP]] to <8 x i8>
; CHECK-NEXT:    [[R:%.*]] = tail call <8 x i8> @llvm.aarch64.neon.uqxtn.v8i8(<8 x i16> [[S]])
; CHECK-NEXT:    store <8 x i8> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i8> [[R]]
;
  %s = ashr <8 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %r = tail call <8 x i8> @llvm.aarch64.neon.uqxtn.v8i8(<8 x i16> %s)
  ret <8 x i8> %r
}

define <8 x i8> @NarrowAShri16By5ToU8(<8 x i16> %x) #0 {
; CHECK-LABEL: define <8 x i8> @NarrowAShri16By5ToU8(
; CHECK-SAME: <8 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <8 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <8 x i16> [[TMP1]], splat (i16 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <8 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <8 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i16> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <8 x i16> [[_MSPROP]] to <8 x i8>
; CHECK-NEXT:    [[R:%.*]] = tail call <8 x i8> @llvm.aarch64.neon.sqxtun.v8i8(<8 x i16> [[S]])
; CHECK-NEXT:    store <8 x i8> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i8> [[R]]
;
  %s = ashr <8 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %r = tail call <8 x i8> @llvm.aarch64.neon.sqxtun.v8i8(<8 x i16> %s)
  ret <8 x i8> %r
}

define <8 x i8> @NarrowLShri16By5(<8 x i16> %x) #0 {
; CHECK-LABEL: define <8 x i8> @NarrowLShri16By5(
; CHECK-SAME: <8 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <8 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <8 x i16> [[TMP1]], splat (i16 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <8 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <8 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i16> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <8 x i16> [[_MSPROP]] to <8 x i8>
; CHECK-NEXT:    [[R:%.*]] = tail call <8 x i8> @llvm.aarch64.neon.sqxtn.v8i8(<8 x i16> [[S]])
; CHECK-NEXT:    store <8 x i8> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i8> [[R]]
;
  %s = lshr <8 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %r = tail call <8 x i8> @llvm.aarch64.neon.sqxtn.v8i8(<8 x i16> %s)
  ret <8 x i8> %r
}

define <8 x i8> @NarrowLShrU16By5(<8 x i16> %x) #0 {
; CHECK-LABEL: define <8 x i8> @NarrowLShrU16By5(
; CHECK-SAME: <8 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <8 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <8 x i16> [[TMP1]], splat (i16 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <8 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <8 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i16> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <8 x i16> [[_MSPROP]] to <8 x i8>
; CHECK-NEXT:    [[R:%.*]] = tail call <8 x i8> @llvm.aarch64.neon.uqxtn.v8i8(<8 x i16> [[S]])
; CHECK-NEXT:    store <8 x i8> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i8> [[R]]
;
  %s = lshr <8 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %r = tail call <8 x i8> @llvm.aarch64.neon.uqxtn.v8i8(<8 x i16> %s)
  ret <8 x i8> %r
}

define <8 x i8> @NarrowLShri16By5ToU8(<8 x i16> %x) #0 {
; CHECK-LABEL: define <8 x i8> @NarrowLShri16By5ToU8(
; CHECK-SAME: <8 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <8 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <8 x i16> [[TMP1]], splat (i16 5)
; CHECK-NEXT:    [[TMP3:%.*]] = or <8 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <8 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i16> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <8 x i16> [[_MSPROP]] to <8 x i8>
; CHECK-NEXT:    [[R:%.*]] = tail call <8 x i8> @llvm.aarch64.neon.sqxtun.v8i8(<8 x i16> [[S]])
; CHECK-NEXT:    store <8 x i8> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i8> [[R]]
;
  %s = lshr <8 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %r = tail call <8 x i8> @llvm.aarch64.neon.sqxtun.v8i8(<8 x i16> %s)
  ret <8 x i8> %r
}





define <4 x i16> @NarrowAShrI32By31(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowAShrI32By31(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <4 x i32> [[TMP1]], splat (i32 16)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <4 x i32> [[X]], splat (i32 16)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.sqxtn.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = ashr <4 x i32> %x, <i32 16, i32 16, i32 16, i32 16>
  %r = tail call <4 x i16> @llvm.aarch64.neon.sqxtn.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}

define <4 x i16> @NarrowAShrI32By31ToU16(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowAShrI32By31ToU16(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = ashr <4 x i32> [[TMP1]], splat (i32 16)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <4 x i32> [[X]], splat (i32 16)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.sqxtun.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = ashr <4 x i32> %x, <i32 16, i32 16, i32 16, i32 16>
  %r = tail call <4 x i16> @llvm.aarch64.neon.sqxtun.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}

define <4 x i16> @NarrowLShrU32By31(<4 x i32> %x) #0 {
; CHECK-LABEL: define <4 x i16> @NarrowLShrU32By31(
; CHECK-SAME: <4 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:    [[TMP1:%.*]] = load <4 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP2:%.*]] = lshr <4 x i32> [[TMP1]], splat (i32 16)
; CHECK-NEXT:    [[TMP3:%.*]] = or <4 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <4 x i32> [[X]], splat (i32 16)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i32> [[TMP3]], zeroinitializer
; CHECK-NEXT:    [[TMP6:%.*]] = trunc <4 x i32> [[_MSPROP]] to <4 x i16>
; CHECK-NEXT:    [[R:%.*]] = tail call <4 x i16> @llvm.aarch64.neon.uqxtn.v4i16(<4 x i32> [[S]])
; CHECK-NEXT:    store <4 x i16> [[TMP6]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i16> [[R]]
;
  %s = lshr <4 x i32> %x, <i32 16, i32 16, i32 16, i32 16>
  %r = tail call <4 x i16> @llvm.aarch64.neon.uqxtn.v4i16(<4 x i32> %s)
  ret <4 x i16> %r
}


define <16 x i8> @signed_minmax_v8i16_to_v16i8(<16 x i16> %x) #0 {
; CHECK-LABEL: define <16 x i8> @signed_minmax_v8i16_to_v16i8(
; CHECK-SAME: <16 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <16 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = ashr <16 x i16> [[TMP0]], splat (i16 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <16 x i16> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <16 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <16 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <16 x i16> @llvm.smin.v16i16(<16 x i16> [[S]], <16 x i16> splat (i16 127))
; CHECK-NEXT:    [[_MSPROP1:%.*]] = or <16 x i16> [[_MSPROP]], zeroinitializer
; CHECK-NEXT:    [[MAX:%.*]] = call <16 x i16> @llvm.smax.v16i16(<16 x i16> [[MIN]], <16 x i16> splat (i16 -128))
; CHECK-NEXT:    [[_MSPROP2:%.*]] = trunc <16 x i16> [[_MSPROP1]] to <16 x i8>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <16 x i16> [[MAX]] to <16 x i8>
; CHECK-NEXT:    store <16 x i8> [[_MSPROP2]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <16 x i8> [[TRUNC]]
;
entry:
  %s = ashr <16 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %min = call <16 x i16> @llvm.smin.v8i16(<16 x i16> %s, <16 x i16> <i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127, i16 127>)
  %max = call <16 x i16> @llvm.smax.v8i16(<16 x i16> %min, <16 x i16> <i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128, i16 -128>)
  %trunc = trunc <16 x i16> %max to <16 x i8>
  ret <16 x i8> %trunc
}

define <16 x i8> @unsigned_minmax_v8i16_to_v16i8(<16 x i16> %x) #0 {
; CHECK-LABEL: define <16 x i8> @unsigned_minmax_v8i16_to_v16i8(
; CHECK-SAME: <16 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <16 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = lshr <16 x i16> [[TMP0]], splat (i16 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <16 x i16> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <16 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <16 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <16 x i16> @llvm.umin.v16i16(<16 x i16> [[S]], <16 x i16> splat (i16 255))
; CHECK-NEXT:    [[_MSPROP1:%.*]] = trunc <16 x i16> [[_MSPROP]] to <16 x i8>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <16 x i16> [[MIN]] to <16 x i8>
; CHECK-NEXT:    store <16 x i8> [[_MSPROP1]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <16 x i8> [[TRUNC]]
;
entry:
  %s = lshr <16 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %min = call <16 x i16> @llvm.umin.v8i16(<16 x i16> %s, <16 x i16> <i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255>)
  %trunc = trunc <16 x i16> %min to <16 x i8>
  ret <16 x i8> %trunc
}

define <16 x i8> @unsigned_signed_minmax_v8i16_to_v16i8(<16 x i16> %x) #0 {
; CHECK-LABEL: define <16 x i8> @unsigned_signed_minmax_v8i16_to_v16i8(
; CHECK-SAME: <16 x i16> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <16 x i16>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = ashr <16 x i16> [[TMP0]], splat (i16 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <16 x i16> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <16 x i16> [[X]], splat (i16 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <16 x i16> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MAX:%.*]] = call <16 x i16> @llvm.smax.v16i16(<16 x i16> [[S]], <16 x i16> zeroinitializer)
; CHECK-NEXT:    [[_MSPROP1:%.*]] = or <16 x i16> [[_MSPROP]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <16 x i16> @llvm.umin.v16i16(<16 x i16> [[MAX]], <16 x i16> splat (i16 255))
; CHECK-NEXT:    [[_MSPROP2:%.*]] = trunc <16 x i16> [[_MSPROP1]] to <16 x i8>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <16 x i16> [[MIN]] to <16 x i8>
; CHECK-NEXT:    store <16 x i8> [[_MSPROP2]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <16 x i8> [[TRUNC]]
;
entry:
  %s = ashr <16 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %max = call <16 x i16> @llvm.smax.v8i16(<16 x i16> %s, <16 x i16> <i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0, i16 0>)
  %min = call <16 x i16> @llvm.umin.v8i16(<16 x i16> %max, <16 x i16> <i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255, i16 255>)
  %trunc = trunc <16 x i16> %min to <16 x i8>
  ret <16 x i8> %trunc
}


define <8 x i16> @signed_minmax_v4i32_to_v8i16(<8 x i32> %x) #0 {
; CHECK-LABEL: define <8 x i16> @signed_minmax_v4i32_to_v8i16(
; CHECK-SAME: <8 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <8 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = ashr <8 x i32> [[TMP0]], splat (i32 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <8 x i32> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <8 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <8 x i32> @llvm.smin.v8i32(<8 x i32> [[S]], <8 x i32> splat (i32 32767))
; CHECK-NEXT:    [[_MSPROP1:%.*]] = or <8 x i32> [[_MSPROP]], zeroinitializer
; CHECK-NEXT:    [[MAX:%.*]] = call <8 x i32> @llvm.smax.v8i32(<8 x i32> [[MIN]], <8 x i32> splat (i32 -32768))
; CHECK-NEXT:    [[_MSPROP2:%.*]] = trunc <8 x i32> [[_MSPROP1]] to <8 x i16>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <8 x i32> [[MAX]] to <8 x i16>
; CHECK-NEXT:    store <8 x i16> [[_MSPROP2]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i16> [[TRUNC]]
;
entry:
  %s = ashr <8 x i32> %x, <i32 5, i32 5, i32 5, i32 5, i32 5, i32 5, i32 5, i32 5>
  %min = call <8 x i32> @llvm.smin.v8i32(<8 x i32> %s, <8 x i32> <i32 32767, i32 32767, i32 32767, i32 32767, i32 32767, i32 32767, i32 32767, i32 32767>)
  %max = call <8 x i32> @llvm.smax.v8i32(<8 x i32> %min, <8 x i32> <i32 -32768, i32 -32768, i32 -32768, i32 -32768, i32 -32768, i32 -32768, i32 -32768, i32 -32768>)
  %trunc = trunc <8 x i32> %max to <8 x i16>
  ret <8 x i16> %trunc
}

define <8 x i16> @unsigned_minmax_v4i32_to_v8i16(<8 x i32> %x) #0 {
; CHECK-LABEL: define <8 x i16> @unsigned_minmax_v4i32_to_v8i16(
; CHECK-SAME: <8 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <8 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = lshr <8 x i32> [[TMP0]], splat (i32 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <8 x i32> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <8 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <8 x i32> @llvm.umin.v8i32(<8 x i32> [[S]], <8 x i32> splat (i32 65535))
; CHECK-NEXT:    [[_MSPROP1:%.*]] = trunc <8 x i32> [[_MSPROP]] to <8 x i16>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <8 x i32> [[MIN]] to <8 x i16>
; CHECK-NEXT:    store <8 x i16> [[_MSPROP1]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i16> [[TRUNC]]
;
entry:
  %s = lshr <8 x i32> %x, <i32 5, i32 5, i32 5, i32 5, i32 5, i32 5, i32 5, i32 5>
  %min = call <8 x i32> @llvm.umin.v8i32(<8 x i32> %s, <8 x i32> <i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535>)
  %trunc = trunc <8 x i32> %min to <8 x i16>
  ret <8 x i16> %trunc
}

define <8 x i16> @unsigned_signed_minmax_v4i32_to_v8i16(<8 x i32> %x) #0 {
; CHECK-LABEL: define <8 x i16> @unsigned_signed_minmax_v4i32_to_v8i16(
; CHECK-SAME: <8 x i32> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <8 x i32>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = ashr <8 x i32> [[TMP0]], splat (i32 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <8 x i32> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <8 x i32> [[X]], splat (i32 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <8 x i32> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MAX:%.*]] = call <8 x i32> @llvm.smax.v8i32(<8 x i32> [[S]], <8 x i32> zeroinitializer)
; CHECK-NEXT:    [[_MSPROP1:%.*]] = or <8 x i32> [[_MSPROP]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <8 x i32> @llvm.umin.v8i32(<8 x i32> [[MAX]], <8 x i32> splat (i32 65535))
; CHECK-NEXT:    [[_MSPROP2:%.*]] = trunc <8 x i32> [[_MSPROP1]] to <8 x i16>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <8 x i32> [[MIN]] to <8 x i16>
; CHECK-NEXT:    store <8 x i16> [[_MSPROP2]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <8 x i16> [[TRUNC]]
;
entry:
  %s = ashr <8 x i32> %x, <i32 5, i32 5, i32 5, i32 5, i32 5, i32 5, i32 5, i32 5>
  %max = call <8 x i32> @llvm.smax.v8i32(<8 x i32> %s, <8 x i32> <i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0>)
  %min = call <8 x i32> @llvm.umin.v8i32(<8 x i32> %max, <8 x i32> <i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535, i32 65535>)
  %trunc = trunc <8 x i32> %min to <8 x i16>
  ret <8 x i16> %trunc
}


define <4 x i32> @signed_minmax_v4i64_to_v8i32(<4 x i64> %x) #0 {
; CHECK-LABEL: define <4 x i32> @signed_minmax_v4i64_to_v8i32(
; CHECK-SAME: <4 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <4 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = ashr <4 x i64> [[TMP0]], splat (i64 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <4 x i64> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <4 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <4 x i64> @llvm.smin.v4i64(<4 x i64> [[S]], <4 x i64> splat (i64 2147483647))
; CHECK-NEXT:    [[_MSPROP1:%.*]] = or <4 x i64> [[_MSPROP]], zeroinitializer
; CHECK-NEXT:    [[MAX:%.*]] = call <4 x i64> @llvm.smax.v4i64(<4 x i64> [[MIN]], <4 x i64> splat (i64 -2147483648))
; CHECK-NEXT:    [[_MSPROP2:%.*]] = trunc <4 x i64> [[_MSPROP1]] to <4 x i32>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <4 x i64> [[MAX]] to <4 x i32>
; CHECK-NEXT:    store <4 x i32> [[_MSPROP2]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i32> [[TRUNC]]
;
entry:
  %s = ashr <4 x i64> %x, <i64 5, i64 5, i64 5, i64 5>
  %min = call <4 x i64> @llvm.smin.v8i64(<4 x i64> %s, <4 x i64> <i64 2147483647, i64 2147483647, i64 2147483647, i64 2147483647>)
  %max = call <4 x i64> @llvm.smax.v8i64(<4 x i64> %min, <4 x i64> <i64 -2147483648, i64 -2147483648, i64 -2147483648, i64 -2147483648>)
  %trunc = trunc <4 x i64> %max to <4 x i32>
  ret <4 x i32> %trunc
}

define <4 x i32> @unsigned_minmax_v4i64_to_v8i32(<4 x i64> %x) #0 {
; CHECK-LABEL: define <4 x i32> @unsigned_minmax_v4i64_to_v8i32(
; CHECK-SAME: <4 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <4 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = lshr <4 x i64> [[TMP0]], splat (i64 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <4 x i64> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = lshr <4 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <4 x i64> @llvm.umin.v4i64(<4 x i64> [[S]], <4 x i64> splat (i64 4294967295))
; CHECK-NEXT:    [[_MSPROP1:%.*]] = trunc <4 x i64> [[_MSPROP]] to <4 x i32>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <4 x i64> [[MIN]] to <4 x i32>
; CHECK-NEXT:    store <4 x i32> [[_MSPROP1]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i32> [[TRUNC]]
;
entry:
  %s = lshr <4 x i64> %x, <i64 5, i64 5, i64 5, i64 5>
  %min = call <4 x i64> @llvm.umin.v8i64(<4 x i64> %s, <4 x i64> <i64 4294967295, i64 4294967295, i64 4294967295, i64 4294967295>)
  %trunc = trunc <4 x i64> %min to <4 x i32>
  ret <4 x i32> %trunc
}

define <4 x i32> @unsigned_signed_minmax_v4i64_to_v8i32(<4 x i64> %x) #0 {
; CHECK-LABEL: define <4 x i32> @unsigned_signed_minmax_v4i64_to_v8i32(
; CHECK-SAME: <4 x i64> [[X:%.*]]) #[[ATTR0]] {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load <4 x i64>, ptr @__msan_param_tls, align 8
; CHECK-NEXT:    call void @llvm.donothing()
; CHECK-NEXT:    [[TMP1:%.*]] = ashr <4 x i64> [[TMP0]], splat (i64 5)
; CHECK-NEXT:    [[TMP2:%.*]] = or <4 x i64> [[TMP1]], zeroinitializer
; CHECK-NEXT:    [[S:%.*]] = ashr <4 x i64> [[X]], splat (i64 5)
; CHECK-NEXT:    [[_MSPROP:%.*]] = or <4 x i64> [[TMP2]], zeroinitializer
; CHECK-NEXT:    [[MAX:%.*]] = call <4 x i64> @llvm.smax.v4i64(<4 x i64> [[S]], <4 x i64> zeroinitializer)
; CHECK-NEXT:    [[_MSPROP1:%.*]] = or <4 x i64> [[_MSPROP]], zeroinitializer
; CHECK-NEXT:    [[MIN:%.*]] = call <4 x i64> @llvm.umin.v4i64(<4 x i64> [[MAX]], <4 x i64> splat (i64 4294967295))
; CHECK-NEXT:    [[_MSPROP2:%.*]] = trunc <4 x i64> [[_MSPROP1]] to <4 x i32>
; CHECK-NEXT:    [[TRUNC:%.*]] = trunc <4 x i64> [[MIN]] to <4 x i32>
; CHECK-NEXT:    store <4 x i32> [[_MSPROP2]], ptr @__msan_retval_tls, align 8
; CHECK-NEXT:    ret <4 x i32> [[TRUNC]]
;
entry:
  %s = ashr <4 x i64> %x, <i64 5, i64 5, i64 5, i64 5>
  %max = call <4 x i64> @llvm.smax.v8i64(<4 x i64> %s, <4 x i64> <i64 0, i64 0, i64 0, i64 0>)
  %min = call <4 x i64> @llvm.umin.v8i64(<4 x i64> %max, <4 x i64> <i64 4294967295, i64 4294967295, i64 4294967295, i64 4294967295>)
  %trunc = trunc <4 x i64> %min to <4 x i32>
  ret <4 x i32> %trunc
}

attributes #0 = { sanitize_memory }
