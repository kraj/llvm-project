// Immediate fix for x8/x9 regression
// Add this to orderFrameObjects() in AArch64FrameLowering.cpp
// This is a focused fix specifically for the x8/x9 pattern

// Add after the line: DenseMap<int, unsigned> FrameIndexRegType;
DenseMap<int, Register> FrameIndexPhysReg; // Track physical register for spills

// In the instruction scanning loop, add:
for (auto &MI : MBB) {
  if (MI.isDebugInstr())
    continue;

  // Track physical register spills/reloads
  int FI = -1;
  Register PhysReg = AArch64::NoRegister;

  // Check for spill
  unsigned SpillFI;
  if (TII->isStoreToStackSlot(MI, SpillFI) && SpillFI < MFI.getObjectIndexEnd()) {
    FI = SpillFI;
    Register Reg = MI.getOperand(0).getReg();
    if (Reg.isPhysical()) {
      PhysReg = Reg;
    }
  }

  // Check for reload
  unsigned LoadFI;
  if (TII->isLoadFromStackSlot(MI, LoadFI) && LoadFI < MFI.getObjectIndexEnd()) {
    FI = LoadFI;
    Register Reg = MI.getOperand(0).getReg();
    if (Reg.isPhysical()) {
      PhysReg = Reg;
    }
  }

  // Track the physical register associated with this frame index
  if (FI >= 0 && PhysReg != AArch64::NoRegister && FrameObjects[FI].IsValid) {
    FrameIndexPhysReg[FI] = PhysReg;
  }

  // Continue with existing proximity tracking...
  // (existing code)
}

// After proximity analysis, add special handling for x8/x9:
// Find all frame indices that hold x8 and x9
SmallVector<int, 8> X8Indices, X9Indices;
for (const auto &Entry : FrameIndexPhysReg) {
  if (Entry.second == AArch64::X8)
    X8Indices.push_back(Entry.first);
  else if (Entry.second == AArch64::X9)
    X9Indices.push_back(Entry.first);
}

// Pair up x8 and x9 slots based on proximity in allocation
for (int X8FI : X8Indices) {
  for (int X9FI : X9Indices) {
    // Check if they're the same size (should be 8 bytes for GPRs)
    if (MFI.getObjectSize(X8FI) == MFI.getObjectSize(X9FI) &&
        MFI.getObjectSize(X8FI) == 8) {

      // Give them a very high pairing score
      int Min = std::min(X8FI, X9FI);
      int Max = std::max(X8FI, X9FI);

      // Override any existing score with a very high value
      PairScores[{Min, Max}] = 1000; // Ensure they pair
    }
  }
}

// Similarly handle other common register pairs if needed:
// - x0/x1 (often used for return values or first two arguments)
// - x2/x3 (next two arguments)
// - d0/d1 (floating point pairs)
// etc.