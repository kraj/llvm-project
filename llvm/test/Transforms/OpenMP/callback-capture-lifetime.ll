; RUN: opt -passes=attributor -S < %s | FileCheck %s --check-prefix=ATTRIBUTOR
; RUN: opt -passes=openmp-opt -S < %s | FileCheck %s --check-prefix=OPENMP
; RUN: opt -passes='openmp-opt,function(loop-mssa(licm))' -S < %s | FileCheck %s --check-prefix=LICM
; RUN: opt -use-dereferenceable-at-point-semantics=false -passes='openmp-opt,function(loop-mssa(licm))' -S < %s | FileCheck %s --check-prefix=LICM

target triple = "aarch64-unknown-linux-gnu"

define void @run(ptr %values) {
entry:
  %values.addr = alloca ptr, align 8
  store ptr %values, ptr %values.addr, align 8
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 1,
      ptr @outlined, ptr %values.addr)
  ret void
}

; Full Attributor already derives the lifetime attributes for mapped callback
; arguments. OpenMPOpt should seed the same generic reasoning.
; ATTRIBUTOR-LABEL: define internal void @outlined(
; ATTRIBUTOR-SAME: {{.*}}ptr noalias nofree noundef nonnull readonly align 8 captures(none) dereferenceable(8) %values.capture) {
; OPENMP-LABEL: define internal void @outlined(
; OPENMP-SAME: {{.*}}ptr noalias readonly align 8 captures(none) dereferenceable(8) %values.capture) {

; Once noalias and readonly establish that the capture storage remains live,
; LICM can speculate the load out of the conditional loop even though an
; unknown call precedes it.
; LICM-LABEL: define internal void @outlined(
; LICM-SAME: {{.*}}ptr noalias readonly align 8 captures(none) dereferenceable(8) %values.capture) {
; LICM: call void @opaque()
; LICM-NEXT: [[VALUES:%.*]] = load ptr, ptr %values.capture, align 8
; LICM: loop:
; LICM-NOT: load ptr, ptr %values.capture
; LICM: if.then:
; LICM-NEXT: [[ELEMENT:%.*]] = getelementptr double, ptr [[VALUES]], i64 [[I:%.*]]

define internal void @outlined(ptr noalias %global_tid, ptr noalias %bound_tid,
                              ptr align 8 dereferenceable(8) %values.capture) {
entry:
  %threads = call i32 @omp_get_num_threads()
  %thread = call i32 @omp_get_thread_num()
  call void @opaque()
  br label %loop

loop:
  %i = phi i64 [ 0, %entry ], [ %next, %latch ]
  %enabled = icmp eq i64 %i, 7
  br i1 %enabled, label %if.then, label %latch

if.then:
  %values = load ptr, ptr %values.capture, align 8
  %element = getelementptr double, ptr %values, i64 %i
  %value = load double, ptr %element, align 8
  call void @use(double %value, i32 %threads, i32 %thread)
  br label %latch

latch:
  %next = add nuw nsw i64 %i, 1
  %done = icmp eq i64 %next, 64
  br i1 %done, label %exit, label %loop

exit:
  ret void
}

; Scalar and aggregate capture storage use the same pointer-argument proof.
; OPENMP-LABEL: define internal void @outlined.scalar(
; OPENMP-SAME: {{.*}}ptr noalias readonly align 8 captures(none) dereferenceable(8) %capture)
define void @run_scalar(i64 %value) {
  %slot = alloca i64, align 8
  store i64 %value, ptr %slot, align 8
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 1,
      ptr @outlined.scalar, ptr %slot)
  ret void
}

define internal void @outlined.scalar(ptr %gtid, ptr %btid,
                                      ptr align 8 dereferenceable(8) %capture) {
  %value = load i64, ptr %capture, align 8
  call void @use.i64(i64 %value)
  ret void
}

; OPENMP-LABEL: define internal void @outlined.aggregate(
; OPENMP-SAME: {{.*}}ptr noalias readonly align 8 captures(none) dereferenceable(16) %capture)
define void @run_aggregate(ptr %a, ptr %b) {
  %slot = alloca { ptr, ptr }, align 8
  store ptr %a, ptr %slot, align 8
  %second = getelementptr { ptr, ptr }, ptr %slot, i32 0, i32 1
  store ptr %b, ptr %second, align 8
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 1,
      ptr @outlined.aggregate, ptr %slot)
  ret void
}

define internal void @outlined.aggregate(
    ptr %gtid, ptr %btid, ptr align 8 dereferenceable(16) %capture) {
  %value = load { ptr, ptr }, ptr %capture, align 8
  %a = extractvalue { ptr, ptr } %value, 0
  call void @use.ptr(ptr %a)
  ret void
}

; Every callback and direct call site must support the inference.
; OPENMP-LABEL: define internal void @outlined.serialized(
; OPENMP-SAME: {{.*}}ptr noalias readonly align 8 captures(none) dereferenceable(8) %capture)
define void @run_serialized(ptr %values, i32 %condition) {
  %slot = alloca ptr, align 8
  store ptr %values, ptr %slot, align 8
  call void @__kmpc_fork_call_if(ptr null, i32 1, ptr @outlined.serialized,
                                 i32 %condition, ptr %slot)
  call void @outlined.serialized(ptr null, ptr null, ptr %slot)
  ret void
}

define internal void @outlined.serialized(
    ptr %gtid, ptr %btid, ptr align 8 dereferenceable(8) %capture) {
  %value = load ptr, ptr %capture, align 8
  call void @use.ptr(ptr %value)
  ret void
}

; OPENMP-LABEL: define internal void @outlined.multiple(
; OPENMP-SAME: {{.*}}ptr noalias readonly align 8 captures(none) dereferenceable(8) %a.capture, ptr noalias readonly align 8 captures(none) dereferenceable(8) %b.capture)
define void @run_multiple(ptr %a, ptr %b) {
  %a.slot = alloca ptr, align 8
  %b.slot = alloca ptr, align 8
  store ptr %a, ptr %a.slot, align 8
  store ptr %b, ptr %b.slot, align 8
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 2,
      ptr @outlined.multiple, ptr %a.slot, ptr %b.slot)
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 2,
      ptr @outlined.multiple, ptr %a.slot, ptr %b.slot)
  ret void
}

