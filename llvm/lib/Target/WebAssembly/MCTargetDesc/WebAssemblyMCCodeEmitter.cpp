//=- WebAssemblyMCCodeEmitter.cpp - Convert WebAssembly code to machine code -//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the WebAssemblyMCCodeEmitter class.
///
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/WebAssemblyFixupKinds.h"
#include "MCTargetDesc/WebAssemblyMCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted.");
STATISTIC(MCNumFixups, "Number of MC fixups created.");

namespace {
class WebAssemblyMCCodeEmitter final : public MCCodeEmitter {
  const MCInstrInfo &MCII;
  MCContext &Ctx;
  // Implementation generated by tablegen.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

public:
  WebAssemblyMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx)
      : MCII(MCII), Ctx{Ctx} {}
};
} // end anonymous namespace

MCCodeEmitter *llvm::createWebAssemblyMCCodeEmitter(const MCInstrInfo &MCII,
                                                    MCContext &Ctx) {
  return new WebAssemblyMCCodeEmitter(MCII, Ctx);
}

void WebAssemblyMCCodeEmitter::encodeInstruction(
    const MCInst &MI, SmallVectorImpl<char> &CB,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  raw_svector_ostream OS(CB);
  uint64_t Start = OS.tell();

  uint64_t Binary = getBinaryCodeForInstr(MI, Fixups, STI);
  if (Binary < (1 << 8)) {
    OS << uint8_t(Binary);
  } else if (Binary < (1 << 16)) {
    OS << uint8_t(Binary >> 8);
    encodeULEB128(uint8_t(Binary), OS);
  } else if (Binary < (1 << 24)) {
    OS << uint8_t(Binary >> 16);
    encodeULEB128(uint16_t(Binary), OS);
  } else {
    llvm_unreachable("Very large (prefix + 3 byte) opcodes not supported");
  }

  // For br_table instructions, encode the size of the table. In the MCInst,
  // there's an index operand (if not a stack instruction), one operand for
  // each table entry, and the default operand.
  unsigned Opcode = MI.getOpcode();
  if (Opcode == WebAssembly::BR_TABLE_I32_S ||
      Opcode == WebAssembly::BR_TABLE_I64_S)
    encodeULEB128(MI.getNumOperands() - 1, OS);
  if (Opcode == WebAssembly::BR_TABLE_I32 ||
      Opcode == WebAssembly::BR_TABLE_I64)
    encodeULEB128(MI.getNumOperands() - 2, OS);

  const MCInstrDesc &Desc = MCII.get(Opcode);
  for (unsigned I = 0, E = MI.getNumOperands(); I < E; ++I) {
    const MCOperand &MO = MI.getOperand(I);
    if (MO.isReg()) {
      /* nothing to encode */

    } else if (MO.isImm()) {
      if (I < Desc.getNumOperands()) {
        const MCOperandInfo &Info = Desc.operands()[I];
        LLVM_DEBUG(dbgs() << "Encoding immediate: type="
                          << int(Info.OperandType) << "\n");
        switch (Info.OperandType) {
        case WebAssembly::OPERAND_I32IMM:
          encodeSLEB128(int32_t(MO.getImm()), OS);
          break;
        case WebAssembly::OPERAND_OFFSET32:
          encodeULEB128(uint32_t(MO.getImm()), OS);
          break;
        case WebAssembly::OPERAND_I64IMM:
          encodeSLEB128(int64_t(MO.getImm()), OS);
          break;
        case WebAssembly::OPERAND_SIGNATURE:
        case WebAssembly::OPERAND_VEC_I8IMM:
          support::endian::write<uint8_t>(OS, MO.getImm(),
                                          llvm::endianness::little);
          break;
        case WebAssembly::OPERAND_VEC_I16IMM:
          support::endian::write<uint16_t>(OS, MO.getImm(),
                                           llvm::endianness::little);
          break;
        case WebAssembly::OPERAND_VEC_I32IMM:
          support::endian::write<uint32_t>(OS, MO.getImm(),
                                           llvm::endianness::little);
          break;
        case WebAssembly::OPERAND_VEC_I64IMM:
          support::endian::write<uint64_t>(OS, MO.getImm(),
                                           llvm::endianness::little);
          break;
        case WebAssembly::OPERAND_GLOBAL:
          Ctx.reportError(
              SMLoc(),
              Twine("Wasm globals should only be accessed symbolically!"));
          break;
        default:
          encodeULEB128(uint64_t(MO.getImm()), OS);
        }
      } else {
        // Variadic immediate operands are br_table's destination operands or
        // try_table's operands (# of catch clauses, catch sub-opcodes, or catch
        // clause destinations)
        assert(WebAssembly::isBrTable(Opcode) ||
               Opcode == WebAssembly::TRY_TABLE_S);
        encodeULEB128(uint32_t(MO.getImm()), OS);
      }

    } else if (MO.isSFPImm()) {
      uint32_t F = MO.getSFPImm();
      support::endian::write<uint32_t>(OS, F, llvm::endianness::little);
    } else if (MO.isDFPImm()) {
      uint64_t D = MO.getDFPImm();
      support::endian::write<uint64_t>(OS, D, llvm::endianness::little);
    } else if (MO.isExpr()) {
      llvm::MCFixupKind FixupKind;
      size_t PaddedSize = 5;
      if (I < Desc.getNumOperands()) {
        const MCOperandInfo &Info = Desc.operands()[I];
        switch (Info.OperandType) {
        case WebAssembly::OPERAND_I32IMM:
          FixupKind = WebAssembly::fixup_sleb128_i32;
          break;
        case WebAssembly::OPERAND_I64IMM:
          FixupKind = WebAssembly::fixup_sleb128_i64;
          PaddedSize = 10;
          break;
        case WebAssembly::OPERAND_FUNCTION32:
        case WebAssembly::OPERAND_TABLE:
        case WebAssembly::OPERAND_OFFSET32:
        case WebAssembly::OPERAND_SIGNATURE:
        case WebAssembly::OPERAND_TYPEINDEX:
        case WebAssembly::OPERAND_GLOBAL:
        case WebAssembly::OPERAND_TAG:
          FixupKind = WebAssembly::fixup_uleb128_i32;
          break;
        case WebAssembly::OPERAND_OFFSET64:
          FixupKind = WebAssembly::fixup_uleb128_i64;
          PaddedSize = 10;
          break;
        default:
          llvm_unreachable("unexpected symbolic operand kind");
        }
      } else {
        // Variadic expr operands are try_table's catch/catch_ref clauses' tags.
        assert(Opcode == WebAssembly::TRY_TABLE_S);
        FixupKind = WebAssembly::fixup_uleb128_i32;
      }
      Fixups.push_back(
          MCFixup::create(OS.tell() - Start, MO.getExpr(), FixupKind));
      ++MCNumFixups;
      encodeULEB128(0, OS, PaddedSize);
    } else {
      llvm_unreachable("unexpected operand kind");
    }
  }

  ++MCNumEmitted; // Keep track of the # of mi's emitted.
}

#include "WebAssemblyGenMCCodeEmitter.inc"
