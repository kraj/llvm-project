; RUN: llc %s -o - | FileCheck %s

; Tail calls which have stack arguments in the same offsets as the caller do not
; need to load and store the arguments from the stack.

target triple = "aarch64-none-linux-gnu"

declare i32 @func(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j)

; CHECK-LABEL: wrapper_func:
define i32 @wrapper_func(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j) {
  ; CHECK: // %bb.
  ; CHECK-NEXT: b func
  %call = tail call i32 @func(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j)
  ret i32 %call
}

; CHECK-LABEL: wrapper_func_zero_arg:
define i32 @wrapper_func_zero_arg(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j) {
  ; CHECK: // %bb.
  ; CHECK-NEXT: mov	w0, wzr
  ; CHECK-NEXT: b func
  %call = tail call i32 @func(i32 0, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j)
  ret i32 %call
}

; CHECK-LABEL: wrapper_func_zero_stack_arg:
define i32 @wrapper_func_zero_stack_arg(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j) {
  ; CHECK: // %bb.
  ; CHECK-NEXT: str wzr, [sp, #8]
  ; CHECK-NEXT: b func
  %call = tail call i32 @func(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 0)
  ret i32 %call
}

; CHECK-LABEL: wrapper_func_overriden_arg:
define i32 @wrapper_func_overriden_arg(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j) {
  ; CHECK: // %bb.
  ; CHECK-NEXT: mov w1, w0
  ; CHECK-NEXT: mov w0, wzr
  ; CHECK-NEXT: b func
  %call = tail call i32 @func(i32 0, i32 %a, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j)
  ret i32 %call
}

; CHECK-LABEL: wrapper_func_overriden_stack_arg:
define i32 @wrapper_func_overriden_stack_arg(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 %i, i32 %j) {
  ; CHECK: // %bb.
  ; CHECK-NEXT: ldr w8, [sp]
  ; CHECK-NEXT: str wzr, [sp]
  ; CHECK-NEXT: str w8, [sp, #8]
  ; CHECK-NEXT: b func
  %call = tail call i32 @func(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f, i32 %g, i32 %h, i32 0, i32 %i)
  ret i32 %call
}