define internal void @outlined.multiple(
    ptr %gtid, ptr %btid,
    ptr align 8 dereferenceable(8) %a.capture,
    ptr align 8 dereferenceable(8) %b.capture) {
  %a = load ptr, ptr %a.capture, align 8
  %b = load ptr, ptr %b.capture, align 8
  call void @use.ptr(ptr %a)
  call void @use.ptr(ptr %b)
  ret void
}

; Passing the same storage for two arguments does not satisfy noalias.
; OPENMP-LABEL: define internal void @outlined.aliased(
; OPENMP-SAME: {{.*}}ptr writeonly align 8 captures(none) dereferenceable(8) %a,
; OPENMP-SAME: ptr readonly align 8 captures(none) dereferenceable(8) %b)
define void @run_aliased(i64 %value) {
  %slot = alloca i64, align 8
  store i64 %value, ptr %slot, align 8
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 2,
      ptr @outlined.aliased, ptr %slot, ptr %slot)
  ret void
}

define internal void @outlined.aliased(
    ptr %gtid, ptr %btid, ptr align 8 dereferenceable(8) %a,
    ptr align 8 dereferenceable(8) %b) {
  store i64 0, ptr %a, align 8
  call void @sync()
  %bv = load i64, ptr %b, align 8
  call void @use.i64(i64 %bv)
  ret void
}

; Capture storage that escaped before the broker call is not noalias.
; OPENMP-LABEL: define internal void @outlined.escaped(
; OPENMP-SAME: {{.*}}ptr readonly align 8 captures(none) dereferenceable(8) %capture)
define void @run_escaped(i64 %value) {
  %slot = alloca i64, align 8
  store i64 %value, ptr %slot, align 8
  call void @escape(ptr %slot)
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 1,
      ptr @outlined.escaped, ptr %slot)
  ret void
}

define internal void @outlined.escaped(
    ptr %gtid, ptr %btid, ptr align 8 dereferenceable(8) %capture) {
  %value = load i64, ptr %capture, align 8
  call void @use.i64(i64 %value)
  ret void
}

; Missing callback metadata means the argument is not seeded.
; OPENMP-LABEL: define internal void @outlined.unmapped(
; OPENMP-SAME: {{.*}}ptr align 8 dereferenceable(8) %capture)
define void @run_unmapped(i64 %value) {
  %slot = alloca i64, align 8
  store i64 %value, ptr %slot, align 8
  call void (ptr, i32, ptr, ...) @fork_without_metadata(ptr null, i32 1,
      ptr @outlined.unmapped, ptr %slot)
  ret void
}

define internal void @outlined.unmapped(
    ptr %gtid, ptr %btid, ptr align 8 dereferenceable(8) %capture) {
  %value = load i64, ptr %capture, align 8
  call void @use.i64(i64 %value)
  ret void
}

; An unknown use of the callback prevents all-call-site reasoning.
@callback_slot = global ptr null

; OPENMP-LABEL: define internal void @outlined.unknown(
; OPENMP-SAME: {{.*}}ptr align 8 dereferenceable(8) %capture)
define void @run_unknown(i64 %value) {
  %slot = alloca i64, align 8
  store i64 %value, ptr %slot, align 8
  store ptr @outlined.unknown, ptr @callback_slot
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 1,
      ptr @outlined.unknown, ptr %slot)
  ret void
}

define internal void @outlined.unknown(
    ptr %gtid, ptr %btid, ptr align 8 dereferenceable(8) %capture) {
  %value = load i64, ptr %capture, align 8
  call void @use.i64(i64 %value)
  ret void
}

; Externally visible callbacks do not have a complete set of call sites.
; OPENMP-LABEL: define void @outlined.external(
; OPENMP-SAME: {{.*}}ptr align 8 dereferenceable(8) %capture)
define void @outlined.external(
    ptr %gtid, ptr %btid, ptr align 8 dereferenceable(8) %capture) {
  %value = load i64, ptr %capture, align 8
  call void @use.i64(i64 %value)
  ret void
}

define void @run_external(i64 %value) {
  %slot = alloca i64, align 8
  store i64 %value, ptr %slot, align 8
  call void (ptr, i32, ptr, ...) @__kmpc_fork_call(ptr null, i32 1,
      ptr @outlined.external, ptr %slot)
  ret void
}

declare !callback !0 void @__kmpc_fork_call(ptr, i32, ptr, ...)
declare !callback !3 void @__kmpc_fork_call_if(ptr, i32, ptr, i32, ptr)
declare void @fork_without_metadata(ptr, i32, ptr, ...)
declare i32 @omp_get_num_threads()
declare i32 @omp_get_thread_num()
declare void @opaque()
declare void @sync()
declare void @use(double, i32, i32) memory(none)
declare void @use.ptr(ptr) memory(none)
declare void @use.i64(i64) memory(none)
declare void @escape(ptr)

!0 = !{!1}
!1 = !{i64 2, i64 -1, i64 -1, i1 true}
!llvm.module.flags = !{!2}
!2 = !{i32 7, !"openmp", i32 51}
!3 = !{!4}
!4 = !{i64 2, i64 -1, i64 -1, i64 4, i1 false}
