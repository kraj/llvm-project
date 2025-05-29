; Note: Make sure that instrumention intrinsic is after entry alloca.
; RUN: opt < %s -passes=pgo-instr-gen -S | FileCheck %s 
; RUN: opt < %s -passes=pgo-instr-gen,instrprof -sampled-instrumentation -S | FileCheck %s --check-prefixes=SAMPLE

%struct.A = type { i32, [0 x i32] }

; CHECK-LABEL: @foo()
; CHECK-NEXT:   %1 = alloca %struct.A
; CHECK-NEXT:   call void @llvm.instrprof.increment(ptr @__profn_foo
; CHECK-NEXT:   call void @bar(ptr

; SAMPLE: @foo()
; SAMPLE-NEXT:  %1 = alloca %struct.A, align 4
; SAMPLE-NEXT:  %[[v:[0-9]+]] = load i16, ptr @__llvm_profile_sampling
; SAMPLE-NEXT:  {{.*}} = icmp ule i16 %[[v]], 199

define dso_local i32 @foo() {
  %1 = alloca %struct.A, align 4
  call void @bar(ptr noundef nonnull %1) #3
  %2 = load i32, ptr %1, align 4
  %3 = icmp sgt i32 %2, 0
  br i1 %3, label %4, label %15

4:
  %5 = getelementptr inbounds i8, ptr %1, i64 4
  %6 = zext nneg i32 %2 to i64
  br label %7

7:
  %8 = phi i64 [ 0, %4 ], [ %13, %7 ]
  %9 = phi i32 [ 0, %4 ], [ %12, %7 ]
  %10 = getelementptr inbounds [0 x i32], ptr %5, i64 0, i64 %8
  %11 = load i32, ptr %10, align 4
  %12 = add nsw i32 %11, %9
  %13 = add nuw nsw i64 %8, 1
  %14 = icmp eq i64 %13, %6
  br i1 %14, label %15, label %7

15:
  %16 = phi i32 [ 0, %0 ], [ %12, %7 ]
  ret i32 %16
}

declare void @bar(ptr noundef)
