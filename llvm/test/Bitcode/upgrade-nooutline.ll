; RUN: llvm-as < %s | llvm-dis - | FileCheck %s --implicit-check-not=\"nooutline\"

; CHECK: define void @f() [[ATTR:#[0-9]+]]
; CHECK: attributes [[ATTR]] = { nooutline }

define void @f() "nooutline" {
  ret void
}
