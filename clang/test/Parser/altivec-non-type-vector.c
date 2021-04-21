// RUN: %clang_cc1 -target-feature +altivec -fsyntax-only %s

int vector();

void test() {
  vector unsigned int v = {0};
}
