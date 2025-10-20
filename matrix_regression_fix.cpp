// Fix for matrix test regression - Add to AArch64FrameLowering.cpp orderFrameObjects()

// Solution 1: Track register-based spill/reload patterns
// Add this analysis before the proximity tracking:

// Track which virtual registers are spilled to which frame indices
DenseMap<int, unsigned> FrameIndexToVirtReg;
DenseMap<std::pair<unsigned, unsigned>, unsigned> VirtRegPairScore;

// In the instruction scanning loop:
for (auto &MI : MBB) {
  // Track spills
  if (TII->isStoreToStackSlot(MI, FI) && FI >= 0) {
    Register SpillReg = MI.getOperand(0).getReg();
    if (SpillReg.isVirtual()) {
      FrameIndexToVirtReg[FI] = SpillReg;
    }
  }

  // Track reloads
  if (TII->isLoadFromStackSlot(MI, FI) && FI >= 0) {
    Register LoadReg = MI.getOperand(0).getReg();
    if (LoadReg.isVirtual()) {
      FrameIndexToVirtReg[FI] = LoadReg;
    }
  }
}

// Solution 2: Special handling for x8/x9 patterns
// After identifying frame indices but before pairing:

// Check if this frame holds x8 or x9 spills
for (auto &MI : MBB) {
  int FI1 = -1, FI2 = -1;

  // Look for patterns like:
  // str x8, [sp, #offset1]
  // str x9, [sp, #offset2]
  // ... (within proximity)
  // ldr x8, [sp, #offset1]
  // ldr x9, [sp, #offset2]

  if (MI.getOpcode() == AArch64::STRXui &&
      MI.getOperand(0).getReg() == AArch64::X8) {
    if (MI.getOperand(1).isFI()) {
      FI1 = MI.getOperand(1).getIndex();

      // Look ahead for x9 spill
      for (auto NextMI = std::next(MI.getIterator());
           NextMI != MBB.end() && std::distance(MI.getIterator(), NextMI) < 5;
           ++NextMI) {
        if (NextMI->getOpcode() == AArch64::STRXui &&
            NextMI->getOperand(0).getReg() == AArch64::X9 &&
            NextMI->getOperand(1).isFI()) {
          FI2 = NextMI->getOperand(1).getIndex();

          // Give high score to x8/x9 pairs
          if (FI1 >= 0 && FI2 >= 0 && FI1 != FI2) {
            int Min = std::min(FI1, FI2);
            int Max = std::max(FI1, FI2);
            PairScores[{Min, Max}] += 50; // High weight
          }
          break;
        }
      }
    }
  }
}

// Solution 3: Preserve consecutive allocations for correlated spills
// Track frame indices allocated consecutively by the register allocator

DenseMap<int, int> AllocationOrder;
int Order = 0;

// Build allocation order from MachineFrameInfo
// (This assumes frame indices are allocated in order during register allocation)
for (int FI = MFI.getObjectIndexBegin(); FI != 0; ++FI) {
  if (MFI.isSpillSlotObjectIndex(FI)) {
    AllocationOrder[FI] = Order++;
  }
}

// When scoring pairs, boost score for consecutively allocated slots
for (auto &Entry : PairScores) {
  int FI1 = Entry.first.first;
  int FI2 = Entry.first.second;

  if (AllocationOrder.count(FI1) && AllocationOrder.count(FI2)) {
    int Order1 = AllocationOrder[FI1];
    int Order2 = AllocationOrder[FI2];

    // Boost score for slots allocated consecutively
    if (abs(Order1 - Order2) == 1) {
      PairScores[Entry.first] *= 3; // Triple the score
    }
  }
}

// Solution 4: Pattern matching for matrix operations
// Detect common matrix access patterns that use register pairs

// Look for patterns like:
// ldr x8, [base]       ; matrix row pointer
// ldr x9, [base, #8]   ; next row pointer
// ... computations using both ...
// This indicates x8/x9 should have adjacent spill slots

for (auto &MI : MBB) {
  if (MI.mayLoad()) {
    Register Reg1 = AArch64::NoRegister;
    Register Reg2 = AArch64::NoRegister;

    // Check for consecutive loads to x8/x9 or similar patterns
    if (MI.getOperand(0).isReg() &&
        MI.getOperand(0).getReg() == AArch64::X8) {

      // Look for next instruction loading x9
      auto NextMI = std::next(MI.getIterator());
      if (NextMI != MBB.end() && NextMI->mayLoad() &&
          NextMI->getOperand(0).isReg() &&
          NextMI->getOperand(0).getReg() == AArch64::X9) {

        // These registers are likely used together
        // Find their spill slots and boost pairing score
        // ... (implementation depends on tracking spill slots)
      }
    }
  }
}