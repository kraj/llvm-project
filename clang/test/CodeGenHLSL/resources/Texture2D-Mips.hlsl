// RUN: %clang_cc1 -triple dxil-pc-shadermodel6.0-pixel -x hlsl -emit-llvm -disable-llvm-passes -finclude-default-header -o - %s | llvm-cxxfilt | FileCheck %s --check-prefixes=CHECK,DXIL

Texture2D<float4> t;

// CHECK: define hidden noundef nofpclass(nan inf) <4 x float> @test_mips(float vector[2])(<2 x float> noundef nofpclass(nan inf) %loc) #1 {
// CHECK: entry:
// CHECK: %[[LOC_ADDR:.*]] = alloca <2 x float>, align 8
// CHECK: %[[REF_TMP:.*]] = alloca %"struct.hlsl::Texture2D<>::mips_slice_type", align 4
// CHECK: store <2 x float> %loc, ptr %[[LOC_ADDR]], align 8
// CHECK: call void @hlsl::Texture2D<float vector[4]>::mips_type::operator[](int) const(ptr dead_on_unwind writable sret(%"struct.hlsl::Texture2D<>::mips_slice_type") align 4 %[[REF_TMP]], ptr noundef nonnull align 4 dereferenceable(4) getelementptr inbounds nuw (%"class.hlsl::Texture2D", ptr @t, i32 0, i32 1), i32 noundef 0)
// CHECK: %[[V0:.*]] = load <2 x float>, ptr %[[LOC_ADDR]], align 8
// CHECK: %[[CONV:.*]] = fptosi <2 x float> %[[V0]] to <2 x i32>
// CHECK: %[[CALL:.*]] = call reassoc nnan ninf nsz arcp afn noundef nofpclass(nan inf) <4 x float> @hlsl::Texture2D<float vector[4]>::mips_slice_type::operator[](int vector[2]) const(ptr noundef nonnull align 4 dereferenceable(8) %[[REF_TMP]], <2 x i32> noundef %[[CONV]])
// CHECK: ret <4 x float> %[[CALL]]

[shader("pixel")]
float4 test_mips(float2 loc : LOC) : SV_Target {
  return t.mips[0][int2(loc)];
}

// CHECK: define linkonce_odr hidden void @hlsl::Texture2D<float vector[4]>::mips_type::operator[](int) const(ptr dead_on_unwind noalias writable sret(%"struct.hlsl::Texture2D<>::mips_slice_type") align 4 %agg.result, ptr noundef nonnull align 4 dereferenceable(4) %this, i32 noundef %Level)
// CHECK: entry:
// CHECK: %[[SLICE:.*]] = alloca %"struct.hlsl::Texture2D<>::mips_slice_type", align 4
// CHECK: %[[THIS1:.*]] = load ptr, ptr %{{.*}}, align 4
// CHECK: call void @hlsl::Texture2D<float vector[4]>::mips_slice_type::mips_slice_type()(ptr noundef nonnull align 4 dereferenceable(8) %[[SLICE]])
// CHECK: %[[HANDLE_GEP:.*]] = getelementptr inbounds nuw %"struct.hlsl::Texture2D<>::mips_type", ptr %[[THIS1]], i32 0, i32 0
// CHECK: %[[HANDLE:.*]] = load target("dx.Texture", <4 x float>, 0, 0, 0, 2), ptr %[[HANDLE_GEP]], align 4
// CHECK: %[[HANDLE_GEP2:.*]] = getelementptr inbounds nuw %"struct.hlsl::Texture2D<>::mips_slice_type", ptr %[[SLICE]], i32 0, i32 0
// CHECK: store target("dx.Texture", <4 x float>, 0, 0, 0, 2) %[[HANDLE]], ptr %[[HANDLE_GEP2]], align 4
// CHECK: %[[L_VAL:.*]] = load i32, ptr %{{.*}}, align 4
// CHECK: %[[LEVEL_GEP:.*]] = getelementptr inbounds nuw %"struct.hlsl::Texture2D<>::mips_slice_type", ptr %[[SLICE]], i32 0, i32 1
// CHECK: store i32 %[[L_VAL]], ptr %[[LEVEL_GEP]], align 4
// CHECK: call void @hlsl::Texture2D<float vector[4]>::mips_slice_type::mips_slice_type(hlsl::Texture2D<float vector[4]>::mips_slice_type const&)(ptr noundef nonnull align 4 dereferenceable(8) %agg.result, ptr noundef nonnull align 4 dereferenceable(8) %[[SLICE]])

// CHECK: define linkonce_odr hidden noundef nofpclass(nan inf) <4 x float> @hlsl::Texture2D<float vector[4]>::mips_slice_type::operator[](int vector[2]) const(ptr noundef nonnull align 4 dereferenceable(8) %this, <2 x i32> noundef %Coord)
// CHECK: entry:
// CHECK: %[[THIS1:.*]] = load ptr, ptr %{{.*}}, align 4
// CHECK: %[[HANDLE_PTR:.*]] = getelementptr inbounds nuw %"struct.hlsl::Texture2D<>::mips_slice_type", ptr %[[THIS1]], i32 0, i32 0
// CHECK: %[[HANDLE:.*]] = load target("dx.Texture", <4 x float>, 0, 0, 0, 2), ptr %[[HANDLE_PTR]], align 4
// CHECK: %[[LEVEL_PTR:.*]] = getelementptr inbounds nuw %"struct.hlsl::Texture2D<>::mips_slice_type", ptr %[[THIS1]], i32 0, i32 1
// CHECK: %[[LEVEL:.*]] = load i32, ptr %[[LEVEL_PTR]], align 4
// DXIL: %[[RES:.*]] = call reassoc nnan ninf nsz arcp afn <4 x float> @llvm.dx.resource.load.level.v4f32.tdx.Texture_v4f32_0_0_0_2t.v2i32.i32.v2i32(target("dx.Texture", <4 x float>, 0, 0, 0, 2) %[[HANDLE]], <2 x i32> %{{.*}}, i32 %[[LEVEL]], <2 x i32> zeroinitializer)
// CHECK: ret <4 x float> %[[RES]]
