; RUN: llc -filetype=null -mcpu=gfx950 -verify-machineinstrs %s
target datalayout = "e-p:64:64-p1:64:64-p2:32:32-p3:32:32-p4:64:64-p5:32:32-p6:32:32-p7:160:256:256:32-p8:128:128:128:48-p9:192:256:256:32-i64:64-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024-v2048:2048-n32:64-S32-A5-G1-ni:7:8:9"
target triple = "amdgcn-amd-amdhsa"

%"struct.ck::f4x2_pk_t" = type { i8 }

@_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared = external addrspace(3) global [49152 x i8]

define amdgpu_kernel void @_ZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentE(i32 %coerce.sroa.5.0.copyload, i32 %sub.i195.i, i32 %div15.i, <2 x i32> %0, i32 %1, i32 %2, <2 x i32> %3, i32 %4, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 %5, <2 x i32> %6, i32 %7, i32 %8, ptr addrspace(3) %9, <2 x i32> %10, i32 %11, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i85.i131.i.i.i.i.i, i32 %12, <2 x i32> %13, <4 x i8> %14, <4 x i8> %15, <4 x i8> %16, <4 x i8> %17, <4 x i8> %18, <4 x i8> %19, <4 x i8> %20, <4 x i8> %21, <4 x i8> %22, <4 x i8> %23, <16 x i8> %24, <16 x i8> %25, <16 x i8> %26, <4 x i8> %27, <16 x i8> %28, <16 x i8> %29, <16 x i8> %30, <16 x i8> %31, <16 x i8> %32, <16 x i8> %33, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i273.i.i.i.i.i.i.i.i.i1383.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i109.i.i.i.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i144.i184.i.i.i.i.i, ptr addrspace(3) %arrayidx.i.i.i73.i.i.i.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared1, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i5.i.i.i.i.i, <2 x i32> %34, ptr addrspace(3) %35, i32 %36, <2 x i32> %37, i32 %38, i32 %39, i32 %invariant.op112.i, <4 x i8> %40, <16 x i8> %41, i32 %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i, ptr addrspace(3) %42) #0 {
entry:
  %43 = tail call i32 @llvm.amdgcn.workitem.id.x()
  %44 = insertelement <2 x i32> zeroinitializer, i32 %43, i64 0
  %45 = shufflevector <2 x i32> %44, <2 x i32> zeroinitializer, <2 x i32> zeroinitializer
  %46 = and <2 x i32> %45, %0
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i18.i.i.i.i.i.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 0, i32 0, i32 0)
  %sub.i1.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i302.i.i.i.i.i.i.i.i.i.i.i = sub i32 0, %43
  %47 = shufflevector <2 x i32> %46, <2 x i32> %0, <2 x i32> <i32 1, i32 2>
  %48 = lshr <2 x i32> %47, splat (i32 1)
  %49 = extractelement <2 x i32> %48, i64 0
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 %49
  %50 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i, i32 4
  %51 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i, i32 8
  %52 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i, i32 12
  %53 = extractelement <2 x i32> %48, i64 1
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i6.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 %53
  %54 = shl <2 x i32> %3, <i32 20480, i32 5>
  %55 = shufflevector <2 x i32> %54, <2 x i32> zeroinitializer, <2 x i32> <i32 1, i32 1>
  %56 = extractelement <2 x i32> %55, i64 0
  %57 = ashr i32 %56, 1
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i17.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared1, i32 %57
  %58 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i17.i.i.i.i, i32 4
  %59 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i17.i.i.i.i, i32 8
  %60 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i17.i.i.i.i, i32 12
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i26.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 %5
  %61 = insertelement <2 x i32> %44, i32 %coerce.sroa.5.0.copyload, i64 1
  %62 = lshr <2 x i32> %61, splat (i32 1)
  %63 = extractelement <2 x i32> %62, i64 0
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i5.i.i.i.i.i2 = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %9, i32 %63
  %64 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i5.i.i.i.i.i2, i32 4
  %65 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i5.i.i.i.i.i, i32 8
  %66 = extractelement <2 x i32> %34, i64 1
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i6.i55.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %35, i32 %36
  %67 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i6.i55.i.i.i.i.i, i32 4
  %68 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i6.i55.i.i.i.i.i, i32 8
  %69 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i6.i55.i.i.i.i.i, i32 12
  %70 = or <2 x i32> %37, <i32 16384, i32 20480>
  %71 = lshr <2 x i32> %70, splat (i32 1)
  %72 = extractelement <2 x i32> %71, i64 0
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i57.i102.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %9, i32 %72
  %73 = extractelement <2 x i32> %71, i64 1
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i109.i149.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) getelementptr inbounds nuw (i8, ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 16384), i32 %73
  %74 = or <2 x i32> %37, <i32 32768, i32 36864>
  %75 = lshr <2 x i32> %74, splat (i32 1)
  %76 = extractelement <2 x i32> %75, i64 0
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i161.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) getelementptr inbounds nuw (i8, ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 16384), i32 %76
  %77 = extractelement <2 x i32> %75, i64 1
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i213.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) getelementptr inbounds nuw (i8, ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 16384), i32 %77
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i85.i131.i.i.i.i.i10 = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %9, i32 %11
  %78 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i85.i131.i.i.i.i.i10, i32 4
  %79 = getelementptr i8, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i85.i131.i.i.i.i.i10, i32 8
  %80 = or <2 x i32> %47, splat (i32 1)
  %81 = extractelement <2 x i32> %80, i64 0
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i203.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %35, i32 %81
  %82 = extractelement <2 x i32> %80, i64 1
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i262.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %35, i32 %82
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i321.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) getelementptr inbounds nuw (i8, ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 16384), i32 %43
  %83 = shl i32 %43, 1
  %div.i.i.i.i.i.i10.i.i.i.i.i.i.i.i150.i.i.i.i = sdiv i32 %sub.i1.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i302.i.i.i.i.i.i.i.i.i.i.i, 2
  %arrayidx.i.i.i.i.i.i.i13.i.i.i.i.i.i.i.i153.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) getelementptr inbounds nuw (i8, ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 16384), i32 %div.i.i.i.i.i.i10.i.i.i.i.i.i.i.i150.i.i.i.i
  %84 = sdiv <2 x i32> %13, splat (i32 2)
  %85 = extractelement <2 x i32> %84, i64 0
  %arrayidx.i.i.i.i.i.i.i273.i.i.i.i.i.i.i.i.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) %35, i32 %85
  %86 = extractelement <2 x i32> %84, i64 1
  %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i134.i.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) getelementptr inbounds nuw (i8, ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 16384), i32 %86
  %87 = extractelement <2 x i32> %0, i64 0
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 0, i32 0, i32 0)
  %a_scale_thread_bufs.sroa.0.0.insert.ext17404.i.i.i = zext i32 %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i to i64
  %88 = or <2 x i32> %44, %13
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i18.i.i.i.i.i.i.i.i, ptr addrspace(3) null, align 16
  %invariant.op112.i3 = mul i32 %38, 3
  br label %do.body.i.i.i

