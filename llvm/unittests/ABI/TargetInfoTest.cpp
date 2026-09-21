//===- TargetInfoTest.cpp - shared ABI TargetInfo unit tests --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Tests for target-independent helpers on the TargetInfo base class. These are
// exercised through a minimal concrete target so any new shared default lands
// here rather than in a per-target test file.
//
//===----------------------------------------------------------------------===//

#include "llvm/ABI/TargetInfo.h"
#include "llvm/ABI/FunctionInfo.h"
#include "llvm/ABI/Types.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/Allocator.h"
#include "gtest/gtest.h"

namespace {

// RecordFlags' bitmask operators are declared in namespace llvm, so combining
// two of them needs that namespace visible.
using namespace llvm;

using ABIType = llvm::abi::Type;
using llvm::abi::ArgInfo;
using llvm::abi::FieldInfo;
using llvm::abi::FunctionInfo;
using llvm::abi::RecordFlags;
using llvm::abi::StructPacking;
using llvm::abi::TargetInfo;
using llvm::abi::TypeBuilder;

// A minimal concrete target that re-exposes the shared, protected default
// classifiers so they can be exercised directly, independent of any real
// target.
class TestTargetInfo : public TargetInfo {
public:
  explicit TestTargetInfo(TypeBuilder &Builder) : TargetInfo(Builder) {}
  void computeInfo(FunctionInfo &) const override {}
  const llvm::abi::ABICompatInfo &getABICompatInfo() const override {
    return Compat;
  }
  using TargetInfo::classifyDefaultArgumentType;
  using TargetInfo::classifyDefaultReturnType;

private:
  llvm::abi::ABICompatInfo Compat;
};

class TargetInfoTest : public ::testing::Test {
protected:
  llvm::BumpPtrAllocator Alloc;
  TypeBuilder TB;
  const ABIType *I16;
  const ABIType *I32;
  const ABIType *Void;
  /// A _BitInt wider than 128 bits, which cannot be passed in registers.
  const ABIType *WideBitInt;

  TargetInfoTest()
      : TB(Alloc), I16(TB.getIntegerType(16, llvm::Align(2), /*Signed=*/true)),
        I32(TB.getIntegerType(32, llvm::Align(4), /*Signed=*/true)),
        Void(TB.getVoidType()),
        WideBitInt(TB.getIntegerType(129, llvm::Align(8), /*Signed=*/true,
                                     /*IsBitInt=*/true)) {}

  /// A record with a single int field, passable in registers.
  const ABIType *recordInReg() {
    return TB.getRecordType({FieldInfo(I32, 0)}, llvm::TypeSize::getFixed(32),
                            llvm::Align(4), /*UnadjustedAlign=*/llvm::Align(4),
                            StructPacking::Default, {}, {},
                            RecordFlags::CanPassInRegisters);
  }

  /// The same record marked as unable to pass in registers, e.g. a non-trivial
  /// C++ type. This takes the RAA_Indirect path.
  const ABIType *recordInMemory() {
    return TB.getRecordType({FieldInfo(I32, 0)}, llvm::TypeSize::getFixed(32),
                            llvm::Align(4), /*UnadjustedAlign=*/llvm::Align(4),
                            StructPacking::Default, {}, {}, RecordFlags::None);
  }

  ArgInfo classifyArg(const ABIType *Ty) {
    TestTargetInfo TI(TB);
    return TI.classifyDefaultArgumentType(Ty);
  }

  ArgInfo classifyRet(const ABIType *Ty) {
    TestTargetInfo TI(TB);
    return TI.classifyDefaultReturnType(Ty);
  }
};

// --- Argument classification -------------------------------------------------

// A word-sized integer is passed directly.
TEST_F(TargetInfoTest, DefaultArgIntIsDirect) {
  ArgInfo Info = classifyArg(I32);
  EXPECT_TRUE(Info.isDirect());
}

// A sub-word integer is promoted.
TEST_F(TargetInfoTest, DefaultArgSmallIntIsExtended) {
  EXPECT_TRUE(classifyArg(I16).isExtend());
}

// A record that fits in registers is passed indirectly by value.
TEST_F(TargetInfoTest, DefaultArgRecordInRegIsIndirectByVal) {
  ArgInfo Info = classifyArg(recordInReg());
  ASSERT_TRUE(Info.isIndirect());
  EXPECT_TRUE(Info.getIndirectByVal());
}

// A record that cannot pass in registers goes indirect without ByVal.
TEST_F(TargetInfoTest, DefaultArgRecordInMemoryIsIndirectNoByVal) {
  ArgInfo Info = classifyArg(recordInMemory());
  ASSERT_TRUE(Info.isIndirect());
  EXPECT_FALSE(Info.getIndirectByVal());
}

// A _BitInt wider than 128 bits is passed indirectly.
TEST_F(TargetInfoTest, DefaultArgWideBitIntIsIndirect) {
  EXPECT_TRUE(classifyArg(WideBitInt).isIndirect());
}

// A transparent union is classified as its first field, so a union of one int
// is passed directly rather than as an aggregate.
TEST_F(TargetInfoTest, DefaultArgTransparentUnionUsesFirstField) {
  const ABIType *U = TB.getUnionType(
      {FieldInfo(I32, 0)}, llvm::TypeSize::getFixed(32), llvm::Align(4),
      /*UnadjustedAlign=*/llvm::Align(4), StructPacking::Default,
      RecordFlags::IsTransparent | RecordFlags::CanPassInRegisters);
  EXPECT_TRUE(classifyArg(U).isDirect());
}

// --- Return classification ---------------------------------------------------

// Void returns are ignored.
TEST_F(TargetInfoTest, DefaultReturnVoidIsIgnored) {
  EXPECT_TRUE(classifyRet(Void).isIgnore());
}

// A word-sized integer is returned directly.
TEST_F(TargetInfoTest, DefaultReturnIntIsDirect) {
  EXPECT_TRUE(classifyRet(I32).isDirect());
}

// A sub-word integer is promoted on return.
TEST_F(TargetInfoTest, DefaultReturnSmallIntIsExtended) {
  EXPECT_TRUE(classifyRet(I16).isExtend());
}

// An aggregate is returned indirectly, and returns never use ByVal.
TEST_F(TargetInfoTest, DefaultReturnRecordIsIndirectNoByVal) {
  ArgInfo Info = classifyRet(recordInReg());
  ASSERT_TRUE(Info.isIndirect());
  EXPECT_FALSE(Info.getIndirectByVal());
}

// A _BitInt wider than 128 bits is returned indirectly.
TEST_F(TargetInfoTest, DefaultReturnWideBitIntIsIndirect) {
  ArgInfo Info = classifyRet(WideBitInt);
  ASSERT_TRUE(Info.isIndirect());
  EXPECT_FALSE(Info.getIndirectByVal());
}

} // namespace
