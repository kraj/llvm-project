//=====-- Rematerializer.cpp - MIR rematerialization support ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//==-----------------------------------------------------------------------===//
//
/// \file
/// Implements helpers for target-independent rematerialization at the MIR
/// level.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/Rematerializer.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/MC/LaneBitmask.h"
#include "llvm/Support/Debug.h"
#include <optional>

#define DEBUG_TYPE "rematerializer"

using namespace llvm;
using RegisterIdx = Rematerializer::RegisterIdx;

// Pin the vtable to this file.
void Rematerializer::Listener::anchor() {}

/// Checks whether the value in \p LI at \p UseIdx is identical to \p OVNI (this
/// implies it is also live there). When \p LI has sub-ranges, checks that
/// all sub-ranges intersecting with \p Mask are also live at \p UseIdx.
static bool isIdenticalAtUse(const VNInfo &OVNI, LaneBitmask Mask,
                             SlotIndex UseIdx, const LiveInterval &LI) {
  if (&OVNI != LI.getVNInfoAt(UseIdx))
    return false;

  if (LI.hasSubRanges()) {
    // Check that intersecting subranges are live at user.
    for (const LiveInterval::SubRange &SR : LI.subranges()) {
      if ((SR.LaneMask & Mask).none())
        continue;
      if (!SR.liveAt(UseIdx))
        return false;

      // Early exit if all used lanes are checked. No need to continue.
      Mask &= ~SR.LaneMask;
      if (Mask.none())
        break;
    }
  }
  return true;
}

/// If \p MO is a virtual read register, returns it. Otherwise returns the
/// sentinel register.
static Register getRegDependency(const MachineOperand &MO) {
  if (!MO.isReg() || !MO.readsReg())
    return Register();
  Register Reg = MO.getReg();
  if (Reg.isPhysical()) {
    // By the requirements on trivially rematerializable instructions, a
    // physical register use is either constant or ignorable.
    return Register();
  }
  return Reg;
}

RegisterIdx Rematerializer::rematerializeToRegion(RegisterIdx RootIdx,
                                                  unsigned UseRegion,
                                                  DependencyReuseInfo &DRI) {
  MachineInstr *FirstMI =
      getReg(RootIdx).getRegionUseBounds(UseRegion, LIS).first;
  // If there are no users in the region, rematerialize the register at the very
  // end of the region.
  MachineBasicBlock::iterator InsertPos =
      FirstMI ? FirstMI : Regions[UseRegion].second;
  RegisterIdx NewRegIdx =
      rematerializeToPos(RootIdx, UseRegion, InsertPos, DRI);
  transferRegionUsers(RootIdx, NewRegIdx, UseRegion);
  return NewRegIdx;
}

RegisterIdx
Rematerializer::rematerializeToPos(RegisterIdx RootIdx, unsigned UseRegion,
                                   MachineBasicBlock::iterator InsertPos,
                                   DependencyReuseInfo &DRI) {
  assert(!DRI.DependencyMap.contains(RootIdx));
  LLVM_DEBUG(dbgs() << "Rematerializing " << printID(RootIdx) << '\n');

  SmallVector<RegisterIdx, 2> NewDeps;
  // Copy all dependencies because recursive rematerialization of dependencies
  // may invalidate references to the backing vector of registers.
  SmallVector<RegisterIdx, 2> OldDeps(getReg(RootIdx).Dependencies);
  for (RegisterIdx DepRegIdx : OldDeps) {
    // Recursively rematerialize required dependencies at the same position as
    // the root. Registers form a DAG so the recursion is guaranteed to
    // terminate.
    auto RematIdx = DRI.DependencyMap.find(DepRegIdx);
    RegisterIdx NewDepRegIdx;
    if (RematIdx == DRI.DependencyMap.end())
      NewDepRegIdx = rematerializeToPos(DepRegIdx, UseRegion, InsertPos, DRI);
    else
      NewDepRegIdx = RematIdx->second;
    NewDeps.push_back(NewDepRegIdx);
  }
  RegisterIdx NewIdx =
      rematerializeReg(RootIdx, UseRegion, InsertPos, std::move(NewDeps));
  DRI.DependencyMap.insert({RootIdx, NewIdx});
  return NewIdx;
}

void Rematerializer::transferUser(RegisterIdx FromRegIdx, RegisterIdx ToRegIdx,
                                  unsigned UserRegion, MachineInstr &UserMI) {
  transferUserImpl(FromRegIdx, ToRegIdx, UserMI);
  Regs[FromRegIdx].eraseUser(&UserMI, UserRegion);
  Regs[ToRegIdx].addUser(&UserMI, UserRegion);
  deleteRegIfUnused(FromRegIdx);
}

void Rematerializer::transferRegionUsers(RegisterIdx FromRegIdx,
                                         RegisterIdx ToRegIdx,
                                         unsigned UseRegion) {
  auto &FromRegUsers = Regs[FromRegIdx].Uses;
  auto UsesIt = FromRegUsers.find(UseRegion);
  if (UsesIt == FromRegUsers.end())
    return;

  const SmallDenseSet<MachineInstr *, 4> &RegionUsers = UsesIt->getSecond();
  for (MachineInstr *UserMI : RegionUsers)
    transferUserImpl(FromRegIdx, ToRegIdx, *UserMI);
  Regs[ToRegIdx].addUsers(RegionUsers, UseRegion);
  FromRegUsers.erase(UseRegion);
  deleteRegIfUnused(FromRegIdx);
}

