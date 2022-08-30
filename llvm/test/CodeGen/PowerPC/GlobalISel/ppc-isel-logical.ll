; RUN: llc -mtriple ppc64le-linux -global-isel -o - < %s | FileCheck %s -check-prefixes=CHECK,LINUX

; CHECK-LABEL: test_andi64:
; LINUX: and 3, 3, 4
; LINUX: blr
define i64 @test_andi64(i64 %a, i64 %b) {
  %res = and i64 %a, %b
  ret i64 %res
}

; CHECK-LABEL: test_nandi64:
; LINUX: nand 3, 3, 4
; LINUX: blr
define i64 @test_nandi64(i64 %a, i64 %b) {
  %and = and i64 %a, %b
  %neg = xor i64 %and, -1
  ret i64 %neg
}

; CHECK-LABEL: test_andci64:
; LINUX: andc 3, 3, 4
; LINUX: blr
define i64 @test_andci64(i64 %a, i64 %b) {
  %neg = xor i64 %b, -1
  %and = and i64 %a, %neg
  ret i64 %and
}

; CHECK-LABEL: test_ori64:
; LINUX: or 3, 3, 4
; LINUX: blr
define i64 @test_ori64(i64 %a, i64 %b) {
  %res = or i64 %a, %b
  ret i64 %res
}

; CHECK-LABEL: test_orci64:
; LINUX: orc 3, 3, 4
; LINUX: blr
define i64 @test_orci64(i64 %a, i64 %b) {
  %neg = xor i64 %b, -1
  %or = or i64 %a, %neg
  ret i64 %or
}

; CHECK-LABEL: test_nori64:
; LINUX: nor 3, 3, 4
; LINUX: blr
define i64 @test_nori64(i64 %a, i64 %b) {
  %or = or i64 %a, %b
  %neg = xor i64 %or, -1
  ret i64 %neg
}

; CHECK-LABEL: test_xori64:
; LINUX: xor 3, 3, 4
; LINUX: blr
define i64 @test_xori64(i64 %a, i64 %b) {
  %res = xor i64 %a, %b
  ret i64 %res
}
