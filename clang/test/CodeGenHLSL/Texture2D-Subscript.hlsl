// RUN: %clang_cc1 -finclude-default-header -triple spirv-pc-vulkan-library %s -emit-llvm-only -disable-llvm-passes -verify
// expected-no-diagnostics

Texture2D<float4> Tex : register(t0);

[numthreads(1,1,1)]
void main(uint2 DTid : SV_DispatchThreadID) {
  float4 val = Tex[DTid];
}

// CHECK: call ptr addrspace(11) @llvm.spv.resource.getpointer{{.*}}(target("spirv.Image"{{.*}}) %{{.*}}, <2 x i32> %{{.*}})
