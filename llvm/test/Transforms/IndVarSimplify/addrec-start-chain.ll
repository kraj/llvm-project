; RUN: opt -passes=indvars -disable-output %s

; Check that proving predicates on AddRecs via their start values avoids
; exponential work.
;
; This is a chain of loops, where the IV of each loop starts at the exit value
; of the IV of the previous loop. So the start value of each IV is an AddRec of
; the previous loop. isKnownPredicateViaAddRecStart used to prove a predicate on
; two AddRecs by calling isKnownPredicate on their start values. That query
; recursed into every previous loop of the chain, and each level tried several
; proofs, so the number of queries grew exponentially with the chain length.
;
; This is a compile-time regression test: indvars should complete quickly.
; No particular transformed IR is required.

target datalayout = "e-m:e-i64:64-n32:64-S128"

define void @chain(ptr %arg, ptr %g) {
entry:
  br label %pre0
pre0:
  br label %loop0
loop0:
  %p0 = phi i64 [ 0, %pre0 ], [ %add0, %loop0 ]
  %q0 = phi i32 [ 0, %pre0 ], [ %addq0, %loop0 ]
  %add0 = add nuw nsw i64 %p0, 1
  %gep0 = getelementptr inbounds nuw i8, ptr %arg, i64 %p0
  store i8 0, ptr %gep0, align 1
  %addq0 = add nuw nsw i32 %q0, 1
  %ld0 = load i32, ptr %g, align 4
  %sh0 = lshr i32 %addq0, %ld0
  %c1_0 = icmp ne i32 %sh0, 0
  %c2_0 = icmp ugt i64 %p0, 17407
  %or0 = or i1 %c2_0, %c1_0
  br i1 %or0, label %exit0, label %loop0
exit0:
  %e0 = phi i64 [ %add0, %loop0 ]
  %ec0 = phi i1 [ %c2_0, %loop0 ]
  br i1 %ec0, label %end, label %pre1
pre1:
  br label %loop1
loop1:
  %p1 = phi i64 [ %e0, %pre1 ], [ %add1, %loop1 ]
  %q1 = phi i32 [ 0, %pre1 ], [ %addq1, %loop1 ]
  %add1 = add nuw nsw i64 %p1, 1
  %gep1 = getelementptr inbounds nuw i8, ptr %arg, i64 %p1
  store i8 1, ptr %gep1, align 1
  %addq1 = add nuw nsw i32 %q1, 1
  %ld1 = load i32, ptr %g, align 4
  %sh1 = lshr i32 %addq1, %ld1
  %c1_1 = icmp ne i32 %sh1, 0
  %c2_1 = icmp ugt i64 %p1, 17407
  %or1 = or i1 %c2_1, %c1_1
  br i1 %or1, label %exit1, label %loop1
exit1:
  %e1 = phi i64 [ %add1, %loop1 ]
  %ec1 = phi i1 [ %c2_1, %loop1 ]
  br i1 %ec1, label %end, label %pre2
pre2:
  br label %loop2
loop2:
  %p2 = phi i64 [ %e1, %pre2 ], [ %add2, %loop2 ]
  %q2 = phi i32 [ 0, %pre2 ], [ %addq2, %loop2 ]
  %add2 = add nuw nsw i64 %p2, 1
  %gep2 = getelementptr inbounds nuw i8, ptr %arg, i64 %p2
  store i8 2, ptr %gep2, align 1
  %addq2 = add nuw nsw i32 %q2, 1
  %ld2 = load i32, ptr %g, align 4
  %sh2 = lshr i32 %addq2, %ld2
  %c1_2 = icmp ne i32 %sh2, 0
  %c2_2 = icmp ugt i64 %p2, 17407
  %or2 = or i1 %c2_2, %c1_2
  br i1 %or2, label %exit2, label %loop2
exit2:
  %e2 = phi i64 [ %add2, %loop2 ]
  %ec2 = phi i1 [ %c2_2, %loop2 ]
  br i1 %ec2, label %end, label %pre3
pre3:
  br label %loop3
loop3:
  %p3 = phi i64 [ %e2, %pre3 ], [ %add3, %loop3 ]
  %q3 = phi i32 [ 0, %pre3 ], [ %addq3, %loop3 ]
  %add3 = add nuw nsw i64 %p3, 1
  %gep3 = getelementptr inbounds nuw i8, ptr %arg, i64 %p3
  store i8 3, ptr %gep3, align 1
  %addq3 = add nuw nsw i32 %q3, 1
  %ld3 = load i32, ptr %g, align 4
  %sh3 = lshr i32 %addq3, %ld3
  %c1_3 = icmp ne i32 %sh3, 0
  %c2_3 = icmp ugt i64 %p3, 17407
  %or3 = or i1 %c2_3, %c1_3
  br i1 %or3, label %exit3, label %loop3
exit3:
  %e3 = phi i64 [ %add3, %loop3 ]
  %ec3 = phi i1 [ %c2_3, %loop3 ]
  br i1 %ec3, label %end, label %pre4
pre4:
  br label %loop4
loop4:
  %p4 = phi i64 [ %e3, %pre4 ], [ %add4, %loop4 ]
  %q4 = phi i32 [ 0, %pre4 ], [ %addq4, %loop4 ]
  %add4 = add nuw nsw i64 %p4, 1
  %gep4 = getelementptr inbounds nuw i8, ptr %arg, i64 %p4
  store i8 4, ptr %gep4, align 1
  %addq4 = add nuw nsw i32 %q4, 1
  %ld4 = load i32, ptr %g, align 4
  %sh4 = lshr i32 %addq4, %ld4
  %c1_4 = icmp ne i32 %sh4, 0
  %c2_4 = icmp ugt i64 %p4, 17407
  %or4 = or i1 %c2_4, %c1_4
  br i1 %or4, label %exit4, label %loop4
exit4:
  %e4 = phi i64 [ %add4, %loop4 ]
  %ec4 = phi i1 [ %c2_4, %loop4 ]
  br i1 %ec4, label %end, label %pre5
pre5:
  br label %loop5
loop5:
  %p5 = phi i64 [ %e4, %pre5 ], [ %add5, %loop5 ]
  %q5 = phi i32 [ 0, %pre5 ], [ %addq5, %loop5 ]
  %add5 = add nuw nsw i64 %p5, 1
  %gep5 = getelementptr inbounds nuw i8, ptr %arg, i64 %p5
  store i8 5, ptr %gep5, align 1
  %addq5 = add nuw nsw i32 %q5, 1
  %ld5 = load i32, ptr %g, align 4
  %sh5 = lshr i32 %addq5, %ld5
  %c1_5 = icmp ne i32 %sh5, 0
  %c2_5 = icmp ugt i64 %p5, 17407
  %or5 = or i1 %c2_5, %c1_5
  br i1 %or5, label %exit5, label %loop5
exit5:
  %e5 = phi i64 [ %add5, %loop5 ]
  %ec5 = phi i1 [ %c2_5, %loop5 ]
  br i1 %ec5, label %end, label %pre6
pre6:
  br label %loop6
loop6:
  %p6 = phi i64 [ %e5, %pre6 ], [ %add6, %loop6 ]
  %q6 = phi i32 [ 0, %pre6 ], [ %addq6, %loop6 ]
  %add6 = add nuw nsw i64 %p6, 1
  %gep6 = getelementptr inbounds nuw i8, ptr %arg, i64 %p6
  store i8 6, ptr %gep6, align 1
  %addq6 = add nuw nsw i32 %q6, 1
  %ld6 = load i32, ptr %g, align 4
  %sh6 = lshr i32 %addq6, %ld6
  %c1_6 = icmp ne i32 %sh6, 0
  %c2_6 = icmp ugt i64 %p6, 17407
  %or6 = or i1 %c2_6, %c1_6
  br i1 %or6, label %exit6, label %loop6
exit6:
  %e6 = phi i64 [ %add6, %loop6 ]
  %ec6 = phi i1 [ %c2_6, %loop6 ]
  br i1 %ec6, label %end, label %pre7
pre7:
  br label %loop7
loop7:
  %p7 = phi i64 [ %e6, %pre7 ], [ %add7, %loop7 ]
  %q7 = phi i32 [ 0, %pre7 ], [ %addq7, %loop7 ]
  %add7 = add nuw nsw i64 %p7, 1
  %gep7 = getelementptr inbounds nuw i8, ptr %arg, i64 %p7
  store i8 7, ptr %gep7, align 1
  %addq7 = add nuw nsw i32 %q7, 1
  %ld7 = load i32, ptr %g, align 4
  %sh7 = lshr i32 %addq7, %ld7
  %c1_7 = icmp ne i32 %sh7, 0
  %c2_7 = icmp ugt i64 %p7, 17407
  %or7 = or i1 %c2_7, %c1_7
  br i1 %or7, label %exit7, label %loop7
exit7:
  %e7 = phi i64 [ %add7, %loop7 ]
  %ec7 = phi i1 [ %c2_7, %loop7 ]
  br i1 %ec7, label %end, label %pre8
pre8:
  br label %loop8
loop8:
  %p8 = phi i64 [ %e7, %pre8 ], [ %add8, %loop8 ]
  %q8 = phi i32 [ 0, %pre8 ], [ %addq8, %loop8 ]
  %add8 = add nuw nsw i64 %p8, 1
  %gep8 = getelementptr inbounds nuw i8, ptr %arg, i64 %p8
  store i8 8, ptr %gep8, align 1
  %addq8 = add nuw nsw i32 %q8, 1
  %ld8 = load i32, ptr %g, align 4
  %sh8 = lshr i32 %addq8, %ld8
  %c1_8 = icmp ne i32 %sh8, 0
  %c2_8 = icmp ugt i64 %p8, 17407
  %or8 = or i1 %c2_8, %c1_8
  br i1 %or8, label %exit8, label %loop8
exit8:
  %e8 = phi i64 [ %add8, %loop8 ]
  %ec8 = phi i1 [ %c2_8, %loop8 ]
  br i1 %ec8, label %end, label %pre9
pre9:
  br label %loop9
loop9:
  %p9 = phi i64 [ %e8, %pre9 ], [ %add9, %loop9 ]
  %q9 = phi i32 [ 0, %pre9 ], [ %addq9, %loop9 ]
  %add9 = add nuw nsw i64 %p9, 1
  %gep9 = getelementptr inbounds nuw i8, ptr %arg, i64 %p9
  store i8 9, ptr %gep9, align 1
  %addq9 = add nuw nsw i32 %q9, 1
  %ld9 = load i32, ptr %g, align 4
  %sh9 = lshr i32 %addq9, %ld9
  %c1_9 = icmp ne i32 %sh9, 0
  %c2_9 = icmp ugt i64 %p9, 17407
  %or9 = or i1 %c2_9, %c1_9
  br i1 %or9, label %exit9, label %loop9
exit9:
  %e9 = phi i64 [ %add9, %loop9 ]
  %ec9 = phi i1 [ %c2_9, %loop9 ]
  br i1 %ec9, label %end, label %pre10
pre10:
  br label %loop10
loop10:
  %p10 = phi i64 [ %e9, %pre10 ], [ %add10, %loop10 ]
  %q10 = phi i32 [ 0, %pre10 ], [ %addq10, %loop10 ]
  %add10 = add nuw nsw i64 %p10, 1
  %gep10 = getelementptr inbounds nuw i8, ptr %arg, i64 %p10
  store i8 10, ptr %gep10, align 1
  %addq10 = add nuw nsw i32 %q10, 1
  %ld10 = load i32, ptr %g, align 4
  %sh10 = lshr i32 %addq10, %ld10
  %c1_10 = icmp ne i32 %sh10, 0
  %c2_10 = icmp ugt i64 %p10, 17407
  %or10 = or i1 %c2_10, %c1_10
  br i1 %or10, label %exit10, label %loop10
exit10:
  %e10 = phi i64 [ %add10, %loop10 ]
  %ec10 = phi i1 [ %c2_10, %loop10 ]
  br i1 %ec10, label %end, label %pre11
pre11:
  br label %loop11
loop11:
  %p11 = phi i64 [ %e10, %pre11 ], [ %add11, %loop11 ]
  %q11 = phi i32 [ 0, %pre11 ], [ %addq11, %loop11 ]
  %add11 = add nuw nsw i64 %p11, 1
  %gep11 = getelementptr inbounds nuw i8, ptr %arg, i64 %p11
  store i8 11, ptr %gep11, align 1
  %addq11 = add nuw nsw i32 %q11, 1
  %ld11 = load i32, ptr %g, align 4
  %sh11 = lshr i32 %addq11, %ld11
  %c1_11 = icmp ne i32 %sh11, 0
  %c2_11 = icmp ugt i64 %p11, 17407
  %or11 = or i1 %c2_11, %c1_11
  br i1 %or11, label %exit11, label %loop11
exit11:
  %e11 = phi i64 [ %add11, %loop11 ]
  %ec11 = phi i1 [ %c2_11, %loop11 ]
  br i1 %ec11, label %end, label %pre12
pre12:
  br label %loop12
loop12:
  %p12 = phi i64 [ %e11, %pre12 ], [ %add12, %loop12 ]
  %q12 = phi i32 [ 0, %pre12 ], [ %addq12, %loop12 ]
  %add12 = add nuw nsw i64 %p12, 1
  %gep12 = getelementptr inbounds nuw i8, ptr %arg, i64 %p12
  store i8 12, ptr %gep12, align 1
  %addq12 = add nuw nsw i32 %q12, 1
  %ld12 = load i32, ptr %g, align 4
  %sh12 = lshr i32 %addq12, %ld12
  %c1_12 = icmp ne i32 %sh12, 0
  %c2_12 = icmp ugt i64 %p12, 17407
  %or12 = or i1 %c2_12, %c1_12
  br i1 %or12, label %exit12, label %loop12
exit12:
  %e12 = phi i64 [ %add12, %loop12 ]
  %ec12 = phi i1 [ %c2_12, %loop12 ]
  br i1 %ec12, label %end, label %pre13
pre13:
  br label %loop13
loop13:
  %p13 = phi i64 [ %e12, %pre13 ], [ %add13, %loop13 ]
  %q13 = phi i32 [ 0, %pre13 ], [ %addq13, %loop13 ]
  %add13 = add nuw nsw i64 %p13, 1
  %gep13 = getelementptr inbounds nuw i8, ptr %arg, i64 %p13
  store i8 13, ptr %gep13, align 1
  %addq13 = add nuw nsw i32 %q13, 1
  %ld13 = load i32, ptr %g, align 4
  %sh13 = lshr i32 %addq13, %ld13
  %c1_13 = icmp ne i32 %sh13, 0
  %c2_13 = icmp ugt i64 %p13, 17407
  %or13 = or i1 %c2_13, %c1_13
  br i1 %or13, label %exit13, label %loop13
exit13:
  %e13 = phi i64 [ %add13, %loop13 ]
  %ec13 = phi i1 [ %c2_13, %loop13 ]
  br i1 %ec13, label %end, label %pre14
pre14:
  br label %loop14
loop14:
  %p14 = phi i64 [ %e13, %pre14 ], [ %add14, %loop14 ]
  %q14 = phi i32 [ 0, %pre14 ], [ %addq14, %loop14 ]
  %add14 = add nuw nsw i64 %p14, 1
  %gep14 = getelementptr inbounds nuw i8, ptr %arg, i64 %p14
  store i8 14, ptr %gep14, align 1
  %addq14 = add nuw nsw i32 %q14, 1
  %ld14 = load i32, ptr %g, align 4
  %sh14 = lshr i32 %addq14, %ld14
  %c1_14 = icmp ne i32 %sh14, 0
  %c2_14 = icmp ugt i64 %p14, 17407
  %or14 = or i1 %c2_14, %c1_14
  br i1 %or14, label %exit14, label %loop14
exit14:
  %e14 = phi i64 [ %add14, %loop14 ]
  %ec14 = phi i1 [ %c2_14, %loop14 ]
  br i1 %ec14, label %end, label %pre15
pre15:
  br label %loop15
loop15:
  %p15 = phi i64 [ %e14, %pre15 ], [ %add15, %loop15 ]
  %q15 = phi i32 [ 0, %pre15 ], [ %addq15, %loop15 ]
  %add15 = add nuw nsw i64 %p15, 1
  %gep15 = getelementptr inbounds nuw i8, ptr %arg, i64 %p15
  store i8 15, ptr %gep15, align 1
  %addq15 = add nuw nsw i32 %q15, 1
  %ld15 = load i32, ptr %g, align 4
  %sh15 = lshr i32 %addq15, %ld15
  %c1_15 = icmp ne i32 %sh15, 0
  %c2_15 = icmp ugt i64 %p15, 17407
  %or15 = or i1 %c2_15, %c1_15
  br i1 %or15, label %exit15, label %loop15
exit15:
  %e15 = phi i64 [ %add15, %loop15 ]
  %ec15 = phi i1 [ %c2_15, %loop15 ]
  br i1 %ec15, label %end, label %pre16
pre16:
  br label %end
end:
  ret void
}