do.body.i.i.i:                                    ; preds = %do.body.i.i.i, %entry
  %c_thread_buf.sroa.19.0.i.i = phi <4 x float> [ zeroinitializer, %entry ], [ splat (float 1.000000e+00), %do.body.i.i.i ]
  %b_blockwise_copy.sroa.435.0.in.i.i = phi <4 x i32> [ zeroinitializer, %entry ], [ %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i357.i.i.i.i.i.i1456.i.i.i, %do.body.i.i.i ]
  %a_blockwise_copy.sroa.0.0.i.i = phi <4 x i32> [ zeroinitializer, %entry ], [ %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i1051.i.i.i, %do.body.i.i.i ]
  %a_blockwise_copy.sroa.4732366.2.i.i = phi i32 [ 0, %entry ], [ %coerce.sroa.5.0.copyload, %do.body.i.i.i ]
  %a_scale_thread_bufs.sroa.0.0.i.i.i = phi i64 [ %a_scale_thread_bufs.sroa.0.0.insert.ext17404.i.i.i, %entry ], [ 0, %do.body.i.i.i ]
  %89 = phi <4 x i8> [ zeroinitializer, %entry ], [ %209, %do.body.i.i.i ]
  %90 = phi <4 x i8> [ zeroinitializer, %entry ], [ %191, %do.body.i.i.i ]
  %91 = phi <4 x i8> [ zeroinitializer, %entry ], [ %207, %do.body.i.i.i ]
  %92 = phi <4 x i8> [ zeroinitializer, %entry ], [ %206, %do.body.i.i.i ]
  %93 = phi <4 x i8> [ zeroinitializer, %entry ], [ %186, %do.body.i.i.i ]
  %94 = phi <4 x i8> [ zeroinitializer, %entry ], [ %211, %do.body.i.i.i ]
  %95 = phi <4 x i8> [ zeroinitializer, %entry ], [ %197, %do.body.i.i.i ]
  %96 = phi <4 x i8> [ zeroinitializer, %entry ], [ %213, %do.body.i.i.i ]
  %97 = phi <4 x i8> [ zeroinitializer, %entry ], [ %23, %do.body.i.i.i ]
  %98 = phi <4 x i8> [ zeroinitializer, %entry ], [ %200, %do.body.i.i.i ]
  %99 = phi <4 x i8> [ zeroinitializer, %entry ], [ %204, %do.body.i.i.i ]
  %100 = phi <4 x i8> [ zeroinitializer, %entry ], [ %193, %do.body.i.i.i ]
  %101 = phi <4 x i8> [ zeroinitializer, %entry ], [ splat (i8 1), %do.body.i.i.i ]
  %102 = phi <2 x i32> [ %88, %entry ], [ zeroinitializer, %do.body.i.i.i ]
  %103 = phi <2 x i32> [ zeroinitializer, %entry ], [ %157, %do.body.i.i.i ]
  %104 = phi <2 x i32> [ zeroinitializer, %entry ], [ %220, %do.body.i.i.i ]
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i4 = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 0, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i7.i.i.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %7, i32 0, i32 0)
  %105 = extractelement <2 x i32> %103, i64 1
  %add.i.i.i.i.i9.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.reass.i.i = or i32 %83, %105
  store <4 x i32> %a_blockwise_copy.sroa.0.0.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i144.i184.i.i.i.i.i, align 16
  %106 = extractelement <2 x i32> %102, i64 1
  %call.i.i.i.i.i.i.i.i.i.i.i17.i.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %5, i32 0, i32 0)
  %add.i.i.i2.i.i.i.i.i.i.i.i32.i.i.i.i = add i32 %39, %38
  %add.i.i.i2.i.i.i.i.i.i.i49.i.i.i.i.reass.i = add i32 %106, %invariant.op112.i
  %call.i.i.i.i.i.i.i.i.i.i56.i.i.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %add.i.i.i2.i.i.i.i.i.i.i49.i.i.i.i.reass.i, i32 0, i32 0)
  store <4 x i32> zeroinitializer, ptr addrspace(3) null, align 16
  store <4 x i32> zeroinitializer, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i13.i.i.i.i.i.i.i.i153.i.i.i.i, align 16
  store <4 x i32> zeroinitializer, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i273.i.i.i.i.i.i.i.i.i.i.i.i, align 16
  store <4 x i32> %b_blockwise_copy.sroa.435.0.in.i.i, ptr addrspace(3) null, align 16
  %.pre.i.i.i.i.i208.i.i.i.i = sdiv i32 %coerce.sroa.5.0.copyload, 2
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i18.i.i.i.i.i215.i.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 %.pre.i.i.i.i.i208.i.i.i.i, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i73.i.i.i.i.i226.i.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 %1, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i189.i.i.i.i.i.i.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 %2, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i305.i.i.i.i.i.i.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 %4, i32 0, i32 0)
  %a_scale_thread_bufs.sroa.0.0.extract.trunc.i.i.i = trunc i64 %a_scale_thread_bufs.sroa.0.0.i.i.i to i32
  %107 = shufflevector <4 x i8> %93, <4 x i8> %16, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %108 = shufflevector <4 x i8> %40, <4 x i8> %100, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %109 = shufflevector <16 x i8> %107, <16 x i8> %29, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %110 = shufflevector <4 x i8> %15, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %111 = shufflevector <16 x i8> %108, <16 x i8> %41, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %112 = shufflevector <16 x i8> %109, <16 x i8> %24, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %113 = shufflevector <16 x i8> %111, <16 x i8> %25, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %114 = bitcast <16 x i8> %112 to <4 x i32>
  %115 = bitcast <16 x i8> %113 to <4 x i32>
  %116 = extractelement <2 x i32> %104, i64 0
  %117 = shufflevector <4 x i8> %15, <4 x i8> %95, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %118 = shufflevector <16 x i8> %117, <16 x i8> %24, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %119 = bitcast <16 x i8> %118 to <4 x i32>
  %120 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %114, <4 x i32> %119, <4 x float> %c_thread_buf.sroa.19.0.i.i, i32 4, i32 4, i32 0, i32 %a_scale_thread_bufs.sroa.0.0.extract.trunc.i.i.i, i32 0, i32 0)
  %121 = shufflevector <4 x i8> %90, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %122 = shufflevector <16 x i8> %30, <16 x i8> %121, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %123 = bitcast <16 x i8> %122 to <4 x i32>
  %124 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %123, <4 x i32> %115, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %125 = shufflevector <4 x i8> %16, <4 x i8> %92, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %126 = shufflevector <4 x i8> %91, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %127 = shufflevector <16 x i8> %125, <16 x i8> %126, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %128 = shufflevector <4 x i8> %99, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %129 = shufflevector <16 x i8> %31, <16 x i8> %128, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %130 = shufflevector <16 x i8> %127, <16 x i8> %24, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %131 = bitcast <16 x i8> %130 to <4 x i32>
  %132 = bitcast <16 x i8> %129 to <4 x i32>
  %133 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %24, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %134 = shufflevector <4 x i8> %94, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %135 = shufflevector <16 x i8> %133, <16 x i8> %134, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %136 = bitcast <16 x i8> %135 to <4 x i32>
  %137 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %136, <4 x float> %120, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %138 = shufflevector <4 x i8> %89, <4 x i8> %17, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %139 = shufflevector <16 x i8> %138, <16 x i8> %33, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %140 = shufflevector <16 x i8> %139, <16 x i8> %28, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %141 = bitcast <16 x i8> %140 to <4 x i32>
  %142 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %141, <4 x i32> %132, <4 x float> %124, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %143 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> splat (float 1.000000e+00), i32 4, i32 4, i32 0, i32 0, i32 0, i32 %116)
  %144 = shufflevector <4 x i8> %98, <4 x i8> %14, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %145 = shufflevector <16 x i8> %144, <16 x i8> %32, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %146 = shufflevector <4 x i8> %97, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %147 = shufflevector <16 x i8> %145, <16 x i8> %146, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %148 = bitcast <16 x i8> %147 to <4 x i32>
  %149 = extractelement <2 x i32> %104, i64 1
  %150 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %148, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %151 = shufflevector <4 x i8> %14, <4 x i8> %96, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %152 = shufflevector <16 x i8> %151, <16 x i8> %26, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %153 = shufflevector <4 x i8> %20, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %154 = shufflevector <16 x i8> %152, <16 x i8> %153, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %155 = bitcast <16 x i8> %154 to <4 x i32>
  %156 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %131, <4 x i32> %155, <4 x float> %150, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %149)
  tail call void @llvm.amdgcn.sched.barrier(i32 0)
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i1051.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 0, i32 0, i32 0)
  %add.i.i.i.i.i12.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i1058.i.i.reass.i = or i32 %a_blockwise_copy.sroa.4732366.2.i.i, 1
  %.pre.i.i.i.i.i.i1059.i.i.i = sdiv i32 %add.i.i.i.i.i12.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i1058.i.i.reass.i, 2
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i18.i.i.i.i.i.i1066.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 %.pre.i.i.i.i.i.i1059.i.i.i, i32 0, i32 0)
  %.pre6.i.i.i.i.i.i1083.i.i.i = sdiv i32 %sub.i195.i, 2
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i125.i.i.i.i.i.i1090.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 %.pre6.i.i.i.i.i.i1083.i.i.i, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i.i16.i.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %sub.i195.i, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i7.i32.i.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %11, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i31.i.i1092.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %8, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i56.i.i1106.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %coerce.sroa.5.0.copyload, i32 0, i32 0)
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i18.i.i.i.i.i215.i.i.i.i, ptr addrspace(3) null, align 16
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i73.i.i.i.i.i226.i.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 16
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i189.i.i.i.i.i.i.i.i.i, ptr addrspace(3) null, align 16
  store <4 x i32> zeroinitializer, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 16
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i305.i.i.i.i.i.i.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i273.i.i.i.i.i.i.i.i.i1383.i.i.i, align 16
  %157 = insertelement <2 x i32> zeroinitializer, i32 %add.i.i.i.i.i9.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.reass.i.i, i64 1
  %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i357.i.i.i.i.i.i1456.i.i.i = tail call <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32> zeroinitializer, i32 0, i32 0, i32 0)
  %158 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %143, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %159 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %160 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %156, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %161 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i, i32 0, i32 0)
  %162 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 2, i32 1, i32 0, i32 0)
  %163 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 1, i32 1, i32 1, i32 0)
  %164 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 1, i32 %sub.i195.i, i32 0, i32 0)
  %165 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 %sub.i195.i, i32 0, i32 0)
  %166 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i56.i.i.i.i.i)
  %167 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 %coerce.sroa.5.0.copyload, i32 1, i32 0)
  %168 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 %sub.i195.i, i32 1, i32 0)
  %169 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i.i17.i.i.i.i)
  %170 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i7.i.i.i.i.i, i32 0, i32 0)
  %171 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %coerce.sroa.5.0.copyload)
  %172 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 1, i32 0, i32 0)
  %173 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 %coerce.sroa.5.0.copyload, i32 0, i32 0)
  %174 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %172, i32 4, i32 4, i32 0, i32 1, i32 0, i32 0)
  %175 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %173, i32 4, i32 4, i32 0, i32 0, i32 0, i32 1)
  %176 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %1)
  %177 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> splat (float 1.000000e+00), i32 4, i32 4, i32 0, i32 0, i32 0, i32 1)
  %178 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 1)
  %179 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 1, i32 0, i32 0, i32 1)
  %180 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 1, i32 1, i32 0)
  %181 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %sub.i195.i)
  %182 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 1, i32 %coerce.sroa.5.0.copyload, i32 1, i32 0)
  %183 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 1, i32 1, i32 0, i32 0)
  %184 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 1, i32 %coerce.sroa.5.0.copyload, i32 0, i32 0)
  %185 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 3)
  %186 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i, align 16
  %187 = load <4 x i8>, ptr addrspace(3) %50, align 4
  %188 = load <4 x i8>, ptr addrspace(3) %51, align 8
  %189 = load <4 x i8>, ptr addrspace(3) %52, align 4
  %190 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i6.i.i.i.i.i.i, align 16
  %191 = load <4 x i8>, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 4
  %192 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i5.i.i.i.i.i2, align 16
  %193 = load <4 x i8>, ptr addrspace(3) %64, align 4
  %194 = load <4 x i8>, ptr addrspace(3) %42, align 8
  %195 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i73.i.i.i.i.i.i, align 4
  %196 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i6.i55.i.i.i.i.i, align 16
  %197 = load <4 x i8>, ptr addrspace(3) %67, align 4
  %198 = load <4 x i8>, ptr addrspace(3) %68, align 8
  %199 = load <4 x i8>, ptr addrspace(3) %69, align 4
  %200 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i57.i102.i.i.i.i.i, align 16
  %201 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i109.i149.i.i.i.i.i, align 8
  %202 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i161.i.i.i.i.i.i, align 16
  %203 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i213.i.i.i.i.i.i, align 16
  %204 = load <4 x i8>, ptr addrspace(3) null, align 16
  %205 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i17.i.i.i.i, align 16
  %206 = load <4 x i8>, ptr addrspace(3) %58, align 4
  %207 = load <4 x i8>, ptr addrspace(3) %59, align 8
  %208 = load <4 x i8>, ptr addrspace(3) %60, align 4
  %209 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i26.i.i.i.i.i.i, align 16
  %210 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i109.i.i.i.i.i.i, align 16
  %211 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i85.i131.i.i.i.i.i, align 4
  %212 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i85.i131.i.i.i.i.i10, align 16
  %213 = load <4 x i8>, ptr addrspace(3) %78, align 4
  %214 = load <4 x i8>, ptr addrspace(3) %79, align 8
  %215 = load <4 x i8>, ptr addrspace(3) %9, align 16
  %216 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i203.i.i.i.i.i.i, align 4
  %217 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i262.i.i.i.i.i.i, align 16
  %218 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i321.i.i.i.i.i.i, align 16
  tail call void @llvm.amdgcn.sched.group.barrier(i32 0, i32 0, i32 0)
  %cmp.i475.i.i = icmp slt i32 0, %coerce.sroa.5.0.copyload
  %219 = insertelement <2 x i32> zeroinitializer, i32 %call.i.i.i.i.i.i.i.i.i.i.i16.i.i.i.i, i64 0
  %220 = insertelement <2 x i32> %219, i32 %call.i.i.i.i.i.i.i.i.i.i7.i32.i.i.i.i, i64 1
  br i1 %cmp.i475.i.i, label %do.body.i.i.i, label %_ZN2ck30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS2_11ColumnMajorES3_NS_9f4x2_pk_tENS_11e8m0_bexp_tES5_S6_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughES9_S9_LNS7_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSC_IJLi1ELi0ELi2EEEESE_Li2ELi32ELi32ELb0ELi0ESD_SE_SE_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSC_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES5_S5_Lb0ELb0EE3RunILb1ELNS_25InMemoryDataOperationEnumE0ELNS_10TailNumberE1EEEvPKS5_PKS6_SN_SP_PDF16_PvRKNSI_7ProblemE.exit

