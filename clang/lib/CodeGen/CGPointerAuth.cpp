//===--- CGPointerAuth.cpp - IR generation for pointer authentication -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains common routines relating to the emission of
// pointer authentication operations.
//
//===----------------------------------------------------------------------===//

#include "CodeGenModule.h"
#include "clang/CodeGen/CodeGenABITypes.h"

using namespace clang;
using namespace CodeGen;

/// Return the abstract pointer authentication schema for a pointer to the given
/// function type.
CGPointerAuthInfo CodeGenModule::getFunctionPointerAuthInfo(QualType T) {
  const auto &Schema = getCodeGenOpts().PointerAuth.FunctionPointers;
  if (!Schema)
    return CGPointerAuthInfo();

  assert(!Schema.isAddressDiscriminated() &&
         "function pointers cannot use address-specific discrimination");

  assert(!Schema.hasOtherDiscrimination() &&
         "function pointers don't support any discrimination yet");

  return CGPointerAuthInfo(Schema.getKey(), Schema.getAuthenticationMode(),
                           /*IsaPointer=*/false, /*AuthenticatesNull=*/false,
                           /*Discriminator=*/nullptr);
}

/// Build a signed-pointer "ptrauth" constant.
static llvm::ConstantPtrAuth *
buildConstantAddress(CodeGenModule &CGM, llvm::Constant *Pointer, unsigned Key,
                     llvm::Constant *StorageAddress,
                     llvm::Constant *OtherDiscriminator) {
  llvm::Constant *AddressDiscriminator = nullptr;
  if (StorageAddress) {
    AddressDiscriminator = StorageAddress;
    assert(StorageAddress->getType() == CGM.UnqualPtrTy);
  } else {
    AddressDiscriminator = llvm::Constant::getNullValue(CGM.UnqualPtrTy);
  }

  llvm::ConstantInt *IntegerDiscriminator = nullptr;
  if (OtherDiscriminator) {
    assert(OtherDiscriminator->getType() == CGM.Int64Ty);
    IntegerDiscriminator = cast<llvm::ConstantInt>(OtherDiscriminator);
  } else {
    IntegerDiscriminator = llvm::ConstantInt::get(CGM.Int64Ty, 0);
  }

  return llvm::ConstantPtrAuth::get(Pointer,
                                    llvm::ConstantInt::get(CGM.Int32Ty, Key),
                                    IntegerDiscriminator, AddressDiscriminator);
}

llvm::Constant *
CodeGenModule::getConstantSignedPointer(llvm::Constant *Pointer, unsigned Key,
                                        llvm::Constant *StorageAddress,
                                        llvm::Constant *OtherDiscriminator) {
  llvm::Constant *Stripped = Pointer->stripPointerCasts();

  // Build the constant.
  return buildConstantAddress(*this, Stripped, Key, StorageAddress,
                              OtherDiscriminator);
}

llvm::Constant *
CodeGen::getConstantSignedPointer(CodeGenModule &CGM, llvm::Constant *Pointer,
                                  unsigned Key, llvm::Constant *StorageAddress,
                                  llvm::Constant *OtherDiscriminator) {
  return CGM.getConstantSignedPointer(Pointer, Key, StorageAddress,
                                      OtherDiscriminator);
}

llvm::Constant *CodeGenModule::getFunctionPointer(llvm::Constant *Pointer,
                                                  QualType FunctionType) {
  assert(FunctionType->isFunctionType() ||
         FunctionType->isFunctionReferenceType() ||
         FunctionType->isFunctionPointerType());

  if (auto PointerAuth = getFunctionPointerAuthInfo(FunctionType))
    return getConstantSignedPointer(
        Pointer, PointerAuth.getKey(), /*StorageAddress=*/nullptr,
        cast_or_null<llvm::Constant>(PointerAuth.getDiscriminator()));

  return Pointer;
}

llvm::Constant *CodeGenModule::getFunctionPointer(GlobalDecl GD,
                                                  llvm::Type *Ty) {
  const auto *FD = cast<FunctionDecl>(GD.getDecl());
  QualType FuncType = FD->getType();
  return getFunctionPointer(getRawFunctionPointer(GD, Ty), FuncType);
}
