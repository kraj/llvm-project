// RUN: %clang_cc1 -triple dxil-pc-shadermodel6.0-library -x hlsl -emit-llvm -disable-llvm-passes -finclude-default-header -Wno-sign-conversion -o - %s | llvm-cxxfilt | FileCheck %s --check-prefixes=CHECK,DXIL

Texture2D<float4> t;

// CHECK: define hidden {{.*}} <4 x float> @test_subscript(float vector[2])
// CHECK: %[[CALL:.*]] = call {{.*}} ptr @hlsl::Texture2D<float vector[4]>::operator[](unsigned int vector[2]) const(ptr {{.*}} @t, <2 x i32> {{.*}})
// CHECK: %[[LOAD:.*]] = load <4 x float>, ptr %[[CALL]]
// CHECK: ret <4 x float> %[[LOAD]]

float4 test_subscript(float2 loc : LOC) : SV_Target {
  return t[int2(loc)];
}

// CHECK: define linkonce_odr hidden {{.*}} ptr @hlsl::Texture2D<float vector[4]>::operator[](unsigned int vector[2]) const(ptr {{.*}} %[[THIS:[^,]+]], <2 x i32> {{.*}} %[[COORD:[^)]+]])
// CHECK: %[[THIS_ADDR:.*]] = alloca ptr
// CHECK: %[[COORD_ADDR:.*]] = alloca <2 x i32>
// CHECK: store ptr %[[THIS]], ptr %[[THIS_ADDR]]
// CHECK: store <2 x i32> %[[COORD]], ptr %[[COORD_ADDR]]
// CHECK: %[[THIS_VAL:.*]] = load ptr, ptr %[[THIS_ADDR]]
// CHECK: %[[HANDLE_GEP:.*]] = getelementptr inbounds nuw %"class.hlsl::Texture2D", ptr %[[THIS_VAL]], i32 0, i32 0
// CHECK: %[[HANDLE:.*]] = load target{{.*}}, ptr %[[HANDLE_GEP]]
// CHECK: %[[COORD_VAL:.*]] = load <2 x i32>, ptr %{{.*}}
// DXIL: %[[RES:.*]] = call ptr @llvm.dx.resource.getpointer.p0.tdx.Texture_v4f32_0_0_0_2t.v2i32(target("dx.Texture", <4 x float>, 0, 0, 0, 2) %[[HANDLE]], <2 x i32> %[[COORD_VAL]])
// CHECK: ret ptr %[[RES]]