void Rematerializer::transferAllUsers(RegisterIdx FromRegIdx,
                                      RegisterIdx ToRegIdx) {
  Reg &FromReg = Regs[FromRegIdx], &ToReg = Regs[ToRegIdx];
  for (const auto &[UseRegion, RegionUsers] : FromReg.Uses) {
    for (MachineInstr *UserMI : RegionUsers)
      transferUserImpl(FromRegIdx, ToRegIdx, *UserMI);
    ToReg.addUsers(RegionUsers, UseRegion);
  }
  FromReg.Uses.clear();
  deleteRegIfUnused(FromRegIdx);
}

void Rematerializer::transferUserImpl(RegisterIdx FromRegIdx,
                                      RegisterIdx ToRegIdx,
                                      MachineInstr &UserMI) {
  assert(FromRegIdx != ToRegIdx && "identical registers");
  assert(getOriginOrSelf(FromRegIdx) == getOriginOrSelf(ToRegIdx) &&
         "unrelated registers");
  assert(any_of(getReg(FromRegIdx).Uses,
                [&](const auto RegionUsers) {
                  return RegionUsers.second.contains(&UserMI);
                }) &&
         "not a user");

  LLVM_DEBUG(dbgs() << "User transfer from " << printID(FromRegIdx) << " to "
                    << printID(ToRegIdx) << ": " << printUser(&UserMI) << '\n');

  Register FromReg = getReg(FromRegIdx).getDefReg(),
           ToReg = getReg(ToRegIdx).getDefReg();
  UserMI.substituteRegister(FromReg, ToReg, 0, TRI);
  LISUpdates.insert(FromRegIdx);
  LISUpdates.insert(ToRegIdx);

  RegisterIdx UserRegIdx = getDefRegIdx(UserMI);
  if (UserRegIdx == NoReg)
    return;

  // When the user is rematerializable, we must reflect the change in its
  // dependencies.
  Reg &UserReg = Regs[UserRegIdx];
  SmallVectorImpl<RegisterIdx> &UserDeps = Regs[UserRegIdx].Dependencies;
  bool IsNewDep = true;
  if (UserReg.Defs.size() > 1) {
    // Other defining MIs might already be using the new register.
    IsNewDep = find(UserDeps, ToRegIdx) == UserDeps.end();

    auto MOIsFromReg = [FromReg](MachineOperand &MO) {
      return MO.getReg() == FromReg;
    };

    // If any other defining instruction of the rematerializable user still uses
    // the original register, we should not remove it from dependencies and may
    // need to add a new dependency if it is the first time the new register is
    // used by defining instructions.
    for (MachineInstr *DefMI : UserReg.Defs) {
      if (DefMI == &UserMI)
        continue;
      if (any_of(DefMI->all_uses(), MOIsFromReg)) {
        if (IsNewDep)
          UserDeps.push_back(ToRegIdx);
        return;
      }
    }
  }

  // No other defining instruction has the original register as user. This
  // either removes a dependency if the new register was previously used, or is
  // a simple replacement if not.
  unsigned *FindFromReg = find(UserDeps, FromRegIdx);
  assert(FindFromReg != UserDeps.end() && "broken dependency");
  if (IsNewDep)
    *FindFromReg = ToRegIdx;
  else
    UserReg.Dependencies.erase(FindFromReg);
}

void Rematerializer::updateLiveIntervals() {
  DenseSet<Register> SeenUnrematRegs;
  for (RegisterIdx RegIdx : LISUpdates) {
    const Reg &UpdateReg = getReg(RegIdx);
    assert(UpdateReg.isAlive() && "dead register");

    Register DefReg = UpdateReg.getDefReg();
    if (LIS.hasInterval(DefReg))
      LIS.removeInterval(DefReg);
    LIS.createAndComputeVirtRegInterval(DefReg);

    LLVM_DEBUG({
      dbgs() << "Re-computed interval for " << printID(RegIdx) << ": ";
      LIS.getInterval(DefReg).print(dbgs());
      dbgs() << '\n' << printRegUsers(RegIdx);
    });

    // Update intervals for unrematerializable operands.
    for (const auto &[UnrematReg, Mask] : getUnrematableDeps(RegIdx)) {
      if (!SeenUnrematRegs.insert(UnrematReg).second)
        continue;
      LIS.removeInterval(UnrematReg);
      LIS.createAndComputeVirtRegInterval(UnrematReg);
      LLVM_DEBUG(
          dbgs() << "  Re-computed interval for unrematerializable register "
                 << printReg(UnrematReg, &TRI, 0, &MRI) << " with lanemask "
                 << Mask << '\n');
    }
  }
  LISUpdates.clear();
}

bool Rematerializer::isMOIdenticalAtUses(MachineOperand &MO,
                                         ArrayRef<SlotIndex> Uses) const {
  unsigned SubIdx = MO.getSubReg();
  LaneBitmask Mask = SubIdx ? TRI.getSubRegIndexLaneMask(SubIdx)
                            : MRI.getMaxLaneMaskForVReg(MO.getReg());
  return isRegIdenticalAtUses(
      MO.getReg(), Mask,
      LIS.getInstructionIndex(*MO.getParent()).getRegSlot(true), Uses);
}

bool Rematerializer::isRegIdenticalAtUses(Register Reg, LaneBitmask Mask,
                                          SlotIndex RefSlot,
                                          ArrayRef<SlotIndex> Uses) const {
  if (Uses.empty())
    return true;
  const LiveInterval &LI = LIS.getInterval(Reg);
  const VNInfo *DefVN = LI.getVNInfoAt(RefSlot);
  for (SlotIndex Use : Uses) {
    if (!isIdenticalAtUse(*DefVN, Mask, Use, LI))
      return false;
  }
  return true;
}