_ZN2ck30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS2_11ColumnMajorES3_NS_9f4x2_pk_tENS_11e8m0_bexp_tES5_S6_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughES9_S9_LNS7_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSC_IJLi1ELi0ELi2EEEESE_Li2ELi32ELi32ELb0ELi0ESD_SE_SE_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSC_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES5_S5_Lb0ELb0EE3RunILb1ELNS_25InMemoryDataOperationEnumE0ELNS_10TailNumberE1EEEvPKS5_PKS6_SN_SP_PDF16_PvRKNSI_7ProblemE.exit: ; preds = %do.body.i.i.i
  %call.i.i.i.i.i.i.i.i.i.i7.i5180.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 0, i32 0, i32 0)
  %call.i.i.i.i.i.i.i.i.i.i.i5201.i.i.i = tail call i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32> zeroinitializer, i32 %106, i32 0, i32 0)
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i1051.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i109.i.i.i.i.i.i, align 16
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i18.i.i.i.i.i.i1066.i.i.i, ptr addrspace(3) null, align 16
  %add.i.i.i.i.i9.i.i.i.i.i.i.i.i.i.i.i102.i.i.i.i.i.i.i.i5428.i.i.i = or i32 %83, %div15.i
  %221 = insertelement <2 x i32> zeroinitializer, i32 0, i64 0
  %arrayidx.i.i.i.i.i.i.i115.i.i.i.i.i.i.i.i5433.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 %add.i.i.i.i.i9.i.i.i.i.i.i.i.i.i.i.i102.i.i.i.i.i.i.i.i5428.i.i.i
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i125.i.i.i.i.i.i1090.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i115.i.i.i.i.i.i.i.i5433.i.i.i, align 16
  store <4 x i32> zeroinitializer, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i134.i.i.i.i, align 16
  store <4 x i32> zeroinitializer, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i13.i.i.i.i.i.i.i.i153.i.i.i.i, align 16
  %222 = insertelement <2 x i32> zeroinitializer, i32 0, i64 0
  %arrayidx.i.i.i.i.i.i.i323.i.i.i.i.i.i.i.i5832.i.i.i = getelementptr %"struct.ck::f4x2_pk_t", ptr addrspace(3) getelementptr inbounds nuw (i8, ptr addrspace(3) @_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 16384), i32 %sub.i1.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i302.i.i.i.i.i.i.i.i.i.i.i
  store <4 x i32> %call.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i357.i.i.i.i.i.i1456.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i323.i.i.i.i.i.i.i.i5832.i.i.i, align 16
  %a_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i231 = shufflevector <4 x i8> %186, <4 x i8> %187, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i227 = shufflevector <4 x i8> %192, <4 x i8> %193, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %223 = shufflevector <4 x i8> %188, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %a_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i232 = shufflevector <16 x i8> %a_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i231, <16 x i8> %223, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %224 = shufflevector <4 x i8> %194, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i228 = shufflevector <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i227, <16 x i8> %224, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %225 = shufflevector <4 x i8> %189, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i233 = shufflevector <16 x i8> %a_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i232, <16 x i8> %225, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %226 = shufflevector <4 x i8> %195, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i229 = shufflevector <16 x i8> %b_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i228, <16 x i8> %226, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %227 = bitcast <16 x i8> %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i233 to <4 x i32>
  %228 = bitcast <16 x i8> %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i229 to <4 x i32>
  %229 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %159, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %b_thread_vec.sroa.0.7.vec.insert.i19.i.i.i.i.i.i.i.i.i.i.i.i.i.i223 = shufflevector <4 x i8> %196, <4 x i8> %15, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %230 = shufflevector <4 x i8> %198, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.11.vec.insert.i27.i.i.i.i.i.i.i.i.i.i.i.i.i.i224 = shufflevector <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i19.i.i.i.i.i.i.i.i.i.i.i.i.i.i223, <16 x i8> %230, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %231 = shufflevector <4 x i8> %199, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.15.vec.insert.i35.i.i.i.i.i.i.i.i.i.i.i.i.i.i225 = shufflevector <16 x i8> %b_thread_vec.sroa.0.11.vec.insert.i27.i.i.i.i.i.i.i.i.i.i.i.i.i.i224, <16 x i8> %231, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %232 = bitcast <16 x i8> %b_thread_vec.sroa.0.15.vec.insert.i35.i.i.i.i.i.i.i.i.i.i.i.i.i.i225 to <4 x i32>
  %233 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %137, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %a_thread_vec.sroa.0.7.vec.insert.i.i.i23.i.i.i.i.i.i.i.i.i.i.i.i219 = shufflevector <4 x i8> %190, <4 x i8> %15, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %234 = shufflevector <4 x i8> %18, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %a_thread_vec.sroa.0.11.vec.insert.i.i.i35.i.i.i.i.i.i.i.i.i.i.i.i220 = shufflevector <16 x i8> %a_thread_vec.sroa.0.7.vec.insert.i.i.i23.i.i.i.i.i.i.i.i.i.i.i.i219, <16 x i8> %234, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %235 = shufflevector <4 x i8> %191, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %a_thread_vec.sroa.0.15.vec.insert.i.i.i47.i.i.i.i.i.i.i.i.i.i.i.i221 = shufflevector <16 x i8> %a_thread_vec.sroa.0.11.vec.insert.i.i.i35.i.i.i.i.i.i.i.i.i.i.i.i220, <16 x i8> %235, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %236 = bitcast <16 x i8> %a_thread_vec.sroa.0.15.vec.insert.i.i.i47.i.i.i.i.i.i.i.i.i.i.i.i221 to <4 x i32>
  %237 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %142, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %238 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %158, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %a_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i18.i.i.i.i.i.i.i.i.i.i215 = shufflevector <4 x i8> %205, <4 x i8> %22, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %a_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i26.i.i.i.i.i.i.i.i.i.i216 = shufflevector <16 x i8> %a_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i18.i.i.i.i.i.i.i.i.i.i215, <16 x i8> %28, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %239 = shufflevector <4 x i8> %208, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i34.i.i.i.i.i.i.i.i.i.i217 = shufflevector <16 x i8> %a_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i26.i.i.i.i.i.i.i.i.i.i216, <16 x i8> %239, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i35.i.i.i.i.i.i.i.i.i.i213 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %29, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %240 = bitcast <16 x i8> %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i34.i.i.i.i.i.i.i.i.i.i217 to <4 x i32>
  %241 = bitcast <16 x i8> %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i35.i.i.i.i.i.i.i.i.i.i213 to <4 x i32>
  %242 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %229, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %243 = shufflevector <4 x i8> %211, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.15.vec.insert.i64.i.i.i.i.i.i.i.i.i.i.i.i.i.i209 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %243, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %244 = bitcast <16 x i8> %b_thread_vec.sroa.0.15.vec.insert.i64.i.i.i.i.i.i.i.i.i.i.i.i.i.i209 to <4 x i32>
  %245 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %233, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %a_thread_vec.sroa.0.7.vec.insert.i.i.i30.i.i.i.i.i.i.i.i.i.i.i.i203 = shufflevector <4 x i8> %209, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %246 = bitcast <16 x i8> %a_thread_vec.sroa.0.7.vec.insert.i.i.i30.i.i.i.i.i.i.i.i.i.i.i.i203 to <4 x i32>
  %247 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %237, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %248 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %238, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i33.i.i.i.i.i.i199 = shufflevector <4 x i8> %200, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %249 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i33.i.i.i.i.i.i199 to <4 x i32>
  %250 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %160, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %251 = shufflevector <4 x i8> %201, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.11.vec.insert.i47.i.i.i.i.i.i.i.i.i.i.i.i.i.i196 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %251, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.15.vec.insert.i63.i.i.i.i.i.i.i.i.i.i.i.i.i.i197 = shufflevector <16 x i8> %b_thread_vec.sroa.0.11.vec.insert.i47.i.i.i.i.i.i.i.i.i.i.i.i.i.i196, <16 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %252 = bitcast <16 x i8> %b_thread_vec.sroa.0.15.vec.insert.i63.i.i.i.i.i.i.i.i.i.i.i.i.i.i197 to <4 x i32>
  %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i25.i.i.i.i.i.i.i.i.i.i191 = shufflevector <4 x i8> %212, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %253 = shufflevector <4 x i8> %214, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i37.i.i.i.i.i.i.i.i.i.i192 = shufflevector <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i25.i.i.i.i.i.i.i.i.i.i191, <16 x i8> %253, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 16, i32 17, i32 18, i32 19, i32 poison, i32 poison, i32 poison, i32 poison>
  %254 = bitcast <16 x i8> %b_thread_vec.sroa.0.11.vec.insert.i.i.i.i.i37.i.i.i.i.i.i.i.i.i.i192 to <4 x i32>
  %255 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %250, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %b_thread_vec.sroa.0.7.vec.insert.i32.i.i.i.i68.i.i.i.i.i.i.i.i.i.i187 = shufflevector <4 x i8> %215, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %256 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i32.i.i.i.i68.i.i.i.i.i.i.i.i.i.i187 to <4 x i32>
  %257 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %258 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %254, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %259 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 1)
  %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i271.i.i.i.i.i.i183 = shufflevector <4 x i8> %202, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %260 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i271.i.i.i.i.i.i183 to <4 x i32>
  %261 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %227, <4 x i32> %260, <4 x float> %161, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i31.i.i1092.i.i.i)
  %b_thread_vec.sroa.0.7.vec.insert.i31.i.i.i.i.i.i.i.i328.i.i.i.i.i.i179 = shufflevector <4 x i8> %203, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %262 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i31.i.i.i.i.i.i.i.i328.i.i.i.i.i.i179 to <4 x i32>
  %263 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %262, <4 x float> %162, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %264 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %163, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %265 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %164, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %266 = shufflevector <4 x i8> %216, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i49.i.i.i.i499.i.i.i.i.i.i177 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %266, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %267 = bitcast <16 x i8> %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i49.i.i.i.i499.i.i.i.i.i.i177 to <4 x i32>
  %268 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %261, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %b_thread_vec.sroa.0.7.vec.insert.i32.i.i.i.i68.i.i.i.i523.i.i.i.i.i.i171 = shufflevector <4 x i8> %217, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %269 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i32.i.i.i.i68.i.i.i.i523.i.i.i.i.i.i171 to <4 x i32>
  %270 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %267, <4 x float> %264, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %271 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %269, <4 x float> %265, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i660.i.i.i.i.i.i167 = shufflevector <4 x i8> %204, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %272 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i660.i.i.i.i.i.i167 to <4 x i32>
  %273 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %166, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %274 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %236, <4 x i32> zeroinitializer, <4 x float> %168, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i25.i.i.i.i856.i.i.i.i.i.i159 = shufflevector <4 x i8> %218, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %275 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i25.i.i.i.i856.i.i.i.i.i.i159 to <4 x i32>
  %276 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %240, <4 x i32> %275, <4 x float> %165, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %277 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %246, <4 x i32> zeroinitializer, <4 x float> %274, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i56.i.i1106.i.i.i)
  %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i63.i.i.i.i153 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %24, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %278 = bitcast <16 x i8> %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i63.i.i.i.i153 to <4 x i32>
  %279 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %278, <4 x i32> %228, <4 x float> %169, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %280 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %232, <4 x float> %170, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %a_thread_vec.sroa.0.15.vec.insert.i.i.i61.i.i.i.i.i.i.i.i.i.i.i.i149 = shufflevector <16 x i8> %24, <16 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %281 = bitcast <16 x i8> %a_thread_vec.sroa.0.15.vec.insert.i.i.i61.i.i.i.i.i.i.i.i.i.i.i.i149 to <4 x i32>
  %282 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %171, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i.i16.i.i.i.i)
  %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i48.i.i.i.i.i.i155.i.i.i.i145 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %25, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %283 = bitcast <16 x i8> %a_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i48.i.i.i.i.i.i155.i.i.i.i145 to <4 x i32>
  %284 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %283, <4 x i32> %241, <4 x float> zeroinitializer, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %a_thread_vec.sroa.0.7.vec.insert.i.i.i30.i.i.i.i.i.i.i.i220.i.i.i.i139 = shufflevector <4 x i8> %210, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %285 = bitcast <16 x i8> %a_thread_vec.sroa.0.7.vec.insert.i.i.i30.i.i.i.i.i.i.i.i220.i.i.i.i139 to <4 x i32>
  %286 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %285, <4 x i32> %244, <4 x float> %282, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %287 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %249, <4 x float> %174, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %288 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %252, <4 x float> %175, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %289 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %176, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %290 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %281, <4 x i32> zeroinitializer, <4 x float> %177, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %291 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %256, <4 x float> %290, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %292 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %178, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %293 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %180, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %294 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %181, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %295 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %272, <4 x float> %182, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %296 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %183, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %297 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %184, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %298 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %185, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %299 = load <4 x i8>, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i5.i.i.i.i.i2, align 4
  %300 = shufflevector <4 x i8> %299, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i7097.i.i.i325 = shufflevector <16 x i8> zeroinitializer, <16 x i8> %300, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %301 = bitcast <16 x i8> %b_thread_vec.sroa.0.15.vec.insert.i.i.i.i.i.i.i.i.i.i.i.i7097.i.i.i325 to <4 x i32>
  %a_thread_vec.sroa.0.15.vec.insert.i.i.i47.i.i.i.i.i.i.i.i.i7193.i.i.i317 = shufflevector <16 x i8> %25, <16 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 16, i32 17, i32 18, i32 19>
  %302 = bitcast <16 x i8> %a_thread_vec.sroa.0.15.vec.insert.i.i.i47.i.i.i.i.i.i.i.i.i7193.i.i.i317 to <4 x i32>
  %303 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %302, <4 x i32> zeroinitializer, <4 x float> %247, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %304 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %242, i32 4, i32 4, i32 0, i32 0, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i.i5201.i.i.i)
  %305 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %245, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %306 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %248, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i33.i.i.i7446.i.i.i295 = shufflevector <4 x i8> %14, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %307 = bitcast <16 x i8> %b_thread_vec.sroa.0.7.vec.insert.i.i.i.i.i.i.i.i.i33.i.i.i7446.i.i.i295 to <4 x i32>
  %308 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> %307, <4 x float> %255, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %309 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %257, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %310 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %258, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %311 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %259, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %312 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %263, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %313 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %271, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %314 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> splat (i32 16843009), <4 x float> %273, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %315 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> splat (i32 1), <4 x float> %167, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %316 = bitcast <16 x i8> %24 to <4 x i32>
  %317 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %316, <4 x i32> %301, <4 x float> %284, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %a_thread_vec.sroa.0.7.vec.insert.i.i.i29.i.i.i.i.i.i.i.i.i8505.i.i.i243 = shufflevector <4 x i8> %19, <4 x i8> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison, i32 poison>
  %318 = bitcast <16 x i8> %a_thread_vec.sroa.0.7.vec.insert.i.i.i29.i.i.i.i.i.i.i.i.i8505.i.i.i243 to <4 x i32>
  %319 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> %318, <4 x i32> zeroinitializer, <4 x float> %286, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %320 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %287, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %321 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %289, i32 4, i32 4, i32 0, i32 0, i32 0, i32 0)
  %322 = tail call <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32> zeroinitializer, <4 x i32> zeroinitializer, <4 x float> %179, i32 4, i32 4, i32 0, i32 %call.i.i.i.i.i.i.i.i.i.i7.i5180.i.i.i, i32 0, i32 0)
  %323 = extractelement <2 x i32> %46, i64 0
  %c_thread_buf.sroa.0.0.vec.extract.i.i = extractelement <4 x float> %304, i64 0
  %conv.i.i.i.i.i.i.i.i.i654.i.i = fptrunc float %c_thread_buf.sroa.0.0.vec.extract.i.i to half
  store half %conv.i.i.i.i.i.i.i.i.i654.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.19.16.vec.extract.i.i = extractelement <4 x float> %305, i64 0
  %conv.i.i.i.i.i69.i.i.i.i.i.i = fptrunc float %c_thread_buf.sroa.19.16.vec.extract.i.i to half
  store half %conv.i.i.i.i.i69.i.i.i.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.51.48.vec.extract.i.i = extractelement <4 x float> %306, i64 0
  %conv.i.i.i.i.i78.i.i.i.i.i.i = fptrunc float %c_thread_buf.sroa.51.48.vec.extract.i.i to half
  store half %conv.i.i.i.i.i78.i.i.i.i.i.i, ptr addrspace(3) null, align 2
  %add.i.i.i2.i.i32.i.i.i.i124.i.i.i.i.i.i = or i32 %323, 0
  %c_thread_buf.sroa.35.44.vec.extract.i.i = extractelement <4 x float> %303, i64 0
  %conv.i.i.i.i.i126.i.i.i.i.i.i = fptrunc float %c_thread_buf.sroa.35.44.vec.extract.i.i to half
  %arrayidx.i.i.i130.i.i.i.i.i.i = getelementptr half, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, i32 %add.i.i.i2.i.i32.i.i.i.i124.i.i.i.i.i.i
  store half %conv.i.i.i.i.i126.i.i.i.i.i.i, ptr addrspace(3) %arrayidx.i.i.i130.i.i.i.i.i.i, align 2
  %c_thread_buf.sroa.67.76.vec.extract.i.i = extractelement <4 x float> %308, i64 3
  %conv.i.i.i.i.i26.i.i.i30.i.i.i = fptrunc float %c_thread_buf.sroa.67.76.vec.extract.i.i to half
  %c_thread_buf.sroa.83.80.vec.extract.i.i = extractelement <4 x float> %309, i64 0
  %conv.i.i.i.i.i69.i.i.i59.i.i.i = fptrunc float %c_thread_buf.sroa.83.80.vec.extract.i.i to half
  %c_thread_buf.sroa.115.112.vec.extract.i.i = extractelement <4 x float> %311, i64 0
  %conv.i.i.i.i.i78.i.i.i69.i.i.i = fptrunc float %c_thread_buf.sroa.115.112.vec.extract.i.i to half
  %c_thread_buf.sroa.99.100.vec.extract.i.i = extractelement <4 x float> %310, i64 0
  %conv.i.i.i.i.i150.i.i.i108.i.i.i = fptrunc float %c_thread_buf.sroa.99.100.vec.extract.i.i to half
  store half %conv.i.i.i.i.i26.i.i.i30.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  store half %conv.i.i.i.i.i69.i.i.i59.i.i.i, ptr addrspace(3) null, align 2
  store half %conv.i.i.i.i.i78.i.i.i69.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  store half %conv.i.i.i.i.i150.i.i.i108.i.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.131.136.vec.extract.i.i = extractelement <4 x float> %268, i64 2
  %conv.i.i.i.i.i14.i.i.i217.i.i.i = fptrunc float %c_thread_buf.sroa.131.136.vec.extract.i.i to half
  store half %conv.i.i.i.i.i14.i.i.i217.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.147.144.vec.extract.i.i = extractelement <4 x float> %312, i64 0
  %conv.i.i.i.i.i69.i.i.i254.i.i.i = fptrunc float %c_thread_buf.sroa.147.144.vec.extract.i.i to half
  store half %conv.i.i.i.i.i69.i.i.i254.i.i.i, ptr addrspace(3) %arrayidx.i.i.i73.i.i.i.i.i.i, align 2
  %c_thread_buf.sroa.179.176.vec.extract.i.i = extractelement <4 x float> %313, i64 0
  %conv.i.i.i.i.i78.i.i.i264.i.i.i = fptrunc float %c_thread_buf.sroa.179.176.vec.extract.i.i to half
  store half %conv.i.i.i.i.i78.i.i.i264.i.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.163.172.vec.extract.i.i = extractelement <4 x float> %270, i64 0
  %conv.i.i.i.i.i126.i.i.i291.i.i.i = fptrunc float %c_thread_buf.sroa.163.172.vec.extract.i.i to half
  store half %conv.i.i.i.i.i126.i.i.i291.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.195.204.vec.extract.i.i = extractelement <4 x float> %276, i64 3
  %conv.i.i.i.i.i26.i.i.i420.i.i.i = fptrunc float %c_thread_buf.sroa.195.204.vec.extract.i.i to half
  %c_thread_buf.sroa.211.208.vec.extract.i.i = extractelement <4 x float> %314, i64 0
  %conv.i.i.i.i.i69.i.i.i449.i.i.i = fptrunc float %c_thread_buf.sroa.211.208.vec.extract.i.i to half
  %c_thread_buf.sroa.243.240.vec.extract.i.i = extractelement <4 x float> %277, i64 0
  %conv.i.i.i.i.i78.i.i.i459.i.i.i = fptrunc float %c_thread_buf.sroa.243.240.vec.extract.i.i to half
  %c_thread_buf.sroa.227.232.vec.extract.i.i = extractelement <4 x float> %315, i64 0
  %conv.i.i.i.i.i138.i.i.i492.i.i.i = fptrunc float %c_thread_buf.sroa.227.232.vec.extract.i.i to half
  %c_thread_buf.sroa.451.460.vec.extract.i.i = extractelement <4 x float> %295, i64 0
  %conv.i.i.i.i.i26.i.i.i607.i.i.i = fptrunc float %c_thread_buf.sroa.451.460.vec.extract.i.i to half
  %c_thread_buf.sroa.467.464.vec.extract.i.i = extractelement <4 x float> %296, i64 0
  %conv.i.i.i.i.i69.i.i.i636.i.i.i = fptrunc float %c_thread_buf.sroa.467.464.vec.extract.i.i to half
  %c_thread_buf.sroa.499.496.vec.extract.i.i = extractelement <4 x float> %298, i64 0
  %conv.i.i.i.i.i78.i.i.i646.i.i.i = fptrunc float %c_thread_buf.sroa.499.496.vec.extract.i.i to half
  %c_thread_buf.sroa.483.492.vec.extract.i.i = extractelement <4 x float> %297, i64 0
  %conv.i.i.i.i.i126.i.i.i673.i.i.i = fptrunc float %c_thread_buf.sroa.483.492.vec.extract.i.i to half
  store half %conv.i.i.i.i.i26.i.i.i420.i.i.i, ptr addrspace(3) %9, align 2
  store half %conv.i.i.i.i.i69.i.i.i449.i.i.i, ptr addrspace(3) null, align 2
  store half %conv.i.i.i.i.i78.i.i.i459.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i273.i.i.i.i.i.i.i.i.i1383.i.i.i, align 2
  store half %conv.i.i.i.i.i138.i.i.i492.i.i.i, ptr addrspace(3) null, align 2
  store half %conv.i.i.i.i.i26.i.i.i607.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i85.i131.i.i.i.i.i, align 2
  store half %conv.i.i.i.i.i69.i.i.i636.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  store half %conv.i.i.i.i.i78.i.i.i646.i.i.i, ptr addrspace(3) null, align 2
  store half %conv.i.i.i.i.i126.i.i.i673.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.387.388.vec.extract.i.i = extractelement <4 x float> %292, i64 0
  %conv.i.i.i.i.i2.i.i.i786.i.i.i = fptrunc float %c_thread_buf.sroa.387.388.vec.extract.i.i to half
  store half %conv.i.i.i.i.i2.i.i.i786.i.i.i, ptr addrspace(3) %arrayidx.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i109.i.i.i.i.i.i, align 2
  %c_thread_buf.sroa.403.400.vec.extract.i.i = extractelement <4 x float> %322, i64 0
  %conv.i.i.i.i.i69.i.i.i831.i.i.i = fptrunc float %c_thread_buf.sroa.403.400.vec.extract.i.i to half
  store half %conv.i.i.i.i.i69.i.i.i831.i.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.435.432.vec.extract.i.i = extractelement <4 x float> %294, i64 0
  %conv.i.i.i.i.i78.i.i.i841.i.i.i = fptrunc float %c_thread_buf.sroa.435.432.vec.extract.i.i to half
  store half %conv.i.i.i.i.i78.i.i.i841.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.419.428.vec.extract.i.i = extractelement <4 x float> %293, i64 0
  %conv.i.i.i.i.i126.i.i.i868.i.i.i = fptrunc float %c_thread_buf.sroa.419.428.vec.extract.i.i to half
  store half %conv.i.i.i.i.i126.i.i.i868.i.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.323.328.vec.extract.i.i = extractelement <4 x float> %320, i64 0
  %conv.i.i.i.i.i14.i.i.i989.i.i.i = fptrunc float %c_thread_buf.sroa.323.328.vec.extract.i.i to half
  store half %conv.i.i.i.i.i14.i.i.i989.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.339.336.vec.extract.i.i = extractelement <4 x float> %288, i64 0
  %conv.i.i.i.i.i69.i.i.i1026.i.i.i = fptrunc float %c_thread_buf.sroa.339.336.vec.extract.i.i to half
  store half %conv.i.i.i.i.i69.i.i.i1026.i.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.371.368.vec.extract.i.i = extractelement <4 x float> %291, i64 0
  %conv.i.i.i.i.i78.i.i.i1036.i.i.i = fptrunc float %c_thread_buf.sroa.371.368.vec.extract.i.i to half
  store half %conv.i.i.i.i.i78.i.i.i1036.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.355.356.vec.extract.i.i = extractelement <4 x float> %321, i64 0
  %conv.i.i.i.i.i150.i.i.i1075.i.i.i = fptrunc float %c_thread_buf.sroa.355.356.vec.extract.i.i to half
  store half %conv.i.i.i.i.i150.i.i.i1075.i.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.259.260.vec.extract.i.i = extractelement <4 x float> %279, i64 0
  %conv.i.i.i.i.i2.i.i.i1176.i.i.i = fptrunc float %c_thread_buf.sroa.259.260.vec.extract.i.i to half
  store half %conv.i.i.i.i.i2.i.i.i1176.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  %c_thread_buf.sroa.259.264.vec.extract.i.i = extractelement <4 x float> %317, i64 0
  %conv.i.i.i.i.i14.i.i.i1184.i.i.i = fptrunc float %c_thread_buf.sroa.259.264.vec.extract.i.i to half
  store half %conv.i.i.i.i.i14.i.i.i1184.i.i.i, ptr addrspace(3) null, align 2
  %c_thread_buf.sroa.275.272.vec.extract.i.i = extractelement <4 x float> %280, i64 0
  %conv.i.i.i.i.i69.i.i.i1221.i.i.i = fptrunc float %c_thread_buf.sroa.275.272.vec.extract.i.i to half
  store half %conv.i.i.i.i.i69.i.i.i1221.i.i.i, ptr addrspace(3) %9, align 2
  %c_thread_buf.sroa.307.304.vec.extract.i.i = extractelement <4 x float> %319, i64 0
  %conv.i.i.i.i.i78.i.i.i1231.i.i.i = fptrunc float %c_thread_buf.sroa.307.304.vec.extract.i.i to half
  store half %conv.i.i.i.i.i78.i.i.i1231.i.i.i, ptr addrspace(3) %_ZZN2ck27kernel_gemm_xdl_cshuffle_v3INS_30GridwiseGemmMX_xdl_cshuffle_v3INS_13tensor_layout4gemm8RowMajorENS3_11ColumnMajorES4_NS_9f4x2_pk_tENS_11e8m0_bexp_tES6_S7_fDF16_DF16_NS_16tensor_operation12element_wise11PassThroughESA_SA_LNS8_6device18GemmSpecializationE0ELi32ELi256ELi128ELi256ELi256ELi32ELi32ELi16ELi16ELi4ELi8ENS_8SequenceIJLi8ELi32ELi1EEEENSD_IJLi1ELi0ELi2EEEESF_Li2ELi32ELi32ELb0ELi0ESE_SF_SF_Li2ELi32ELi32ELb0ELi0ELi2ELi2ENSD_IJLi1ELi32ELi1ELi8EEEELi8ELNS_26BlockGemmPipelineSchedulerE0ELNS_24BlockGemmPipelineVersionE2ES6_S6_Lb0ELb0EEELb1ELNS_25InMemoryDataOperationEnumE0ELi1ELNS_10TailNumberE1EEEvNT_8ArgumentEE8p_shared, align 2
  store half 0xH0000, ptr addrspace(3) null, align 2
  ret void
}

