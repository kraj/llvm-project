; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --check-globals none --version 5
; RUN: opt -p loop-vectorize -force-vector-width=2 -prefer-predicate-over-epilogue=predicate-dont-vectorize -S %s | FileCheck %s

define void @canonical_small_tc_i8(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_small_tc_i8(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i8> [ <i8 0, i8 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i8> [[VEC_IND]], splat (i8 14)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP24:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP24]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i8> [[VEC_IND]], splat (i8 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[INDEX_NEXT]], 16
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP0:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], 15
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP3:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 15
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_upper_limit_i8(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_upper_limit_i8(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i8> [ <i8 0, i8 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i8> [[VEC_IND]], splat (i8 -2)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP24:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP24]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i8> [[VEC_IND]], splat (i8 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[INDEX_NEXT]], 256
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP4:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], 255
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP5:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 255
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_lower_limit_i16(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_lower_limit_i16(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i16> [ <i16 0, i16 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i16> [[VEC_IND]], splat (i16 256)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP24:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP24]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i16> [[VEC_IND]], splat (i16 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[INDEX_NEXT]], 258
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP6:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], 257
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP7:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 257
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_upper_limit_i16(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_upper_limit_i16(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i16> [ <i16 0, i16 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i16> [[VEC_IND]], splat (i16 -2)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP24:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP24]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i16> [[VEC_IND]], splat (i16 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[INDEX_NEXT]], 65536
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP8:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], 65535
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP9:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 65535
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_lower_limit_i32(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_lower_limit_i32(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i32> [ <i32 0, i32 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i32> [[VEC_IND]], splat (i32 65536)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP24:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP24]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i32> [[VEC_IND]], splat (i32 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[INDEX_NEXT]], 65538
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP10:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], 65537
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP11:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 65537
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_upper_limit_i32(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_upper_limit_i32(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i32> [ <i32 0, i32 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i32> [[VEC_IND]], splat (i32 -2)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP24:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP24]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i32> [[VEC_IND]], splat (i32 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[INDEX_NEXT]], 4294967296
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP12:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], 4294967295
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP13:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 4294967295
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_lower_limit_i64(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_lower_limit_i64(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[TMP8:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i64> [ <i64 0, i64 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i64> [[VEC_IND]], splat (i64 4294967296)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP6:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP6]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[TMP8]] = add i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i64> [[VEC_IND]], splat (i64 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[TMP8]], 4294967298
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP14:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], 4294967297
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP15:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 4294967297
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_upper_limit_i64(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_upper_limit_i64(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i64 [ 0, %[[VECTOR_PH]] ], [ [[TMP8:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i64> [ <i64 0, i64 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i64> [[VEC_IND]], splat (i64 -2)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i64 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i64 [[INDEX]], 1
; CHECK-NEXT:    [[TMP6:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP6]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[TMP8]] = add i64 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i64> [[VEC_IND]], splat (i64 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i64 [[TMP8]], 0
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP16:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i64 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i64 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i64 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i64 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i64 [[IV_NEXT]], -1
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP17:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i64 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i64 %iv, 1
  %cond = icmp eq i64 %iv.next, 18446744073709551615
  br i1 %cond, label %end, label %loop

end:
  ret void
}

define void @canonical_lower_limit_i128(ptr nocapture noundef writeonly %p) {
; CHECK-LABEL: define void @canonical_lower_limit_i128(
; CHECK-SAME: ptr noundef writeonly captures(none) [[P:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i256 [ 0, %[[VECTOR_PH]] ], [ [[TMP8:%.*]], %[[PRED_STORE_CONTINUE2:.*]] ]
; CHECK-NEXT:    [[VEC_IND:%.*]] = phi <2 x i128> [ <i128 0, i128 1>, %[[VECTOR_PH]] ], [ [[VEC_IND_NEXT:%.*]], %[[PRED_STORE_CONTINUE2]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = icmp ule <2 x i128> [[VEC_IND]], splat (i128 18446744073709551616)
; CHECK-NEXT:    [[TMP1:%.*]] = extractelement <2 x i1> [[TMP0]], i32 0
; CHECK-NEXT:    br i1 [[TMP1]], label %[[PRED_STORE_IF:.*]], label %[[PRED_STORE_CONTINUE:.*]]
; CHECK:       [[PRED_STORE_IF]]:
; CHECK-NEXT:    [[TMP2:%.*]] = add i256 [[INDEX]], 0
; CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds i16, ptr [[P]], i256 [[TMP2]]
; CHECK-NEXT:    store i16 1, ptr [[TMP3]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE]]
; CHECK:       [[PRED_STORE_CONTINUE]]:
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <2 x i1> [[TMP0]], i32 1
; CHECK-NEXT:    br i1 [[TMP4]], label %[[PRED_STORE_IF1:.*]], label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_IF1]]:
; CHECK-NEXT:    [[TMP5:%.*]] = add i256 [[INDEX]], 1
; CHECK-NEXT:    [[TMP6:%.*]] = getelementptr inbounds i16, ptr [[P]], i256 [[TMP5]]
; CHECK-NEXT:    store i16 1, ptr [[TMP6]], align 2
; CHECK-NEXT:    br label %[[PRED_STORE_CONTINUE2]]
; CHECK:       [[PRED_STORE_CONTINUE2]]:
; CHECK-NEXT:    [[TMP8]] = add i256 [[INDEX]], 2
; CHECK-NEXT:    [[VEC_IND_NEXT]] = add <2 x i128> [[VEC_IND]], splat (i128 2)
; CHECK-NEXT:    [[TMP7:%.*]] = icmp eq i256 [[TMP8]], 18446744073709551618
; CHECK-NEXT:    br i1 [[TMP7]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP18:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br label %[[END:.*]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i256 [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[IV:%.*]] = phi i256 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[IV_NEXT:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    [[P_IV:%.*]] = getelementptr inbounds i16, ptr [[P]], i256 [[IV]]
; CHECK-NEXT:    store i16 1, ptr [[P_IV]], align 2
; CHECK-NEXT:    [[IV_NEXT]] = add nuw nsw i256 [[IV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i256 [[IV_NEXT]], 18446744073709551617
; CHECK-NEXT:    br i1 [[COND]], label %[[END]], label %[[LOOP]], !llvm.loop [[LOOP19:![0-9]+]]
; CHECK:       [[END]]:
; CHECK-NEXT:    ret void
;
entry:
  br label %loop

loop:
  %iv = phi i256 [ 0, %entry ], [ %iv.next, %loop ]
  %p.iv = getelementptr inbounds i16, ptr %p, i256 %iv
  store i16 1, ptr %p.iv, align 2
  %iv.next = add nuw nsw i256 %iv, 1
  %cond = icmp eq i256 %iv.next, 18446744073709551617
  br i1 %cond, label %end, label %loop

end:
  ret void
}

