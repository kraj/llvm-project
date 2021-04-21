// RUN: %clang_cc1 -fsyntax-only -target-feature +altivec %s

template <typename T> class vector {
public:
  vector(int) {}
};

void f() {
  vector int v = {0};
  vector<int> vi = {0};
}