RegisterIdx Rematerializer::findRematInRegion(RegisterIdx RegIdx,
                                              unsigned Region,
                                              SlotIndex Before) const {
  auto It = Rematerializations.find(getOriginOrSelf(RegIdx));
  if (It == Rematerializations.end())
    return NoReg;
  const RematsOf &Remats = It->getSecond();

  SlotIndex BestSlot;
  RegisterIdx BestRegIdx = NoReg;
  for (RegisterIdx RematRegIdx : Remats) {
    const Reg &RematReg = getReg(RematRegIdx);
    if (RematReg.DefRegion != Region || RematReg.Uses.empty())
      continue;
    SlotIndex RematRegSlot =
        LIS.getInstructionIndex(*RematReg.getLastDef()).getRegSlot();
    if (RematRegSlot < Before &&
        (BestRegIdx == NoReg || RematRegSlot > BestSlot)) {
      BestSlot = RematRegSlot;
      BestRegIdx = RematRegIdx;
    }
  }
  return BestRegIdx;
}

void Rematerializer::deleteRegIfUnused(RegisterIdx RootIdx) {
  if (!getReg(RootIdx).Uses.empty())
    return;

  // Traverse the root's dependency DAG depth-first to find the set of registers
  // we can delete and a legal order to delete them in.
  SmallVector<RegisterIdx, 4> DepDAG{RootIdx};
  SmallSetVector<RegisterIdx, 8> DeleteOrder;
  DeleteOrder.insert(RootIdx);
  do {
    // A deleted register's dependencies may be deletable too.
    const Reg &DeleteReg = getReg(DepDAG.pop_back_val());
    for (RegisterIdx DepRegIdx : DeleteReg.Dependencies) {
      // All dependencies lose a user (the deleted register).
      Reg &DepReg = Regs[DepRegIdx];
      for (MachineInstr *DefMI : DeleteReg.Defs) {
        if (DepReg.tryEraseUser(DefMI, DeleteReg.DefRegion) &&
            DepReg.Uses.empty()) {
          DeleteOrder.insert(DepRegIdx);
          DepDAG.push_back(DepRegIdx);
          break;
        }
      }
    }
  } while (!DepDAG.empty());

  for (RegisterIdx RegIdx : reverse(DeleteOrder)) {
    Reg &DeleteReg = Regs[RegIdx];

    // It is possible that the defined register we are deleting doesn't have an
    // interval yet if the LIS hasn't been updated since it was created.
    Register DefReg = DeleteReg.getDefReg();
    if (LIS.hasInterval(DefReg))
      LIS.removeInterval(DefReg);
    LISUpdates.erase(RegIdx);

    deleteReg(RegIdx);
    if (isRematerializedRegister(RegIdx)) {
      // Delete rematerialized register from its origin's rematerializations.
      const RegisterIdx OriginIdx = getOriginOf(RegIdx);
      RematsOf &OriginRemats = Rematerializations.at(OriginIdx);
      assert(OriginRemats.contains(RegIdx) && "broken remat<->origin link");
      OriginRemats.erase(RegIdx);
      if (OriginRemats.empty())
        Rematerializations.erase(OriginIdx);
    }
    LLVM_DEBUG(dbgs() << "** Deleted " << printID(RegIdx) << "\n");
  }
}

void Rematerializer::deleteReg(RegisterIdx RegIdx) {
  noteRegDeleted(RegIdx);

  Reg &DeleteReg = Regs[RegIdx];
  assert(DeleteReg.getFirstDef() && "register was already deleted");

  MachineBasicBlock::iterator &RegionBegin = Regions[DeleteReg.DefRegion].first;
  for (MachineInstr *DefMI : DeleteReg.Defs) {
    // It is not possible for the deleted instruction to be the upper region
    // boundary since we don't ever consider them rematerializable.
    if (RegionBegin == DefMI)
      RegionBegin = std::next(MachineBasicBlock::iterator(DefMI));
    LIS.RemoveMachineInstrFromMaps(*DefMI);
    DefMI->eraseFromParent();
  }
  DeleteReg.Defs.clear();
}

Rematerializer::Rematerializer(MachineFunction &MF,
                               SmallVectorImpl<RegionBoundaries> &Regions,
                               LiveIntervals &LIS)
    : Regions(Regions), MRI(MF.getRegInfo()), LIS(LIS),
      TII(*MF.getSubtarget().getInstrInfo()), TRI(TII.getRegisterInfo()) {
#ifdef EXPENSIVE_CHECKS
  // Check that regions are valid.
  DenseSet<MachineInstr *> SeenMIs;
  for (const auto &[RegionBegin, RegionEnd] : Regions) {
    assert(RegionBegin != RegionEnd && "empty region");
    for (auto MI = RegionBegin; MI != RegionEnd; ++MI) {
      bool IsNewMI = SeenMIs.insert(&*MI).second;
      assert(IsNewMI && "overlapping regions");
      assert(!MI->isTerminator() && "terminator in region");
    }
    if (RegionEnd != RegionBegin->getParent()->end()) {
      bool IsNewMI = SeenMIs.insert(&*RegionEnd).second;
      assert(IsNewMI && "overlapping regions (upper bound)");
    }
  }
#endif
}

