; RUN: not llvm-as %s -o /dev/null 2>&1 | FileCheck %s

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_kernel void @callee_kernel(ptr addrspace(1) %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_kernel void @caller_kernel(ptr addrspace(1) %out) {
entry:
  call void @callee_kernel(ptr addrspace(1) %out)
  ret void
}

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_vs void @callee_vs(ptr addrspace(1) inreg %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_vs void @caller_vs(ptr addrspace(1) inreg %out) {
entry:
  call void @callee_vs(ptr addrspace(1) %out)
  ret void
}

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_gs void @callee_gs(ptr addrspace(1) %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_gs void @caller_gs(ptr addrspace(1) %out) {
entry:
  call void @callee_gs(ptr addrspace(1) %out)
  ret void
}

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_ps void @callee_ps(ptr addrspace(1) %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_ps void @caller_ps(ptr addrspace(1) %out) {
entry:
  call void @callee_ps(ptr addrspace(1) %out)
  ret void
}

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_cs void @callee_cs(ptr addrspace(1) %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_cs void @caller_cs(ptr addrspace(1) %out) {
entry:
  call void @callee_cs(ptr addrspace(1) %out)
  ret void
}

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_es void @callee_es(ptr addrspace(1) %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_es void @caller_es(ptr addrspace(1) %out) {
entry:
  call void @callee_es(ptr addrspace(1) %out)
  ret void
}

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_hs void @callee_hs(ptr addrspace(1) %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_hs void @caller_hs(ptr addrspace(1) %out) {
entry:
  call void @callee_hs(ptr addrspace(1) %out)
  ret void
}

; CHECK: Call to amdgpu entry function is not allowed
define amdgpu_ls void @callee_ls(ptr addrspace(1) %out) {
entry:
  store volatile i32 0, ptr addrspace(1) %out
  ret void
}

define amdgpu_ls void @caller_ls(ptr addrspace(1) %out) {
entry:
  call void @callee_ls(ptr addrspace(1) %out)
  ret void
}
