; RUN: llc -O3 -mtriple=amdgcn-amd-amdhsa -mcpu=gfx1201 %s -o - | FileCheck %s
; CHECK: Occupancy: 16

%struct.CIntersectionData.26.38.62.146.157.294.312.816.1410.2265.15360.15392.15432.15440.15448.15464.15480.15780.16380.16398.16422.16440.16458.16464.16476.16494.16536.16542.16554.16572.16590 = type { i32, i32, float, float, i32, i32, i32 }
%struct.HIP_vector_type.30.42.66.150.161.298.316.820.1414.2269.15364.15396.15436.15444.15452.15468.15484.15782.16382.16400.16424.16442.16460.16466.16478.16496.16538.16544.16556.16574.16592 = type { %struct.HIP_vector_base.29.41.65.149.160.297.315.819.1413.2268.15363.15395.15435.15443.15451.15467.15483.15781.16381.16399.16423.16441.16459.16465.16477.16495.16537.16543.16555.16573.16591 }
%struct.HIP_vector_base.29.41.65.149.160.297.315.819.1413.2268.15363.15395.15435.15443.15451.15467.15483.15781.16381.16399.16423.16441.16459.16465.16477.16495.16537.16543.16555.16573.16591 = type { float, float, float, float }
%struct.RSArrayTextureObject1D.31.43.67.151.162.299.317.821.1415.2270.15365.15397.15437.15445.15453.15469.15485.15783.16383.16401.16425.16443.16461.16467.16479.16497.16539.16545.16557.16575.16593 = type { ptr }
%struct.CTextureHWSampler.33.45.69.153.164.301.319.823.1417.2272.15367.15399.15439.15447.15455.15471.15487.15785.16385.16403.16427.16445.16463.16469.16481.16499.16541.16547.16559.16577.16595 = type { %struct.RSTextureObject2D.32.44.68.152.163.300.318.822.1416.2271.15366.15398.15438.15446.15454.15470.15486.15784.16384.16402.16426.16444.16462.16468.16480.16498.16540.16546.16558.16576.16594 }
%struct.RSTextureObject2D.32.44.68.152.163.300.318.822.1416.2271.15366.15398.15438.15446.15454.15470.15486.15784.16384.16402.16426.16444.16462.16468.16480.16498.16540.16546.16558.16576.16594 = type { ptr }