bool Rematerializer::analyze() {
  Regs.clear();
  UnrematableDeps.clear();
  Origins.clear();
  Rematerializations.clear();
  RegionMBB.clear();
  RegToIdx.clear();
  LISUpdates.clear();
  if (Regions.empty())
    return false;

  /// Maps all MIs to their parent region. Region terminators are considered
  /// part of the region they terminate.
  DenseMap<MachineInstr *, unsigned> MIRegion;

  // Initialize MI to containing region mapping.
  RegionMBB.reserve(Regions.size());
  for (unsigned I = 0, E = Regions.size(); I < E; ++I) {
    RegionBoundaries Region = Regions[I];
    assert(Region.first != Region.second && "empty cannot be region");
    for (auto MI = Region.first; MI != Region.second; ++MI) {
      assert(!MIRegion.contains(&*MI) && "regions should not intersect");
      MIRegion.insert({&*MI, I});
    }
    MachineBasicBlock &MBB = *Region.first->getParent();
    RegionMBB.push_back(&MBB);

    // A terminator instruction is considered part of the region it terminates.
    if (Region.second != MBB.end()) {
      MachineInstr *RegionTerm = &*Region.second;
      assert(!MIRegion.contains(RegionTerm) && "regions should not intersect");
      MIRegion.insert({RegionTerm, I});
    }
  }

  const unsigned NumVirtRegs = MRI.getNumVirtRegs();
  BitVector SeenRegs(NumVirtRegs);
  for (unsigned I = 0, E = NumVirtRegs; I != E; ++I) {
    if (!SeenRegs[I])
      addRegIfRematerializable(I, MIRegion, SeenRegs);
  }
  assert(Regs.size() == UnrematableDeps.size());

  LLVM_DEBUG({
    for (RegisterIdx I = 0, E = getNumRegs(); I < E; ++I)
      dbgs() << printDependencyDAG(I) << '\n';
  });
  return !Regs.empty();
}

void Rematerializer::addRegIfRematerializable(
    unsigned VirtRegIdx, const DenseMap<MachineInstr *, unsigned> &MIRegion,
    BitVector &SeenRegs) {
  assert(!SeenRegs[VirtRegIdx] && "register already seen");
  Register DefReg = Register::index2VirtReg(VirtRegIdx);
  SeenRegs.set(VirtRegIdx);
  Reg RematReg;

  // Check that the register's definitions can be rematerialized.
  SmallPtrSet<MachineInstr *, 1> DefSet;
  for (MachineOperand &MO : MRI.def_operands(DefReg)) {
    MachineInstr &DefMI = *MO.getParent();
    // If a single MI has multiple defs for the same register, we don't need to
    // redo MI-based checks.
    if (!DefSet.insert(&DefMI).second)
      continue;

    // The defining MI must be rematerializable and in the same region as all
    // other defining MIs.
    if (!isMIRematerializable(DefMI))
      return;
    auto DefRegion = MIRegion.find(&DefMI);
    if (DefRegion == MIRegion.end())
      return;
    if (RematReg.Defs.empty())
      RematReg.DefRegion = DefRegion->getSecond();
    else if (RematReg.DefRegion != DefRegion->getSecond())
      return;
    RematReg.Defs.push_back(&DefMI);
  }
  if (RematReg.Defs.empty())
    return;

  // Order defining MIs by slot index.
  sort(RematReg.Defs, [&](MachineInstr *LHS, MachineInstr *RHS) {
    return LIS.getInstructionIndex(*LHS) < LIS.getInstructionIndex(*RHS);
  });
  SlotIndex LastDefSlot = LIS.getInstructionIndex(*RematReg.getLastDef());

  // Set the register's mask to all active lanes after the last def.
  const LiveInterval &DefLI = LIS.getInterval(DefReg);
  SlotIndex AfterLastDef = LastDefSlot.getRegSlot();
  if (DefLI.hasSubRanges()) {
    for (const LiveInterval::SubRange &SR : DefLI.subranges())
      if (SR.liveAt(AfterLastDef))
        RematReg.Mask |= SR.LaneMask;
  } else {
    RematReg.Mask = MRI.getMaxLaneMaskForVReg(DefReg);
  }

  // Collect the candidate's direct users, both rematerializable and
  // unrematerializable.
  const bool MoreThanOneDef = RematReg.Defs.size() > 1;
  for (MachineInstr &UseMI : MRI.use_nodbg_instructions(DefReg)) {
    // We are only interested in users that do not define part of the register.
    if (DefSet.contains(&UseMI))
      continue;
    // MIs outside provided regions cannot be tracked so the registers they use
    // are not safely rematerializable.
    auto UseRegion = MIRegion.find(&UseMI);
    if (UseRegion == MIRegion.end())
      return;
    // Disallow reads before the last def.
    if (MoreThanOneDef && RematReg.DefRegion == UseRegion->second &&
        LastDefSlot > LIS.getInstructionIndex(UseMI))
      return;

    RematReg.addUser(&UseMI, UseRegion->second);
  }
  if (RematReg.Uses.empty())
    return;

  // Collect the candidate's dependencies, rematerializable or not. If the same
  // rematerializable register is used multiple times we just need to consider
  // it once.
  SmallDenseSet<Register, 4> AllDepRegs;
  SmallMapVector<Register, LaneBitmask, 2> UnrematDeps;
  for (auto [DefIdx, DefMI] : enumerate(RematReg.Defs)) {
    for (const MachineOperand &MO : DefMI->all_uses()) {
      Register DepReg = getRegDependency(MO);
      if (!DepReg || DepReg == DefReg || !AllDepRegs.insert(DepReg).second)
        continue;
      unsigned DepRegIdx = DepReg.virtRegIndex();
      if (!SeenRegs[DepRegIdx])
        addRegIfRematerializable(DepRegIdx, MIRegion, SeenRegs);
      if (auto DepIt = RegToIdx.find(DepReg); DepIt != RegToIdx.end()) {
        RematReg.Dependencies.push_back(DepIt->second);
      } else {
        LaneBitmask &CurrentMask =
            UnrematDeps.try_emplace(DepReg, LaneBitmask::getNone())
                .first->second;
        if (MoreThanOneDef && CurrentMask.none()) {
          // A def of an unrematerializable operand between the defs of the
          // rematerializable register makes the latter unrematerializable.
          SlotIndex FirstDefSlot =
              LIS.getInstructionIndex(*RematReg.getFirstDef());
          for (MachineOperand &UnrematMODef : MRI.def_operands(DepReg)) {
            MachineInstr &UnrematDefMI = *UnrematMODef.getParent();
            SlotIndex UnrematDefSlot = LIS.getInstructionIndex(UnrematDefMI);
            if (UnrematDefSlot > FirstDefSlot || UnrematDefSlot < LastDefSlot)
              return;
          }
        }
        LaneBitmask Mask = MO.getSubReg()
                               ? TRI.getSubRegIndexLaneMask(MO.getSubReg())
                               : MRI.getMaxLaneMaskForVReg(DepReg);
        CurrentMask |= Mask;
      }
    }
  }

  // The register is rematerializable.
  RegToIdx.insert({DefReg, Regs.size()});
  Regs.push_back(RematReg);
  UnrematableDeps.push_back(UnrematDeps.takeVector());
}

