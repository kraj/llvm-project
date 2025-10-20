// Add to orderFrameObjects() in AArch64FrameLowering.cpp

// Track the original allocation order from MachineFrameInfo
DenseMap<int, int> OriginalOrder;
int Order = 0;
for (auto &Obj : ObjectsToAllocate) {
  OriginalOrder[Obj] = Order++;
}

// When creating pairs, give bonus weight to originally-adjacent allocations
for (const auto &Entry : PairScores) {
  int FI1 = Entry.first.first;
  int FI2 = Entry.first.second;

  // Check if these were originally adjacent
  int Order1 = OriginalOrder[FI1];
  int Order2 = OriginalOrder[FI2];
  if (std::abs(Order1 - Order2) == 1) {
    // These were allocated consecutively - likely related
    // Double their pairing score
    PairScores[Entry.first] *= 2;
  }
}