; Function Attrs: convergent nocallback nofree nounwind willreturn memory(none)
declare i32 @llvm.amdgcn.readfirstlane.i32(i32) #1

; Function Attrs: convergent nocallback nofree nounwind willreturn
declare void @llvm.amdgcn.sched.barrier(i32 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(read)
declare <4 x i32> @llvm.amdgcn.raw.buffer.load.v4i32(<4 x i32>, i32, i32, i32 immarg) #3

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(read)
declare i32 @llvm.amdgcn.raw.buffer.load.i32(<4 x i32>, i32, i32, i32 immarg) #3

; Function Attrs: convergent nocallback nofree nounwind willreturn
declare void @llvm.amdgcn.sched.group.barrier(i32 immarg, i32 immarg, i32 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare noundef range(i32 0, 1024) i32 @llvm.amdgcn.workitem.id.x() #4

; Function Attrs: convergent nocallback nofree nosync nounwind willreturn memory(none)
declare <4 x float> @llvm.amdgcn.mfma.scale.f32.16x16x128.f8f6f4.v4i32.v4i32(<4 x i32>, <4 x i32>, <4 x float>, i32 immarg, i32 immarg, i32 immarg, i32, i32 immarg, i32) #5

attributes #0 = { "amdgpu-flat-work-group-size"="1,256" }
attributes #1 = { convergent nocallback nofree nounwind willreturn memory(none) }
attributes #2 = { convergent nocallback nofree nounwind willreturn }
attributes #3 = { nocallback nofree nosync nounwind willreturn memory(read) }
attributes #4 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #5 = { convergent nocallback nofree nosync nounwind willreturn memory(none) }
