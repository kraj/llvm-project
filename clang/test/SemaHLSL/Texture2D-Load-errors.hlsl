// RUN: %clang_cc1 -triple dxil-pc-shadermodel6.0-library -x hlsl -finclude-default-header -verify %s

Texture2D<float4> t;

float4 test_too_few_args() {
  return t.Load(); // expected-error {{no matching member function for call to 'Load'}}
  // expected-note@*:* {{candidate function not viable: requires single argument 'Location', but no arguments were provided}}
  // expected-note@*:* {{candidate function not viable: requires 2 arguments, but 0 were provided}}
}

float4 test_too_many_args(int2 loc) {
  return t.Load(int3(loc, 0), int2(1, 1), 1); // expected-error {{no matching member function for call to 'Load'}}
  // expected-note@*:* {{candidate function not viable: requires 2 arguments, but 3 were provided}}
  // expected-note@*:* {{candidate function not viable: requires single argument 'Location', but 3 arguments were provided}}
}

float4 test_invalid_coord_type(float2 loc) {
  return t.Load(float3(loc, 0)); // expected-warning {{implicit conversion turns floating-point number into integer: 'float3' (aka 'vector<float, 3>') to 'vector<int, 3>'}}
}

float4 test_invalid_offset_type(int2 loc, float2 offset) {
  return t.Load(int3(loc, 0), offset); // expected-warning {{implicit conversion turns floating-point number into integer: 'float2' (aka 'vector<float, 2>') to 'vector<int, 2>'}}
}
