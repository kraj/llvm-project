; RUN: mlir-translate -import-llvm %s | FileCheck %s

; CHECK-LABEL: llvm.func @repeated_type_metadata
; CHECK-SAME: function_metadata
; CHECK-SAME: #llvm.func_metadata<"type", <#llvm.md_const<0 : i64>, #llvm.md_string<"typeid0">>>
; CHECK-SAME: #llvm.func_metadata<"type", <#llvm.md_const<0 : i64>, #llvm.md_string<"typeid1">>>
define void @repeated_type_metadata() !type !0 !type !1 {
  ret void
}

; CHECK-LABEL: llvm.func @function_ref_metadata
; CHECK-SAME: function_metadata
; CHECK-SAME: #llvm.func_metadata<"callees", <#llvm.md_func<@callee>>>
define void @function_ref_metadata() !callees !2 {
  ret void
}

; CHECK-LABEL: llvm.func @alias_ref_metadata
; CHECK-SAME: function_metadata
; CHECK-SAME: #llvm.func_metadata<"callees", <#llvm.md_func<@alias>>>
define void @alias_ref_metadata() !callees !3 {
  ret void
}

declare void @callee()
define void @alias_target() {
  ret void
}
@alias = alias void (), ptr @alias_target

!0 = !{i64 0, !"typeid0"}
!1 = !{i64 0, !"typeid1"}
!2 = !{ptr @callee}
!3 = !{ptr @alias}
