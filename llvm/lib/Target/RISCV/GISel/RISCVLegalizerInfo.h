//===-- RISCVLegalizerInfo.h ------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the Machinelegalizer class for RISC-V.
/// \todo This should be generated by TableGen.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCV_RISCVMACHINELEGALIZER_H
#define LLVM_LIB_TARGET_RISCV_RISCVMACHINELEGALIZER_H

#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"
#include "llvm/CodeGen/Register.h"

namespace llvm {

class GISelChangeObserver;
class MachineIRBuilder;
class RISCVSubtarget;

class RISCVLegalizerInfo : public LegalizerInfo {
  const RISCVSubtarget &STI;
  const unsigned XLen;
  const LLT sXLen;

public:
  RISCVLegalizerInfo(const RISCVSubtarget &ST);

  bool legalizeCustom(LegalizerHelper &Helper, MachineInstr &MI,
                      LostDebugLocObserver &LocObserver) const override;

  bool legalizeIntrinsic(LegalizerHelper &Helper,
                         MachineInstr &MI) const override;

private:
  bool shouldBeInConstantPool(const APInt &APImm, bool ShouldOptForSize) const;
  bool legalizeShlAshrLshr(MachineInstr &MI, MachineIRBuilder &MIRBuilder,
                           GISelChangeObserver &Observer) const;

  bool legalizeBRJT(MachineInstr &MI, MachineIRBuilder &MIRBuilder) const;
  bool legalizeVAStart(MachineInstr &MI, MachineIRBuilder &MIRBuilder) const;
  bool legalizeVScale(MachineInstr &MI, MachineIRBuilder &MIB) const;
  bool legalizeExt(MachineInstr &MI, MachineIRBuilder &MIRBuilder) const;
  bool legalizeSplatVector(MachineInstr &MI, MachineIRBuilder &MIB) const;
  bool legalizeExtractSubvector(MachineInstr &MI, MachineIRBuilder &MIB) const;
  bool legalizeInsertSubvector(MachineInstr &MI, LegalizerHelper &Helper,
                               MachineIRBuilder &MIB) const;
  bool legalizeLoadStore(MachineInstr &MI, LegalizerHelper &Helper,
                         MachineIRBuilder &MIB) const;
};
} // end namespace llvm
#endif
