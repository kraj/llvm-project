; RUN: llc -O0 -mtriple=spirv64-unknown-unknown %s -o - | FileCheck %s
; RUN: %if spirv-tools %{ llc -O0 -mtriple=spirv64-unknown-unknown %s -o - -filetype=obj | spirv-val %}

@_ZN3barC1Ev = alias void (), ptr addrspace(4) @_ZN3barC2Ev

define spir_func void @_ZN3barC2Ev() addrspace(4) {
; CHECK:         %6 = OpFunction %4 None %5 ; -- Begin function _ZN3barC2Ev
; CHECK-NEXT:    %2 = OpLabel
; CHECK-NEXT:    OpReturn
; CHECK-NEXT:    OpFunctionEnd
entry:
  ret void
}

define spir_kernel void @_Z11test_kernelPi() addrspace(4) {
entry:
; CHECK: %7 = OpFunction %4 None %5              ; -- Begin function _Z11test_kernelPi
; CHECK-NEXT: %3 = OpLabel
; CHECK-NEXT: %8 = OpFunctionCall %4 %6
  call spir_func addrspace(4) void @_ZN3barC1Ev()
  ret void
}
