// A multi-arch driver command lowers to two -cc1 jobs for one TU. With macro
// canonicalization enabled, BOTH emitted -cc1 command lines must be
// canonicalized identically: for `-DB=2 -DA=1 -UB -DC=3` the trailing `-UB`
// wins, so the redundant `B=2` define is dropped.

// REQUIRES: system-darwin
// RUN: rm -rf %t
// RUN: split-file %s %t
// RUN: sed -e "s|DIR|%/t|g" %t/cdb.json.in > %t/cdb.json

// RUN: clang-scan-deps -compilation-database %t/cdb.json \
// RUN:   -format experimental-full -optimize-args=canonicalize-macros -j 1 \
// RUN:   > %t/result.json

// RUN: FileCheck %s < %t/result.json

// No emitted -cc1 command line may retain the redundant B=2 define.
// CHECK-NOT: B=2

//--- cdb.json.in
[{
  "directory": "DIR",
  "command": "clang -c DIR/main.c -o DIR/main.o -arch x86_64 -arch arm64 -DB=2 -DA=1 -UB -DC=3",
  "file": "DIR/main.c"
}]

//--- main.c
int main(void) { return 0; }
