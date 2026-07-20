void stop() {}

int main(int argc, char **argv) {
  int i = 1;
  short sh = 1;
  double d = 1.0;
  int *ptr = &i;
  int &iref = i;
  int arr[] = {1, 2, 3};

  struct SizeOfFoo {
    int x, y;
    double d;
  } foo;
  SizeOfFoo *foo_ptr = &foo;

  auto int_size = sizeof(int);
  auto short_size = sizeof(short);
  auto double_size = sizeof(double);
  auto ptr_size = sizeof(int *);
  auto intref_size = sizeof(int &);
  auto arr_size = sizeof(arr);
  auto foo_size = sizeof(SizeOfFoo);

  stop(); // Set a breakpoint here
  return 0;
}
