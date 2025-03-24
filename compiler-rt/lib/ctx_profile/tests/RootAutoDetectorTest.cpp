#include "../RootAutoDetector.h"
#include "sanitizer_common/sanitizer_array_ref.h"
#include "gtest/gtest.h"

using namespace __ctx_profile;

class MockCallsiteTree final : public PerThreadCallsiteTrie {
  // Return the first multiple of 100.
  uptr getFctStartAddr(uptr CallsiteAddress) const override {
    return (CallsiteAddress / 100) * 100;
  }
};

class Marker {
  enum class Kind { End, Value, Split };
  const uptr Value;
  const Kind K;
  Marker(uptr V, Kind S) : Value(V), K(S) {}

public:
  Marker(uptr V) : Marker(V, Kind::Value) {}

  static Marker split(uptr V) { return Marker(V, Kind::Split); }
  static Marker term() { return Marker(0, Kind::End); }

  bool isSplit() const { return K == Kind::Split; }
  bool isTerm() const { return K == Kind::End; }
  bool isVal() const { return K == Kind::Value; }

  bool operator==(const Marker &M) const {
    return Value == M.Value && K == M.K;
  }
};

void popAndCheck(ArrayRef<Marker> &Preorder, Marker M) {
  ASSERT_FALSE(Preorder.empty());
  ASSERT_EQ(Preorder[0], M);
  Preorder = Preorder.drop_front();
}

void checkSameImpl(const Trie &T, ArrayRef<Marker> &Preorder) {
  popAndCheck(Preorder, T.address());

  if (T.children().size() == 0) {
    popAndCheck(Preorder, Marker::term());
    return;
  }

  if (T.children().size() > 1)
    popAndCheck(Preorder, Marker::split(T.children().size()));

  T.children().forEach([&](const auto &KVP) {
    checkSameImpl(KVP.second, Preorder);
    return true;
  });
}

void checkSame(const PerThreadCallsiteTrie &RCT, ArrayRef<Marker> Preorder) {
  checkSameImpl(RCT.start(), Preorder);
  ASSERT_TRUE(Preorder.empty());
}

TEST(PerThreadCallsiteTrieTest, Insert) {
  PerThreadCallsiteTrie R;
  uptr Stack1[]{4, 3, 2, 1};
  R.insertStack(StackTrace(Stack1, 4));
  checkSame(R, ArrayRef<Marker>({0, 1, 2, 3, 4, Marker::term()}));

  uptr Stack2[]{5, 4, 3, 2, 1};
  R.insertStack(StackTrace(Stack2, 5));
  checkSame(R, ArrayRef<Marker>({0, 1, 2, 3, 4, 5, Marker::term()}));

  uptr Stack3[]{6, 3, 2, 1};
  R.insertStack(StackTrace(Stack3, 4));
  checkSame(R, ArrayRef<Marker>({0, 1, 2, 3, Marker::split(2), 4, 5,
                                 Marker::term(), 6, Marker::term()}));

  uptr Stack4[]{7, 2, 1};
  R.insertStack(StackTrace(Stack4, 3));
  checkSame(R, ArrayRef<Marker>({0, 1, 2, Marker::split(2), 7, Marker::term(),
                                 3, Marker::split(2), 4, 5, Marker::term(), 6,
                                 Marker::term()}));
}

TEST(PerThreadCallsiteTrieTest, DetectRoots) {
  MockCallsiteTree T;

  uptr Stack1[]{501, 302, 202, 102};
  uptr Stack2[]{601, 402, 203, 102};
  T.insertStack({Stack1, 4});
  T.insertStack({Stack2, 4});

  auto R = T.determineRoots();
  EXPECT_EQ(R.size(), 2U);
  EXPECT_TRUE(R.contains(300));
  EXPECT_TRUE(R.contains(400));
}

TEST(PerThreadCallsiteTrieTest, DetectRootsNoBranches) {
  MockCallsiteTree T;

  uptr Stack1[]{501, 302, 202, 102};
  T.insertStack({Stack1, 4});

  auto R = T.determineRoots();
  EXPECT_EQ(R.size(), 0U);
}

TEST(PerThreadCallsiteTrieTest, DetectRootsUnknownFct) {
  MockCallsiteTree T;

  uptr Stack1[]{501, 302, 202, 102};
  // The MockCallsiteTree address resolver resolves addresses over 100, so 40
  // will be mapped to 0.
  uptr Stack2[]{601, 40, 203, 102};
  T.insertStack({Stack1, 4});
  T.insertStack({Stack2, 4});

  auto R = T.determineRoots();
  EXPECT_EQ(R.size(), 2U);
  EXPECT_TRUE(R.contains(300));
  EXPECT_TRUE(R.contains(0));
}