bool Rematerializer::isMIRematerializable(const MachineInstr &MI) const {
  if (!TII.isReMaterializable(MI))
    return false;

  assert(MI.getOperand(0).getReg().isVirtual() && "should be virtual");

  for (const MachineOperand &MO : MI.all_uses()) {
    // We can't remat physreg uses, unless it is a constant or an ignorable
    // use (e.g. implicit exec use on VALU instructions)
    if (MO.getReg().isPhysical()) {
      if (MRI.isConstantPhysReg(MO.getReg()) || TII.isIgnorableUse(MO))
        continue;
      return false;
    }
  }

  return true;
}

RegisterIdx Rematerializer::getDefRegIdx(const MachineInstr &MI) const {
  if (!MI.getNumOperands() || !MI.getOperand(0).isReg() ||
      !MI.getOperand(0).isDef())
    return NoReg;
  Register Reg = MI.getOperand(0).getReg();
  auto UserRegIt = RegToIdx.find(Reg);
  if (UserRegIt == RegToIdx.end())
    return NoReg;
  return UserRegIt->second;
}

RegisterIdx
Rematerializer::rematerializeReg(RegisterIdx RegIdx, unsigned UseRegion,
                                 MachineBasicBlock::iterator InsertPos,
                                 SmallVectorImpl<RegisterIdx> &&Dependencies) {
  RegisterIdx NewRegIdx = Regs.size();

  Reg &NewReg = Regs.emplace_back();
  Reg &FromReg = Regs[RegIdx];
  NewReg.Mask = FromReg.Mask;
  NewReg.DefRegion = UseRegion;
  NewReg.Defs.reserve(FromReg.Defs.size());
  NewReg.Dependencies = std::move(Dependencies);

  // Track rematerialization link between registers. Origins are always
  // registers that existed originally, and rematerializations are always
  // attached to them.
  const RegisterIdx OriginIdx = getOriginOrSelf(RegIdx);
  Origins.push_back(OriginIdx);
  Rematerializations[OriginIdx].insert(NewRegIdx);

  // Use the TII to rematerialize the defining instruction with a new defined
  // register.
  Register NewDefReg = MRI.cloneVirtualRegister(FromReg.getDefReg());
  for (const MachineInstr *DefMI : FromReg.Defs) {
    TII.reMaterialize(*RegionMBB[UseRegion], InsertPos, NewDefReg, 0, *DefMI);
    NewReg.Defs.push_back(&*std::prev(InsertPos));
  }
  RegToIdx.insert({NewDefReg, NewRegIdx});
  postRematerialization(RegIdx, NewRegIdx);

  noteRegCreated(NewRegIdx);
  LLVM_DEBUG(dbgs() << "** Rematerialized " << printID(RegIdx) << " as "
                    << printRematReg(NewRegIdx) << '\n');
  return NewRegIdx;
}

void Rematerializer::recreateReg(
    RegisterIdx RegIdx, ArrayRef<MachineBasicBlock::iterator> Positions,
    Register DefReg, SmallVectorImpl<RegisterIdx> &&Dependencies) {
  assert(RegToIdx.contains(DefReg) && "unknown defined register");
  assert(RegToIdx.at(DefReg) == RegIdx && "incorrect defined register");
  assert(!getReg(RegIdx).isAlive() && "register is still alive");

  Reg &OriginReg = Regs[RegIdx];
  OriginReg.Dependencies = std::move(Dependencies);

  // Re-establish the link between origin and rematerialization if necessary.
  const bool RecreateOriginalReg = isOriginalRegister(RegIdx);
  if (!RecreateOriginalReg)
    Rematerializations[getOriginOf(RegIdx)].insert(RegIdx);

  // Rematerialize from one of the existing rematerializations or from the
  // origin. We expect at least one to exist, otherwise it would mean the value
  // held by the original register is no longer available anywhere in the MF.
  RegisterIdx ModelRegIdx;
  if (RecreateOriginalReg) {
    assert(Rematerializations.contains(RegIdx) && "expected remats");
    ModelRegIdx = *Rematerializations.at(RegIdx).begin();
  } else {
    assert(getReg(getOriginOf(RegIdx)).isAlive() && "expected alive origin");
    ModelRegIdx = getOriginOf(RegIdx);
  }
  const Reg &ModelReg = getReg(ModelRegIdx);
  for (auto [DefMI, InsertPos] : zip_equal(ModelReg.Defs, Positions)) {
    TII.reMaterialize(*RegionMBB[OriginReg.DefRegion], InsertPos, DefReg, 0,
                      *DefMI);
    OriginReg.Defs.push_back(&*std::prev(InsertPos));
  }
  postRematerialization(ModelRegIdx, RegIdx);
  LLVM_DEBUG(dbgs() << "** Recreated " << printID(RegIdx) << " as "
                    << printRematReg(RegIdx) << '\n');
}

