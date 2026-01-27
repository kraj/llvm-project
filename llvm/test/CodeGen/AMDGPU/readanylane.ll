; RUN: llc -mtriple=amdgcn-amd-amdhsa -mcpu=gfx900 < %s | FileCheck --check-prefix=GCN %s

; GCN-LABEL: {{^}}hoist_readanylane:
; GCN: ; %for.body.preheader
; GCN: ds_read_b32 [[V:v[0-9]+]],
; GCN: v_readfirstlane_b32 s{{[0-9]+}}, [[V]]

; GCN: [[L:\.LBB[0-9_]+]]: ; %for.body
; GCN-NOT: v_readfirstlane_b32
; GCN: s_cbranch_execnz [[L]]

define void @hoist_readanylane(ptr addrspace(3) inreg %in, ptr addrspace(1) %p, i32 %n) {
entry:
  %cmp4 = icmp sgt i32 %n, 0
  br i1 %cmp4, label %for.body.preheader, label %for.cond.cleanup

for.body.preheader:                               ; preds = %entry
  %uv = load i32, ptr addrspace(3) %in
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body, %entry
  ret void

for.body:                                         ; preds = %for.body.preheader, %for.body
  %i.05 = phi i32 [ %inc, %for.body ], [ 0, %for.body.preheader ]
  %idx = tail call i32 @llvm.amdgcn.readfirstlane(i32 %uv)
  %idxprom = sext i32 %idx to i64
  %arrayidx = getelementptr inbounds i32, ptr addrspace(1) %p, i64 %idxprom
  store i32 %i.05, ptr addrspace(1) %arrayidx, align 4
  %inc = add nuw nsw i32 %i.05, 1
  %exitcond.not = icmp eq i32 %inc, %n
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body
}
