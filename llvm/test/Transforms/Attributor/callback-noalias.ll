; RUN: opt -passes=attributor -S < %s | FileCheck %s
; RUN: opt -passes=attributor-cgscc -S < %s | FileCheck %s

; A callback argument number is not necessarily the corresponding broker
; operand number. Make sure the alias comparison skips the actual call-site
; operand, rather than an unrelated mapped argument.

; CHECK-LABEL: define internal void @callback(
; CHECK-SAME: ptr nofree noundef nonnull writeonly align 8 captures(none) dereferenceable(8) %a,
; CHECK-SAME: ptr nofree noundef nonnull readonly align 8 captures(none) dereferenceable(8) %b)

define void @caller(i64 %value) {
  %slot = alloca i64, align 8
  store i64 %value, ptr %slot, align 8
  call void @broker(ptr @callback, ptr %slot, ptr %slot)
  ret void
}

define internal void @callback(ptr align 8 dereferenceable(8) %a,
                               ptr align 8 dereferenceable(8) %b) {
  store i64 0, ptr %a, align 8
  call void @sync()
  %value = load i64, ptr %b, align 8
  call void @use(i64 %value)
  ret void
}

declare !callback !0 void @broker(ptr, ptr, ptr)
declare void @sync()
declare void @use(i64) memory(none)

!0 = !{!1}
!1 = !{i64 0, i64 1, i64 2, i1 false}