void Rematerializer::postRematerialization(RegisterIdx ModelRegIdx,
                                           RegisterIdx RematRegIdx) {
  Reg &ModelReg = Regs[ModelRegIdx], &RematReg = Regs[RematRegIdx];
  for (MachineInstr *DefMI : RematReg.Defs)
    LIS.InsertMachineInstrInMaps(*DefMI);

  // The start of the new register's region may have changed.
  MachineInstr &FirstDefMI = *RematReg.getFirstDef();
  auto &[RegionBegin, RegionEnd] = Regions[RematReg.DefRegion];
  if (RegionBegin == RegionEnd ||
      (!RegionBegin->isDebugInstr() && LIS.getInstructionIndex(*RegionBegin) >
                                           LIS.getInstructionIndex(FirstDefMI)))
    RegionBegin = FirstDefMI.getIterator();

  // Replace dependencies as needed in the rematerialized MI. All dependencies
  // of the latter gain a new user.
  auto ZipedDeps = zip_equal(ModelReg.Dependencies, RematReg.Dependencies);
  for (const auto &[OldDepRegIdx, NewDepRegIdx] : ZipedDeps) {
    LLVM_DEBUG(dbgs() << "  Dependency: " << printID(OldDepRegIdx) << " -> "
                      << printID(NewDepRegIdx) << '\n');

    const bool DifferentDep = OldDepRegIdx != NewDepRegIdx;
    Reg &NewDepReg = Regs[NewDepRegIdx];
    Register OldReg = getReg(OldDepRegIdx).getDefReg();
    Register NewReg = NewDepReg.getDefReg();
    for (MachineInstr *DefMI : RematReg.Defs) {
      bool HasReg = false;
      for (MachineOperand &MO : DefMI->operands()) {
        if (!MO.isReg() || MO.getReg() != OldReg)
          continue;
        if (DifferentDep)
          MO.substVirtReg(NewReg, 0, TRI);
        HasReg = true;
      }
      if (HasReg)
        NewDepReg.addUser(DefMI, RematReg.DefRegion);
    }
    LISUpdates.insert(OldDepRegIdx);
    if (DifferentDep)
      LISUpdates.insert(NewDepRegIdx);
  }
}

std::pair<MachineInstr *, MachineInstr *>
Rematerializer::Reg::getRegionUseBounds(unsigned UseRegion,
                                        const LiveIntervals &LIS) const {
  auto It = Uses.find(UseRegion);
  if (It == Uses.end())
    return {nullptr, nullptr};
  const RegionUsers &RegionUsers = It->getSecond();
  assert(!RegionUsers.empty() && "empty userset in region");

  auto User = RegionUsers.begin(), UserEnd = RegionUsers.end();
  MachineInstr *FirstMI = *User, *LastMI = FirstMI;
  SlotIndex FirstIndex = LIS.getInstructionIndex(*FirstMI),
            LastIndex = FirstIndex;

  while (++User != UserEnd) {
    SlotIndex UserIndex = LIS.getInstructionIndex(**User);
    if (UserIndex < FirstIndex) {
      FirstIndex = UserIndex;
      FirstMI = *User;
    } else if (UserIndex > LastIndex) {
      LastIndex = UserIndex;
      LastMI = *User;
    }
  }

  return {FirstMI, LastMI};
}

void Rematerializer::Reg::addUser(MachineInstr *MI, unsigned Region) {
  Uses[Region].insert(MI);
}

void Rematerializer::Reg::addUsers(const RegionUsers &NewUsers,
                                   unsigned Region) {
  Uses[Region].insert_range(NewUsers);
}

void Rematerializer::Reg::eraseUser(MachineInstr *MI, unsigned Region) {
  RegionUsers &RUsers = Uses.at(Region);
  assert(RUsers.contains(MI) && "user not in region");
  if (RUsers.size() == 1)
    Uses.erase(Region);
  else
    RUsers.erase(MI);
}

bool Rematerializer::Reg::tryEraseUser(MachineInstr *MI, unsigned Region) {
  auto RegionUsers = Uses.find(Region);
  if (RegionUsers == Uses.end() || !RegionUsers->getSecond().erase(MI))
    return false;
  if (RegionUsers->getSecond().empty())
    Uses.erase(Region);
  return true;
}

