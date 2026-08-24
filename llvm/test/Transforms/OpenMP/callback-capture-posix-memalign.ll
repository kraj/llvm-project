; RUN: opt -passes='inferattrs,openmp-opt,function(loop-mssa(licm))' -S < %s | FileCheck %s

target triple = "aarch64-unknown-linux-gnu"

; posix_memalign writes through its output slot but does not capture the slot.
; That earlier use must not prevent the callback capture from becoming noalias
; or the load from being hoisted across an unknown call.
; CHECK: call i32 @posix_memalign(ptr captures(none) %slot, i64 64, i64 %size)
; CHECK-LABEL: define internal void @outlined(
; CHECK-SAME: ptr noalias readonly align 8 captures(none) dereferenceable(8) %capture)
; CHECK: call void @opaque()
; CHECK-NEXT: [[VALUES:%.*]] = load ptr, ptr %capture, align 8
; CHECK: loop:
; CHECK-NOT: load ptr, ptr %capture
; CHECK: if.then:
; CHECK-NEXT: [[ELEMENT:%.*]] = getelementptr double, ptr [[VALUES]], i64 [[I:%.*]]

define i32 @run(i64 %size) {
entry:
  %slot = alloca ptr, align 8
  %status = call i32 @posix_memalign(ptr %slot, i64 64, i64 %size)
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(
      ptr null, i32 1, ptr @outlined, ptr %slot)
  ret i32 %status
}

define internal void @outlined(
    ptr %global_tid, ptr %bound_tid,
    ptr align 8 dereferenceable(8) %capture) {
entry:
  call void @opaque()
  br label %loop

loop:
  %i = phi i64 [ 0, %entry ], [ %next, %latch ]
  %enabled = icmp eq i64 %i, 7
  br i1 %enabled, label %if.then, label %latch

if.then:
  %values = load ptr, ptr %capture, align 8
  %element = getelementptr double, ptr %values, i64 %i
  %value = load double, ptr %element, align 8
  call void @use(double %value)
  br label %latch

latch:
  %next = add nuw nsw i64 %i, 1
  %done = icmp eq i64 %next, 64
  br i1 %done, label %exit, label %loop

exit:
  ret void
}

declare i32 @posix_memalign(ptr, i64, i64)
declare !callback !0 void @__kmpc_fork_call(ptr, i32, ptr, ...)
declare void @opaque()
declare void @use(double) memory(none)

!0 = !{!1}
!1 = !{i64 2, i64 -1, i64 -1, i1 true}
!llvm.module.flags = !{!2}
!2 = !{i32 7, !"openmp", i32 51}