define amdgpu_kernel void @the_kernel(i32 %0, i32 %rem.i925, i32 %notmask.i, i8 %coerce.sroa.9.0.copyload, i32 %coerce.sroa.11788.0.copyload, i32 %and120.i, i1 %cmp121.i, i1 %loadedv89.i, ptr addrspace(1) %1, i32 %traceSetPaletteOffset.2.i, i1 %hasAlreadyBeenQueuedToUnfinishedList.3.off0.i, i32 %2, i1 %3, i1 %.not, i1 %cmp16.i, i1 %brmerge.not, ptr addrspace(1) %4, ptr addrspace(1) %5, ptr addrspace(1) %arrayidx107.i, ptr addrspace(1) %6, ptr addrspace(1) %arrayidx114.i, ptr addrspace(1) %7, ptr addrspace(1) %arrayidx238.i, ptr addrspace(1) %8, ptr addrspace(1) %arrayidx256.i, ptr addrspace(1) %9, ptr addrspace(1) %10, ptr addrspace(1) %arrayidx274.i, ptr addrspace(1) %11, ptr addrspace(1) %arrayidx281.i, ptr addrspace(1) %12, ptr addrspace(1) %arrayidx289.i, float %13, float %spec.select4288.i, float %RAYDATA_CLOSESTINTERSECTION.i.10, float %RAYDATA_CLOSESTINTERSECTION.i.11, float %14, ptr addrspace(1) %15, ptr addrspace(1) %arrayidx607.i, ptr addrspace(1) %origin604.sroa.4.0.arrayidx607.sroa_idx.i, <4 x i32> %16, float %17, <4 x i32> %18, ptr addrspace(1) %19, ptr addrspace(1) %20, i1 %cmp.i10516.i) {
entry:
  %21 = tail call i32 @llvm.amdgcn.workitem.id.x()
  %arrayidx10.i = getelementptr %struct.CIntersectionData.26.38.62.146.157.294.312.816.1410.2265.15360.15392.15432.15440.15448.15464.15480.15780.16380.16398.16422.16440.16458.16464.16476.16494.16536.16542.16554.16572.16590, ptr addrspace(3) null, i32 %21
  %22 = addrspacecast ptr addrspace(3) %arrayidx10.i to ptr
  %arrayidx24.i = getelementptr i8, ptr addrspace(3) null, i32 %21
  %23 = addrspacecast ptr addrspace(3) %arrayidx24.i to ptr
  %arrayidx50.i = getelementptr i32, ptr addrspace(3) null, i32 %21
  %24 = addrspacecast ptr addrspace(3) %arrayidx50.i to ptr
  %instanceID.i = getelementptr i8, ptr addrspace(3) %arrayidx10.i, i32 20
  %25 = addrspacecast ptr addrspace(3) %instanceID.i to ptr
  %primID900.i = getelementptr i8, ptr addrspace(3) %arrayidx10.i, i32 4
  %26 = addrspacecast ptr addrspace(3) %primID900.i to ptr
  %matID_relID901.i = getelementptr i8, ptr addrspace(3) %arrayidx10.i, i32 16
  %27 = addrspacecast ptr addrspace(3) %matID_relID901.i to ptr
  %barycentricV1469.i = getelementptr i8, ptr addrspace(3) %arrayidx10.i, i32 12
  %28 = addrspacecast ptr addrspace(3) %barycentricV1469.i to ptr
  %barycentricU_or_hairM1468.i = getelementptr i8, ptr addrspace(3) %arrayidx10.i, i32 8
  %29 = addrspacecast ptr addrspace(3) %barycentricU_or_hairM1468.i to ptr
  %booleanMeshID.i = getelementptr i8, ptr addrspace(3) %arrayidx10.i, i32 24
  %30 = addrspacecast ptr addrspace(3) %booleanMeshID.i to ptr
  br label %while.cond.i

while.cond.i:                                     ; preds = %if.end1706.i, %entry
  %v.i.0 = phi float [ 0.000000e+00, %entry ], [ %v.i.2, %if.end1706.i ]
  %t1122.i.052 = phi float [ 0.000000e+00, %entry ], [ %spec.select4288.i, %if.end1706.i ]
  %time.0.i = phi float [ 0.000000e+00, %entry ], [ %time.2.i, %if.end1706.i ]
  %materialCullingMask.0.i = phi i32 [ 0, %entry ], [ %materialCullingMask.8.i, %if.end1706.i ]
  br i1 %.not, label %_Z13__ballot_syncIyEyT_i.exit, label %do.body13.i

do.body13.i:                                      ; preds = %while.cond.i
  %31 = tail call i32 @llvm.amdgcn.ballot.i32(i1 false)
  %cmp15.i = icmp eq i32 0, %31
  ret void

_Z13__ballot_syncIyEyT_i.exit:                    ; preds = %while.cond.i
  br i1 %cmp16.i, label %if.then17.i, label %if.end352.i

if.then17.i:                                      ; preds = %_Z13__ballot_syncIyEyT_i.exit
  br i1 %3, label %land.lhs.true.i, label %if.end28.i

land.lhs.true.i:                                  ; preds = %if.then17.i
  %32 = load i8, ptr %23, align 1
  %loadedv.i = trunc i8 %32 to i1
  br i1 %loadedv.i, label %_ZL5TracePV17CIntersectionDataPVbPVibbbjbRK17RayTraceArguments.exit, label %if.end28.i

if.end28.i:                                       ; preds = %land.lhs.true.i, %if.then17.i
  br i1 %brmerge.not, label %if.then44.i, label %if.end65.i

if.then44.i:                                      ; preds = %if.end28.i
  %33 = atomicrmw add ptr addrspace(1) %4, i32 0 monotonic, align 4
  store i8 1, ptr null, align 1
  br label %if.end65.i

if.end65.i:                                       ; preds = %if.then44.i, %if.end28.i
  br i1 %cmp121.i, label %if.then72.i, label %if.end352.i

if.then72.i:                                      ; preds = %if.end65.i
  %34 = load volatile i32, ptr %24, align 4
  %35 = load i32, ptr addrspace(1) %12, align 4
  %36 = load i32, ptr addrspace(1) %arrayidx114.i, align 4
  %cmp117.i = icmp eq i32 %36, 0
  %materialCullingMask.1.i = select i1 %cmp117.i, i32 %traceSetPaletteOffset.2.i, i32 %0
  %37 = load i32, ptr addrspace(1) %arrayidx238.i, align 8
  %spec.select4286.i = select i1 %loadedv89.i, i32 0, i32 %materialCullingMask.1.i
  %cmp251.i = icmp ult i32 %37, %2
  %spec.select4287.i = select i1 %cmp251.i, i32 0, i32 %spec.select4286.i
  store i32 0, ptr addrspace(1) %arrayidx274.i, align 4
  store float 1.000000e+00, ptr addrspace(1) null, align 4
  store i32 -1, ptr addrspace(1) %arrayidx107.i, align 4
  %cmp342.i82 = fcmp ult float %13, 0.000000e+00
  %stackDepth.1.i = select i1 %cmp342.i82, i32 1, i32 0
  br label %if.end352.i

if.end352.i:                                      ; preds = %if.then72.i, %if.end65.i, %_Z13__ballot_syncIyEyT_i.exit
  %currentNodeAddr.i.1 = phi i32 [ %coerce.sroa.11788.0.copyload, %if.then72.i ], [ 0, %if.end65.i ], [ 0, %_Z13__ballot_syncIyEyT_i.exit ]
  %RAYDATA_CLOSESTINTERSECTION.i.1 = phi float [ %RAYDATA_CLOSESTINTERSECTION.i.11, %if.then72.i ], [ 0.000000e+00, %if.end65.i ], [ 0.000000e+00, %_Z13__ballot_syncIyEyT_i.exit ]
  %time.2.i = phi float [ %13, %if.then72.i ], [ 0.000000e+00, %if.end65.i ], [ 0.000000e+00, %_Z13__ballot_syncIyEyT_i.exit ]
  %materialCullingMask.8.i = phi i32 [ %spec.select4287.i, %if.then72.i ], [ 0, %if.end65.i ], [ 0, %_Z13__ballot_syncIyEyT_i.exit ]
  %stackDepth.3.i86 = phi i32 [ %stackDepth.1.i, %if.then72.i ], [ 0, %if.end65.i ], [ 0, %_Z13__ballot_syncIyEyT_i.exit ]
  %dirY.2.i = phi float [ %14, %if.then72.i ], [ 0.000000e+00, %if.end65.i ], [ 0.000000e+00, %_Z13__ballot_syncIyEyT_i.exit ]
  %originY.2.i = phi float [ 1.000000e+00, %if.then72.i ], [ 0.000000e+00, %if.end65.i ], [ 0.000000e+00, %_Z13__ballot_syncIyEyT_i.exit ]
  %rayIDMOD.2.i = phi i32 [ %and120.i, %if.then72.i ], [ 0, %if.end65.i ], [ 0, %_Z13__ballot_syncIyEyT_i.exit ]
  %rayID.3.i = phi i32 [ %35, %if.then72.i ], [ 0, %if.end65.i ], [ 0, %_Z13__ballot_syncIyEyT_i.exit ]
  %mul372.i = fmul float %originY.2.i, %dirY.2.i
  %cmp374.i = icmp sgt i32 %stackDepth.3.i86, 0
  br i1 %cmp374.i, label %while.body380.i, label %while.end.i

while.body380.i:                                  ; preds = %while.body380.i, %if.end352.i
  %and383.i = and i32 %currentNodeAddr.i.1, %notmask.i
  %cmp384.i = icmp eq i32 %and383.i, 0
  %cmp460.i = fcmp oge float %mul372.i, 0.000000e+00
  %narrow.i88 = select i1 %cmp384.i, i1 %cmp460.i, i1 false
  %cmp463.i = fcmp oge float %RAYDATA_CLOSESTINTERSECTION.i.1, 0.000000e+00
  %38 = select i1 %narrow.i88, i1 false, i1 %cmp463.i
  br i1 %38, label %if.else475.i, label %while.body380.i

if.else475.i:                                     ; preds = %while.body380.i
  ret void

while.end.i:                                      ; preds = %if.end352.i
  %idxprom606.i93 = sext i32 %rayIDMOD.2.i to i64
  %arrayidx607.i94 = getelementptr %struct.HIP_vector_type.30.42.66.150.161.298.316.820.1414.2269.15364.15396.15436.15444.15452.15468.15484.15782.16382.16400.16424.16442.16460.16466.16478.16496.16538.16544.16556.16574.16592, ptr addrspace(1) %15, i64 %idxprom606.i93
  %origin604.sroa.0.0.copyload.i = load float, ptr addrspace(1) %arrayidx607.i94, align 16
  %origin604.sroa.4.0.copyload.i = load float, ptr addrspace(1) %7, align 4
  %origin604.sroa.5.0.copyload.i = load float, ptr addrspace(1) %5, align 8
  %dir611.sroa.0.0.copyload.i = load float, ptr addrspace(1) %6, align 16
  %dir611.sroa.4.0.copyload.i = load float, ptr addrspace(1) %origin604.sroa.4.0.arrayidx607.sroa_idx.i, align 4
  %dir611.sroa.5.0.copyload.i = load float, ptr addrspace(1) null, align 8
  %39 = load i32, ptr addrspace(5) null, align 4
  %and778.i = and i32 %39, 1
  %idxprom782.i = zext i32 %39 to i64
  %arrayidx783.i = getelementptr %struct.RSArrayTextureObject1D.31.43.67.151.162.299.317.821.1415.2270.15365.15397.15437.15445.15453.15469.15485.15783.16383.16401.16425.16443.16461.16467.16479.16497.16539.16545.16557.16575.16593, ptr addrspace(4) null, i64 %idxprom782.i
  %40 = inttoptr i64 %idxprom782.i to ptr
  %41 = load i16, ptr addrspace(1) %arrayidx607.i, align 4
  %cmp.i10516.i1 = icmp slt i16 %41, 0
  %42 = load i32, ptr addrspace(1) %10, align 4
  %conv803.i = fptoui float %time.0.i to i32
  br i1 %cmp121.i, label %if.then1007.i, label %while.body1268.i.lr.ph

if.then1007.i:                                    ; preds = %while.end.i
  %43 = tail call float @llvm.sqrt.f32(float %17)
  %cmp.i210 = fcmp oeq float %43, 0.000000e+00
  %t1122.i.4 = select i1 %cmp.i210, float %t1122.i.052, float 0.000000e+00
  %cmp.i235 = fcmp olt float 0.000000e+00, %time.0.i
  %cmp7.i = fcmp olt float %t1122.i.4, 0.000000e+00
  %or.cond928 = select i1 %cmp.i235, i1 %cmp7.i, i1 false
  %v.i.5 = select i1 %or.cond928, float 0.000000e+00, float %v.i.0
  store i32 0, ptr %25, align 4
  br label %if.end1622.i

while.body1268.i.lr.ph:                           ; preds = %while.end.i
  %44 = addrspacecast ptr %40 to ptr addrspace(4)
  %45 = load <4 x i32>, ptr addrspace(4) %44, align 16
  %cmp1465.i = icmp eq i32 %materialCullingMask.0.i, 0
  br label %while.body1268.i

while.body1268.i:                                 ; preds = %if.end1616.i, %while.body1268.i.lr.ph
  %uv0.sroa.0.0.copyload.i = phi float [ %dir611.sroa.5.0.copyload.i, %while.body1268.i.lr.ph ], [ 0.000000e+00, %if.end1616.i ]
  %46 = phi i32 [ 0, %while.body1268.i.lr.ph ], [ %56, %if.end1616.i ]
  %mul1277.i = mul i32 %46, %42
  %add1279.i = or i32 %conv803.i, %mul1277.i
  %47 = tail call <3 x float> @llvm.amdgcn.struct.buffer.load.format.v3f32(<4 x i32> %45, i32 %add1279.i, i32 0, i32 0, i32 0)
  %retval.sroa.0.4.vec.extract.i560 = extractelement <3 x float> %47, i64 0
  %mul.i135.i = fmul float %uv0.sroa.0.0.copyload.i, %dir611.sroa.5.0.copyload.i
  %48 = tail call float @llvm.fma.f32(float %dir611.sroa.4.0.copyload.i, float 0.000000e+00, float %mul.i135.i)
  %49 = tail call float @llvm.fma.f32(float %dir611.sroa.0.0.copyload.i, float %retval.sroa.0.4.vec.extract.i560, float 0.000000e+00)
  %mul28.i = fmul float %48, %origin604.sroa.0.0.copyload.i
  %mul31.i149 = fmul float %origin604.sroa.5.0.copyload.i, %49
  %add30.i = fadd float %mul28.i, %mul31.i149
  %add32.i = fadd float %add30.i, %origin604.sroa.4.0.copyload.i
  %u.i.5 = select i1 %cmp.i10516.i, float 0.000000e+00, float %add32.i
  %v1354.i.7 = select i1 %cmp.i10516.i1, float 1.000000e+00, float 0.000000e+00
  br i1 %hasAlreadyBeenQueuedToUnfinishedList.3.off0.i, label %if.then1457.i, label %if.then1401.i

if.then1401.i:                                    ; preds = %while.body1268.i
  %uv0.sroa.4.0.copyload.i = load float, ptr addrspace(1) inttoptr (i64 4 to ptr addrspace(1)), align 4
  %uv1.sroa.0.0.copyload.i = load float, ptr addrspace(1) inttoptr (i64 8 to ptr addrspace(1)), align 8
  %uv1.sroa.4.0.copyload.i = load float, ptr addrspace(1) inttoptr (i64 12 to ptr addrspace(1)), align 4
  %uv2.sroa.0.0.copyload.i = load float, ptr addrspace(1) inttoptr (i64 16 to ptr addrspace(1)), align 8
  %uv2.sroa.4.0.copyload.i = load float, ptr addrspace(1) inttoptr (i64 20 to ptr addrspace(1)), align 4
  %mul1406.i = fmul float %uv1.sroa.0.0.copyload.i, %u.i.5
  %mul1408.i = fmul float %uv2.sroa.0.0.copyload.i, %v1354.i.7
  %add1415.i = fadd float %mul1406.i, %mul1408.i
  %add1421.i = fadd float %uv1.sroa.4.0.copyload.i, %uv0.sroa.4.0.copyload.i
  %add1427.i = fadd float %add1421.i, %uv2.sroa.4.0.copyload.i
  %50 = tail call <4 x float> @llvm.amdgcn.struct.buffer.load.format.v4f32(<4 x i32> %16, i32 0, i32 0, i32 0, i32 0)
  %retval.sroa.0.8.vec.extract.i538 = extractelement <4 x float> %50, i64 0
  %mul1436.i = fmul float %RAYDATA_CLOSESTINTERSECTION.i.10, %add1427.i
  %arrayidx1448.i = getelementptr %struct.CTextureHWSampler.33.45.69.153.164.301.319.823.1417.2272.15367.15399.15439.15447.15455.15471.15487.15785.16385.16403.16427.16445.16463.16469.16481.16499.16541.16547.16559.16577.16595, ptr addrspace(1) %19, i64 %idxprom782.i
  %51 = load ptr, ptr addrspace(1) %arrayidx1448.i, align 8
  %52 = addrspacecast ptr %51 to ptr addrspace(4)
  %53 = load <4 x i32>, ptr addrspace(4) %arrayidx783.i, align 16
  %54 = load <8 x i32>, ptr addrspace(4) %52, align 32
  %55 = tail call float @llvm.amdgcn.image.sample.lz.2d.f32.f32.v8i32.v4i32(i32 1, float %add1415.i, float %mul1436.i, <8 x i32> %54, <4 x i32> %53, i1 false, i32 0, i32 0)
  %cmp1453.i = fcmp ogt float %55, %retval.sroa.0.8.vec.extract.i538
  br i1 %cmp1453.i, label %if.then1457.i, label %if.end1616.i

if.then1457.i:                                    ; preds = %if.then1401.i, %while.body1268.i
  br i1 %cmp1465.i, label %if.then1466.i, label %if.end1477.i

if.then1466.i:                                    ; preds = %if.then1457.i
  %cond1475.i = select i1 %loadedv89.i, i32 %and778.i, i32 0
  store float 0.000000e+00, ptr %29, align 4
  store float 0.000000e+00, ptr %28, align 4
  store i32 %cond1475.i, ptr %27, align 4
  br label %if.end1477.i

if.end1477.i:                                     ; preds = %if.then1466.i, %if.then1457.i
  store i32 %39, ptr %22, align 4
  br label %if.end1616.i

if.end1616.i:                                     ; preds = %if.end1477.i, %if.then1401.i
  %56 = phi i32 [ 0, %if.end1477.i ], [ 1, %if.then1401.i ]
  br i1 %loadedv89.i, label %while.body1268.i, label %if.end1622.i

if.end1622.i:                                     ; preds = %if.end1616.i, %if.then1007.i
  %v.i.2 = phi float [ %v.i.5, %if.then1007.i ], [ %v.i.0, %if.end1616.i ]
  br i1 %brmerge.not, label %if.end1706.i, label %if.then1699.i

if.then1699.i:                                    ; preds = %if.end1622.i
  store i8 0, ptr addrspace(1) %11, align 1
  %57 = atomicrmw add ptr addrspace(1) %8, i32 0 monotonic, align 4
  store i32 %rayID.3.i, ptr addrspace(1) %arrayidx281.i, align 4
  br label %if.end1706.i

if.end1706.i:                                     ; preds = %if.then1699.i, %if.end1622.i
  store i32 536870911, ptr %30, align 4
  %58 = load volatile i32, ptr %26, align 4
  %59 = tail call float @llvm.amdgcn.struct.buffer.load.format.f32(<4 x i32> %18, i32 0, i32 0, i32 0, i32 0)
  %60 = bitcast float %59 to i32
  store i32 0, ptr addrspace(1) %9, align 16
  store i32 %60, ptr addrspace(1) null, align 4
  %arrayidx33.i8487.i = getelementptr i32, ptr addrspace(1) %20, i64 %idxprom606.i93
  store i32 0, ptr addrspace(1) %arrayidx33.i8487.i, align 4
  store i32 -2147483648, ptr addrspace(1) %arrayidx256.i, align 4
  store i32 0, ptr addrspace(1) %arrayidx289.i, align 16
  %arrayidx430.i9729.i = getelementptr %struct.HIP_vector_type.30.42.66.150.161.298.316.820.1414.2269.15364.15396.15436.15444.15452.15468.15484.15782.16382.16400.16424.16442.16460.16466.16478.16496.16538.16544.16556.16574.16592, ptr addrspace(1) %1, i64 %idxprom606.i93
  store float 0.000000e+00, ptr addrspace(1) %arrayidx430.i9729.i, align 16
  br label %while.cond.i

_ZL5TracePV17CIntersectionDataPVbPVibbbjbRK17RayTraceArguments.exit: ; preds = %land.lhs.true.i
  ret void
}

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.fma.f32(float, float, float) #0

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.sqrt.f32(float) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare noundef range(i32 0, 1024) i32 @llvm.amdgcn.workitem.id.x() #1

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(read)
declare <4 x float> @llvm.amdgcn.struct.buffer.load.format.v4f32(<4 x i32>, i32, i32, i32, i32 immarg) #2

; Function Attrs: convergent nocallback nofree nounwind willreturn memory(none)
declare i32 @llvm.amdgcn.ballot.i32(i1) #3

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(read)
declare float @llvm.amdgcn.struct.buffer.load.format.f32(<4 x i32>, i32, i32, i32, i32 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(read)
declare <3 x float> @llvm.amdgcn.struct.buffer.load.format.v3f32(<4 x i32>, i32, i32, i32, i32 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(read)
declare float @llvm.amdgcn.image.sample.lz.2d.f32.f32.v8i32.v4i32(i32 immarg, float, float, <8 x i32>, <4 x i32>, i1 immarg, i32 immarg, i32 immarg) #2

; uselistorder directives
uselistorder ptr @llvm.fma.f32, { 1, 0 }

attributes #0 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nocallback nofree nosync nounwind willreturn memory(read) }
attributes #3 = { convergent nocallback nofree nounwind willreturn memory(none) }
