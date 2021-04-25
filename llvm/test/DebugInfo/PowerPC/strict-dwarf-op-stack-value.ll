; RUN: llc -filetype=obj -mtriple=powerpc64le-unknown-linux-gnu < %s | \
; RUN:   llvm-dwarfdump -debug-info - | FileCheck %s
; RUN: llc -filetype=obj -mtriple=powerpc64le-unknown-linux-gnu \
; RUN:   -strict-dwarf=true < %s | llvm-dwarfdump -debug-info - | \
; RUN:   FileCheck %s -check-prefix=STRICT

; check that DWARF 4 location expression DW_OP_stack_value is not emitted in
; DWARF 3 at strict dwarf mode.

; CHECK: DW_AT_location (DW_OP_addr 0x0, DW_OP_stack_value)
; STRICT-NOT: DW_OP_stack_value

; $ cat 1.cpp
; extern bool cmp();
;
; typedef bool comparator_function();
; template<class Iterator, comparator_function compare>
; void quicksort(Iterator begin, Iterator end)
; {
; }
; void foo(void)
; {
;   double a[100];
;   quicksort<double *, cmp>(a, a + 100);
; }


$_Z9quicksortIPdXadL_Z3cmpvEEEvT_S1_ = comdat any

; Function Attrs: noinline optnone uwtable mustprogress
define dso_local void @_Z3foov() #0 !dbg !8 {
entry:
  %a = alloca [100 x double], align 8
  call void @llvm.dbg.declare(metadata [100 x double]* %a, metadata !11, metadata !DIExpression()), !dbg !16
  %arraydecay = getelementptr inbounds [100 x double], [100 x double]* %a, i64 0, i64 0, !dbg !17
  %arraydecay1 = getelementptr inbounds [100 x double], [100 x double]* %a, i64 0, i64 0, !dbg !18
  %add.ptr = getelementptr inbounds double, double* %arraydecay1, i64 100, !dbg !19
  call void @_Z9quicksortIPdXadL_Z3cmpvEEEvT_S1_(double* %arraydecay, double* %add.ptr), !dbg !20
  ret void, !dbg !21
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable mustprogress
define linkonce_odr dso_local void @_Z9quicksortIPdXadL_Z3cmpvEEEvT_S1_(double* %begin, double* %end) #2 comdat !dbg !22 {
entry:
  %begin.addr = alloca double*, align 8
  %end.addr = alloca double*, align 8
  store double* %begin, double** %begin.addr, align 8
  call void @llvm.dbg.declare(metadata double** %begin.addr, metadata !33, metadata !DIExpression()), !dbg !34
  store double* %end, double** %end.addr, align 8
  call void @llvm.dbg.declare(metadata double** %end.addr, metadata !35, metadata !DIExpression()), !dbg !36
  ret void, !dbg !37
}

declare zeroext i1 @_Z3cmpv() #3

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 13.0.0", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "1.cpp", directory: "./")
!2 = !{}
!3 = !{i32 7, !"Dwarf Version", i32 3}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{!"clang version 13.0.0"}
!8 = distinct !DISubprogram(name: "foo", linkageName: "_Z3foov", scope: !1, file: !1, line: 9, type: !9, scopeLine: 10, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null}
!11 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 11, type: !12)
!12 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, size: 6400, elements: !14)
!13 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!14 = !{!15}
!15 = !DISubrange(count: 100)
!16 = !DILocation(line: 11, column: 10, scope: !8)
!17 = !DILocation(line: 12, column: 28, scope: !8)
!18 = !DILocation(line: 12, column: 31, scope: !8)
!19 = !DILocation(line: 12, column: 33, scope: !8)
!20 = !DILocation(line: 12, column: 3, scope: !8)
!21 = !DILocation(line: 13, column: 1, scope: !8)
!22 = distinct !DISubprogram(name: "quicksort<double *, &cmp>", linkageName: "_Z9quicksortIPdXadL_Z3cmpvEEEvT_S1_", scope: !1, file: !1, line: 6, type: !23, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, templateParams: !26, retainedNodes: !2)
!23 = !DISubroutineType(types: !24)
!24 = !{null, !25, !25}
!25 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!26 = !{!27, !28}
!27 = !DITemplateTypeParameter(name: "Iterator", type: !25)
!28 = !DITemplateValueParameter(name: "compare", type: !29, value: i1 ()* @_Z3cmpv)
!29 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !30, size: 64)
!30 = !DISubroutineType(types: !31)
!31 = !{!32}
!32 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!33 = !DILocalVariable(name: "begin", arg: 1, scope: !22, file: !1, line: 6, type: !25)
!34 = !DILocation(line: 6, column: 25, scope: !22)
!35 = !DILocalVariable(name: "end", arg: 2, scope: !22, file: !1, line: 6, type: !25)
!36 = !DILocation(line: 6, column: 41, scope: !22)
!37 = !DILocation(line: 8, column: 1, scope: !22)
