// RUN: rm -rf %t && mkdir -p %t
// RUN: split-file %s %t
// RUN: clang-doc -format=html %t/compile-commands.json %t/main.cpp

//--- main.cpp

class Foo {
  void getFoo();
};

int main() {
  return 0;
}

//--- compile-commands.json
[
{
  "directory": "foo",
  "file":"main.cpp",
  "command":"clang main.cpp -c"
}
]
