; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 5
; RUN: opt -S < %s | FileCheck %s

define void @strip_bitcast() {
; CHECK-LABEL: define void @strip_bitcast() {
; CHECK-NEXT:    [[A:%.*]] = alloca i8, align 1
; CHECK-NEXT:    [[B:%.*]] = bitcast ptr [[A]] to ptr
; CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 1, ptr [[A]])
; CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 1, ptr [[A]])
; CHECK-NEXT:    ret void
;
  %a = alloca i8
  %b = bitcast ptr %a to ptr
  call void @llvm.lifetime.start.p0(i64 1, ptr %b)
  call void @llvm.lifetime.end.p0(i64 1, ptr %b)
  ret void
}

define void @strip_addrspacecast() {
; CHECK-LABEL: define void @strip_addrspacecast() {
; CHECK-NEXT:    [[A:%.*]] = alloca i8, align 1
; CHECK-NEXT:    [[B:%.*]] = addrspacecast ptr [[A]] to ptr addrspace(1)
; CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 1, ptr [[A]])
; CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 1, ptr [[A]])
; CHECK-NEXT:    ret void
;
  %a = alloca i8
  %b = addrspacecast ptr %a to ptr addrspace(1)
  call void @llvm.lifetime.start.p1(i64 1, ptr addrspace(1) %b)
  call void @llvm.lifetime.end.p1(i64 1, ptr addrspace(1) %b)
  ret void
}

define void @strip_gep() {
; CHECK-LABEL: define void @strip_gep() {
; CHECK-NEXT:    [[A:%.*]] = alloca [2 x i8], align 1
; CHECK-NEXT:    [[B:%.*]] = getelementptr [2 x i8], ptr [[A]], i64 0, i64 0
; CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 1, ptr [[A]])
; CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 1, ptr [[A]])
; CHECK-NEXT:    ret void
;
  %a = alloca [2 x i8]
  %b = getelementptr [2 x i8], ptr %a, i64 0, i64 0
  call void @llvm.lifetime.start.p0(i64 1, ptr %b)
  call void @llvm.lifetime.end.p0(i64 1, ptr %b)
  ret void
}

define void @remove_unanalyzable(ptr %p) {
; CHECK-LABEL: define void @remove_unanalyzable(
; CHECK-SAME: ptr [[P:%.*]]) {
; CHECK-NEXT:    ret void
;
  call void @llvm.lifetime.start.p0(i64 1, ptr %p)
  call void @llvm.lifetime.end.p0(i64 1, ptr %p)
  ret void
}