Printable Rematerializer::printDependencyDAG(RegisterIdx RootIdx) const {
  return Printable([&, RootIdx](raw_ostream &OS) {
    DenseMap<RegisterIdx, unsigned> RegDepths;
    std::function<void(RegisterIdx, unsigned)> WalkTree =
        [&](RegisterIdx RegIdx, unsigned Depth) -> void {
      unsigned MaxDepth = std::max(RegDepths.lookup_or(RegIdx, Depth), Depth);
      RegDepths.emplace_or_assign(RegIdx, MaxDepth);
      for (RegisterIdx DepRegIdx : getReg(RegIdx).Dependencies)
        WalkTree(DepRegIdx, Depth + 1);
    };
    WalkTree(RootIdx, 0);

    // Sort in decreasing depth order to print root at the bottom.
    SmallVector<std::pair<RegisterIdx, unsigned>> Regs(RegDepths.begin(),
                                                       RegDepths.end());
    sort(Regs, [](const auto &LHS, const auto &RHS) {
      return LHS.second > RHS.second;
    });

    OS << printID(RootIdx) << " has " << Regs.size() - 1 << " dependencies\n";
    for (const auto &[RegIdx, Depth] : Regs) {
      OS << indent(Depth, 2) << (Depth ? '|' : '*') << ' '
         << printRematReg(RegIdx, /*SkipRegions=*/Depth) << '\n';
    }
    OS << printRegUsers(RootIdx);
  });
}

Printable Rematerializer::printID(RegisterIdx RegIdx) const {
  return Printable([&, RegIdx](raw_ostream &OS) {
    const Reg &PrintReg = getReg(RegIdx);
    OS << '(' << RegIdx << '/';
    if (!PrintReg.isAlive())
      OS << "<dead>";
    else
      OS << printReg(PrintReg.getDefReg(), &TRI, 0, &MRI);
    OS << ")[" << PrintReg.DefRegion << "]";
  });
}

Printable Rematerializer::printRematReg(RegisterIdx RegIdx,
                                        bool SkipRegions) const {
  return Printable([&, RegIdx, SkipRegions](raw_ostream &OS) {
    const Reg &PrintReg = getReg(RegIdx);
    OS << printID(RegIdx);
    if (!SkipRegions) {
      OS << " [" << PrintReg.DefRegion;
      if (!PrintReg.Uses.empty()) {
        assert(PrintReg.isAlive() && "dead register cannot have uses");
        const LiveInterval &LI = LIS.getInterval(PrintReg.getDefReg());
        // First display all regions in which the register is live-through and
        // not used.
        bool First = true;
        for (const auto &[I, Bounds] : enumerate(Regions)) {
          if (PrintReg.Uses.contains(I))
            continue;
          // The register must be live at the live-ins and live-outs of the
          // region.
          MachineBasicBlock::iterator LiveIn =
              skipDebugInstructionsForward(Bounds.first, Bounds.second);
          if (LiveIn == Bounds.second) {
            // The region has no non-debug instructions, it's hard to assess
            // whether the register is live across it without an index.
            continue;
          }
          // LiveIn is inside the range and a non-debug instruction so we know
          // this will also point to a non-debug instruction within the region.
          MachineBasicBlock::iterator LiveOut = skipDebugInstructionsBackward(
              std::prev(Bounds.second), Bounds.first);
          if (LI.liveAt(LIS.getInstructionIndex(*LiveIn)) &&
              LI.liveAt(LIS.getInstructionIndex(*LiveOut).getDeadSlot())) {
            OS << (First ? " - " : ",") << I;
            First = false;
          }
        }
        OS << (First ? " --> " : " -> ");

        // Then display regions in which the register is used.
        auto It = PrintReg.Uses.begin();
        OS << It->first;
        while (++It != PrintReg.Uses.end())
          OS << "," << It->first;
      }
      OS << "] ";
    }
    if (PrintReg.isAlive()) {
      OS << ' ';
      PrintReg.getFirstDef()->print(OS, /*IsStandalone=*/true,
                                    /*SkipOpers=*/false,
                                    /*SkipDebugLoc=*/false,
                                    /*AddNewLine=*/false);
      OS << " @ ";
      LIS.getInstructionIndex(*PrintReg.getFirstDef()).print(OS);
      if (PrintReg.Defs.size() > 1)
        OS << " [...other defs...]";
    }
  });
}

Printable Rematerializer::printRegUsers(RegisterIdx RegIdx) const {
  return Printable([&, RegIdx](raw_ostream &OS) {
    for (const auto &[UseRegion, Users] : getReg(RegIdx).Uses) {
      for (MachineInstr *MI : Users)
        OS << "  User " << printUser(MI, UseRegion) << '\n';
    }
  });
}

Printable Rematerializer::printUser(const MachineInstr *MI,
                                    std::optional<unsigned> UseRegion) const {
  return Printable([&, MI, UseRegion](raw_ostream &OS) {
    RegisterIdx RegIdx = getDefRegIdx(*MI);
    if (RegIdx != NoReg) {
      OS << printID(RegIdx);
    } else {
      OS << "(-/-)[";
      if (UseRegion)
        OS << *UseRegion;
      else
        OS << '?';
      OS << ']';
    }
    OS << ' ';
    MI->print(OS, /*IsStandalone=*/true, /*SkipOpers=*/false,
              /*SkipDebugLoc=*/false, /*AddNewLine=*/false);
    OS << " @ ";
    LIS.getInstructionIndex(*MI).print(OS);
  });
}

Rollbacker::RollbackInfo::RollbackInfo(const Rematerializer::Reg &Reg,
                                       SmallVector<Position, 1> &&Positions)
    : DefReg(Reg.getDefReg()), Dependencies(Reg.Dependencies),
      Positions(std::move(Positions)) {}

void Rollbacker::rematerializerNoteRegCreated(const Rematerializer &Remater,
                                              RegisterIdx RegIdx) {
  if (RollingBack)
    return;
  Rematerializations[Remater.getOriginOf(RegIdx)].insert(RegIdx);
}

