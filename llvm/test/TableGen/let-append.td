// RUN: llvm-tblgen %s | FileCheck %s
// XFAIL: vg_leak

// Test 'let append' and 'let prepend' syntax.

def op;

class Base {
  list<int> items = [1, 2];
  string text = "hello";
  dag d = (op);
}

// Defs are printed in alphabetical order.

// Test append with code type.
class WithCode {
  code body = [{ int x = 0; }];
}

// CHECK: def AppendCode
// CHECK: code body = [{ int x = 0;  int y = 1; }]
def AppendCode : WithCode {
  let append body = [{ int y = 1; }];
}

// CHECK: def AppendDag
// CHECK: dag d = (op 3:$a);
def AppendDag : Base {
  let append d = (op 3:$a);
}

// CHECK: def AppendList
// CHECK: list<int> items = [1, 2, 3, 4];
def AppendList : Base {
  let append items = [3, 4];
}

// CHECK: def AppendString
// CHECK: string text = "hello world";
def AppendString : Base {
  let append text = " world";
}

// Test append on initially-unset field.
class WithUnset {
  list<int> vals = ?;
  string msg = ?;
}

// CHECK: def AppendUnset
// CHECK: list<int> vals = [1];
// CHECK: string msg = "hi";
def AppendUnset : WithUnset {
  let append vals = [1];
  let append msg = "hi";
}

// Test multi-level inheritance accumulation.
class Middle : Base {
  let append items = [3];
  let append text = " world";
}

// CHECK: def MultiLevel
// CHECK: list<int> items = [1, 2, 3, 4];
// CHECK: string text = "hello world!";
def MultiLevel : Middle {
  let append items = [4];
  let append text = "!";
}

// CHECK: def OverrideAfterAppend
// CHECK: list<int> items = [10];
def OverrideAfterAppend : Base {
  let append items = [3];
  let items = [10];
}

// CHECK: def PrependDag
// CHECK: dag d = (op 0:$z);
def PrependDag : Base {
  let prepend d = (op 0:$z);
}

// CHECK: def PrependList
// CHECK: list<int> items = [0, 1, 2];
def PrependList : Base {
  let prepend items = [0];
}

// CHECK: def PrependString
// CHECK: string text = "say hello";
def PrependString : Base {
  let prepend text = "say ";
}

// Test top-level let with append.
let append items = [100] in {
  // CHECK: def TopLevelAppend
  // CHECK: list<int> items = [1, 2, 100];
  def TopLevelAppend : Base;
}

// Test top-level let with prepend.
let prepend items = [0] in {
  // CHECK: def TopLevelPrepend
  // CHECK: list<int> items = [0, 1, 2];
  def TopLevelPrepend : Base;
}
