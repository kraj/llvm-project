// Add this to AArch64FrameLowering.cpp in orderFrameObjects()

// New: Track frame indices used together in function calls
DenseMap<std::pair<int, int>, unsigned> CallSitePairs;

// In the instruction scanning loop:
for (auto &MI : MBB) {
  // Check if this is a call instruction
  if (MI.isCall()) {
    // Collect all frame indices used as arguments to this call
    SmallVector<int, 4> CallFrameIndices;

    // Look at the instructions before the call that set up arguments
    for (auto It = std::prev(MI.getIterator());
         It != MBB.begin() && CallFrameIndices.size() < 8; // Max 8 args
         --It) {
      if (It->isDebugInstr()) continue;

      // Check if this instruction loads from a frame index
      for (const MachineOperand &MO : It->operands()) {
        if (MO.isFI()) {
          int FI = MO.getIndex();
          if (FrameObjects[FI].IsValid) {
            CallFrameIndices.push_back(FI);
          }
        }
      }

      // Stop if we hit another call or branch
      if (It->isCall() || It->isTerminator())
        break;
    }

    // Create pairs from consecutive arguments
    for (size_t i = 0; i + 1 < CallFrameIndices.size(); i++) {
      int FI1 = CallFrameIndices[i];
      int FI2 = CallFrameIndices[i + 1];

      // Check size compatibility
      if (MFI.getObjectSize(FI1) == MFI.getObjectSize(FI2)) {
        int Min = std::min(FI1, FI2);
        int Max = std::max(FI1, FI2);
        CallSitePairs[{Min, Max}] += 10; // High weight for call site pairing
      }
    }
  }
}