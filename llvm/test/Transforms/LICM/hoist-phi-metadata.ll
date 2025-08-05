; Test that hoising conditional branches copies over profiling metadata
; RUN: opt -S -passes=licm -licm-control-flow-hoisting=1 %s -o - | FileCheck %s

; CHECK-LABEL: @triangle_phi
define void @triangle_phi(i32 %x, ptr %p) {
; CHECK-LABEL: entry:
; CHECK: %cmp1 = icmp sgt i32 %x, 0
; CHECK: br i1 %cmp1, label %[[IF_LICM:.*]], label %[[THEN_LICM:.*]], !prof !0
entry:
  br label %loop

; CHECK: [[IF_LICM]]:
; CHECK: %add = add i32 %x, 1
; CHECK: br label %[[THEN_LICM]]

; CHECK: [[THEN_LICM]]:
; CHECK: phi i32 [ %add, %[[IF_LICM]] ], [ %x, %entry ]
; CHECK: store i32 %phi, ptr %p
; CHECK: %cmp2 = icmp ne i32 %phi, 0
; CHECK: br label %loop

loop:
  %cmp1 = icmp sgt i32 %x, 0
  br i1 %cmp1, label %if, label %then, !prof !0

if:
  %add = add i32 %x, 1
  br label %then

; CHECK-LABEL: then:
; CHECK: br i1 %cmp2, label %loop, label %end, !prof !1
then:
  %phi = phi i32 [ %add, %if ], [ %x, %loop ]
  store i32 %phi, ptr %p
  %cmp2 = icmp ne i32 %phi, 0
  br i1 %cmp2, label %loop, label %end, !prof !1

; CHECK-LABEL: end:
end:
  ret void
}

; CHECK: !0 = !{!"branch_weights", i32 5, i32 7}
; CHECK: !1 = !{!"branch_weights", i32 13, i32 11}
!0 = !{!"branch_weights", i32 5, i32 7}
!1 = !{!"branch_weights", i32 13, i32 11}
