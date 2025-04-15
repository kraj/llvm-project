; RUN: not llvm-as %s --disable-output 2>&1 | FileCheck %s

target triple = "amdgcn-amd-amdhsa"

; CHECK: alloca on amdgpu must be in addrspace(5)
; CHECK-NEXT: %alloca = alloca i32, align 4
; CHECK: alloca on amdgpu must be in addrspace(5)
; CHECK-NEXT: %alloca.0 = alloca i32, i32 4, align 4
define void @foo() {
entry:
  %alloca = alloca i32, align 4
  %alloca.0 = alloca i32, i32 4, align 4
  ret void
}