void Rollbacker::rematerializerNoteRegDeleted(const Rematerializer &Remater,
                                              RegisterIdx RegIdx) {
  if (RollingBack || Remater.isRematerializedRegister(RegIdx))
    return;
  const Rematerializer::Reg &Reg = Remater.getReg(RegIdx);

  SmallVector<RollbackInfo::Position, 1> Positions(Reg.Defs.size());
  for (const auto [DefIdx, DefMI] : enumerate(Reg.Defs)) {
    RematMI ThisDef(RegIdx, static_cast<unsigned>(DefIdx));
    RollbackInfo::Position &Pos = Positions[DefIdx];
    Pos.InsertBefore = std::next(DefMI->getIterator());
    auto &[NextRegIdx, NextDefIdx] = Pos.NextRematMI;

    // If the MI that was originally after the one defining the register is
    // rematerializable, derive its corresponding register and definition index.
    NextRegIdx = Rematerializer::NoReg;
    auto KnownNextRegIdx = AdjacentDeletedMIs.find(ThisDef);
    if (KnownNextRegIdx != AdjacentDeletedMIs.end()) {
      Pos.NextRematMI = KnownNextRegIdx->second;
    } else {
      if (Pos.InsertBefore != DefMI->getParent()->end()) {
        const MachineInstr *NextMI = &*Pos.InsertBefore;
        NextRegIdx = Remater.getDefRegIdx(*NextMI);
        if (NextRegIdx == RegIdx) {
          // Defining instructions are stored in program order so when the next
          // instruction defines the same register as the one that is being
          // deleted it is necessarily the next defining MI in the vector.
          assert(Reg.Defs[DefIdx + 1] == NextMI && "inconsistent def MI");
          NextDefIdx = DefIdx + 1;
        } else if (NextRegIdx != Rematerializer::NoReg) {
          const Rematerializer::Reg &NextReg = Remater.getReg(NextRegIdx);
          NextDefIdx =
              std::distance(NextReg.Defs.begin(), find(NextReg.Defs, NextMI));
          assert(NextDefIdx != NextReg.Defs.size() && "cannot find def");
        }
      }
    }

    // Keep track of adjacent rematerializable MIs to allow re-creation in the
    // same exact order regardless of rematerialization order.
    if (DefMI->getParent()->begin() != DefMI->getIterator()) {
      const MachineInstr *PrevMI = &*std::prev(DefMI->getIterator());
      RegisterIdx PrevRegIdx = Remater.getDefRegIdx(*PrevMI);
      if (PrevRegIdx != Rematerializer::NoReg) {
        const Rematerializer::Reg &PrevReg = Remater.getReg(PrevRegIdx);
        unsigned PrevDefIdx =
            std::distance(PrevReg.Defs.begin(), find(PrevReg.Defs, PrevMI));
        assert(PrevDefIdx != PrevReg.Defs.size() && "cannot find def");

        // The key might already be in the map, which indicates that there were
        // originally instructions in between the MIs which have since then been
        // deleted. In these cases we leave the value untouched, as we care
        // about MIs that were adjacent in the original instruction order.
        AdjacentDeletedMIs.try_emplace({PrevRegIdx, PrevDefIdx}, ThisDef);
      }
    }
  }

  DeadRegs.try_emplace(RegIdx, Reg, std::move(Positions));
}

void Rollbacker::rollback(Rematerializer &Remater) {
  RollingBack = true;

  // Re-create deleted registers.
  for (auto &[RegIdx, Info] : DeadRegs) {
    assert(!Remater.getReg(RegIdx).isAlive() && "register should be dead");

    SmallVector<MachineBasicBlock::iterator, 1> Positions;
    // Make a copy here because we may modify it.
    for (RollbackInfo::Position Pos : Info.Positions) {
      auto &[NextRegIdx, NextDefIdx] = Pos.NextRematMI;

      // The MI that was originally just after the MI defining the register we
      // are trying to re-create may have been deleted. In such cases, we can
      // re-create at that MI's own insert position (and apply the same logic
      // recursively).
      while (NextRegIdx != Rematerializer::NoReg) {
        const Rematerializer::Reg &MaybeAliveReg = Remater.getReg(NextRegIdx);
        // When the next MI is alive (including when it was dead and already
        // re-created), we must use it as the position to insert before.
        if (MaybeAliveReg.isAlive()) {
          Pos.InsertBefore = MaybeAliveReg.Defs[NextDefIdx]->getIterator();
          break;
        }
        Pos = DeadRegs.at(NextRegIdx).Positions[NextDefIdx];
      }
      Positions.push_back(Pos.InsertBefore);
    }
    Remater.recreateReg(RegIdx, Positions, Info.DefReg,
                        std::move(Info.Dependencies));
  }

  // Rollback rematerializations.
  for (const auto &[RegIdx, RematsOf] : Rematerializations) {
    for (RegisterIdx RematRegIdx : RematsOf) {
      // It is possible that rematerializations were deleted. Their users would
      // have been transfered to some other rematerialization so we can safely
      // ignore them. Original registers that were deleted were just re-created
      // so we do not need to check for that.
      if (Remater.getReg(RematRegIdx).isAlive())
        Remater.transferAllUsers(RematRegIdx, RegIdx);
    }
  }

  Remater.updateLiveIntervals();
  DeadRegs.clear();
  Rematerializations.clear();
  AdjacentDeletedMIs.clear();
  RollingBack = false;
}
