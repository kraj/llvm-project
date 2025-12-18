; RUN: not llc -O0 -mtriple=spirv64-unknown-unknown < %s 

; Interposable aliases are not yet supported.
@bar_alias = weak alias void (), ptr addrspace(4) @bar

define spir_func void @bar() addrspace(4) {
entry:
  ret void
}

define spir_kernel void @kernel() addrspace(4) {
entry:
  call spir_func addrspace(4) void @bar_alias()
  ret void
}
