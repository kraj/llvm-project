; RUN: split-file %s %t
; RUN: llvm-as < %t/ppc_fp128.ll | llvm-dis | FileCheck %s --check-prefix=PPCFP128
; RUN: llvm-as < %t/fp128.ll | llvm-dis | FileCheck %s --check-prefix=FP128
; RUN: llvm-as < %t/x86_fp80.ll | llvm-dis | FileCheck %s --check-prefix=X86FP80
; RUN: llvm-as < %t/double.ll | llvm-dis | FileCheck %s --check-prefix=DOUBLE

;--- ppc_fp128.ll
!llvm.module.flags = !{!0}
!0 = !{i32 1, !"long-double-type", !"ppc_fp128"}
; PPCFP128: !0 = !{i32 1, !"long-double-type", !"ppc_fp128"}

;--- fp128.ll
!llvm.module.flags = !{!0}
!0 = !{i32 1, !"long-double-type", !"fp128"}
; FP128: !0 = !{i32 1, !"long-double-type", !"fp128"}

;--- x86_fp80.ll
!llvm.module.flags = !{!0}
!0 = !{i32 1, !"long-double-type", !"x86_fp80"}
; X86FP80: !0 = !{i32 1, !"long-double-type", !"x86_fp80"}

;--- double.ll
!llvm.module.flags = !{!0}
!0 = !{i32 1, !"long-double-type", !"double"}
; DOUBLE: !0 = !{i32 1, !"long-double-type", !"double"}
