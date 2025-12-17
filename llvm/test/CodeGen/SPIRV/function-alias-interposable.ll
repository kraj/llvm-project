; RUN: not llc -O0 -mtriple=spirv64-unknown-unknown %s -o - 

; Interposable aliases are not yet supported.
@_ZN3barC1Ev = weak alias void (), ptr addrspace(4) @_ZN3barC2Ev

define spir_func void @_ZN3barC2Ev() addrspace(4) {
entry:
  ret void
}

define spir_kernel void @_Z11test_kernelPi() addrspace(4) {
entry:
  call spir_func addrspace(4) void @_ZN3barC1Ev()
  ret void
}
