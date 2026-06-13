void foo(bool cond) {
  if (cond) {
  }
}

// LCOV_EXCL_STOP

void bar() {
}

int main() {
  foo(false);
  bar();
  return 0;
}
