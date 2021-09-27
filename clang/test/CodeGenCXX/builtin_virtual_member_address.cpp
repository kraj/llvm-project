// RUN: %clang_cc1 -triple arm64e-apple-ios -fptrauth-calls -emit-llvm -no-enable-noundef-analysis -o - %s | FileCheck %s --check-prefixes CHECK,CHECK-AUTH
// RUN: %clang_cc1 -triple arm64-apple-ios -emit-llvm -no-enable-noundef-analysis -o - %s | FileCheck %s --check-prefixes CHECK,CHECK-NOAUTH

struct Base {
  virtual void func1();
  virtual void func2();
  void nonvirt();
};

struct Derived : Base {
  virtual void func1();
};

// CHECK-LABEL: define ptr @_Z6simpleR4Base(ptr {{.*}} %b)

// CHECK-AUTH: [[BLOC:%.*]] = alloca ptr
// CHECK-AUTH: store ptr %b, ptr [[BLOC]]
// CHECK-AUTH: [[B:%.*]] = load ptr, ptr [[BLOC]]
// CHECK-AUTH: [[VTABLE:%.*]] = load ptr, ptr [[B]]
// CHECK-AUTH: [[VTABLE_AUTH_IN:%.*]] = ptrtoint ptr [[VTABLE]] to i64
// CHECK-AUTH: [[VTABLE_AUTH_OUT:%.*]] = call i64 @llvm.ptrauth.auth(i64 [[VTABLE_AUTH_IN]], i32 2, i64 0)
// CHECK-AUTH: [[VTABLE:%.*]] = inttoptr i64 [[VTABLE_AUTH_OUT]] to ptr
// CHECK-AUTH: [[FUNC_ADDR:%.*]] = getelementptr inbounds ptr, ptr [[VTABLE]], i64 1
// CHECK-AUTH: [[FUNC:%.*]] = load ptr, ptr [[FUNC_ADDR]]
// CHECK-AUTH: [[FUNC_ADDR_I64:%.*]] = ptrtoint ptr [[FUNC_ADDR]] to i64
// CHECK-AUTH: [[DISC:%.*]] = call i64 @llvm.ptrauth.blend(i64 [[FUNC_ADDR_I64]], i64 25637)
// CHECK-AUTH: [[FUNC_I64:%.*]] = ptrtoint ptr [[FUNC]] to i64
// CHECK-AUTH: [[FUNC_AUTHED:%.*]] =  call i64 @llvm.ptrauth.auth(i64 [[FUNC_I64]], i32 0, i64 [[DISC]])
// CHECK-AUTH: [[FUNC:%.*]] = inttoptr i64 [[FUNC_AUTHED]] to ptr
// CHECK-AUTH: ret ptr [[FUNC]]

// CHECK-NOAUTH: [[BLOC:%.*]] = alloca ptr
// CHECK-NOAUTH: store ptr %b, ptr [[BLOC]]
// CHECK-NOAUTH: [[B:%.*]] = load ptr, ptr [[BLOC]]
// CHECK-NOAUTH: [[VTABLE:%.*]] = load ptr, ptr [[B]]
// CHECK-NOAUTH: [[FUNC_ADDR:%.*]] = getelementptr inbounds ptr, ptr [[VTABLE]], i64 1
// CHECK-NOAUTH: [[FUNC:%.*]] = load ptr, ptr [[FUNC_ADDR]]
// CHECK-NOAUTH: ret ptr [[FUNC]]
void *simple(Base &b) {
  return __builtin_virtual_member_address(b, &Base::func2);
}
