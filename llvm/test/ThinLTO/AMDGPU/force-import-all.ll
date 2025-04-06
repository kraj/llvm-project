; RUN: opt -mtriple=amdgcn-amd-amdhsa -module-summary %s -o %t1.bc
; RUN: opt -mtriple=amdgcn-amd-amdhsa -module-summary %p/Inputs/noinline.ll -o %t2.bc
; RUN: llvm-lto -thinlto-action=thinlink -force-import-all %t1.bc %t2.bc -o %t3.index.bc
; RUN: llvm-lto -thinlto-action=import -force-import-all %t1.bc %t2.bc --thinlto-index=%t3.index.bc -thinlto-save-temps=%t3.
; RUN: llvm-dis %t3.0.3.imported.bc -o - | FileCheck %s --check-prefix=MOD1
; RUN: llvm-dis %t3.1.3.imported.bc -o - | FileCheck %s --check-prefix=MOD2

define i32 @f1(ptr %p) #0 {
entry:
  call void @f2(ptr %p)
  ret i32 0
}

define weak hidden void @f3(ptr %v) #0 {
entry:
  store i32 12345, ptr %v
  ret void
}

declare void @f2(ptr)

attributes #0 = { noinline }

; MOD1: define weak hidden void @f3
; MOD1: define available_externally void @f2
; MOD2: define void @f2
; MOD2: define available_externally hidden void @f3
