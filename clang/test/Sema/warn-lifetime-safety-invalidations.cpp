// RUN: %clang_cc1 -fsyntax-only -Wlifetime-safety-invalidation -Wno-dangling -verify %s

#include "Inputs/lifetime-analysis.h"

namespace logging {
class VoidifyStream {
 public:
  VoidifyStream() = default;
  template <typename T>
  void operator&(const T&) {}
};
class CheckError {
 public:
  static CheckError Check(const char* file, int line, const char* condition);
};
}  // namespace logging
#define LAZY_CHECK_STREAM(stream, condition) \
  !(condition) ? (void)0 : ::logging::VoidifyStream() & (stream)
#define CHECK(condition)                                                     \
  LAZY_CHECK_STREAM(                                                         \
      ::logging::CheckError::Check(__FILE__, __LINE__, #condition).stream(), \
      !(condition))

bool Bool();
void PassBool(bool);

// --- Test Cases ---

namespace SimpleResize {
void IteratorInvalidAfterResize(int new_size) {
  std::vector<int> v;
  auto it = std::begin(v);  // expected-warning {{object whose reference is captured is later invalidated}}
  v.resize(new_size);       // expected-note {{invalidated here}}
  *it;                      // expected-note {{later used here}}
}

void IteratorValidAfterResize(int new_size) {
  std::vector<int> v;
  auto it = std::begin(v);
  v.resize(new_size);
  it = std::begin(v);
  if (it != std::end(v)) {
    *it;  // ok
  }
}
}  // namespace SimpleResize

namespace CheckModel {
void IteratorValidAfterCheck() {
  std::vector<int> v;
  auto it = v.begin();
  *it;  // ok
}
}  // namespace CheckModel

namespace PointerToContainer {
std::vector<int>* GetContainerPointer();
void PointerToContainerTest() {
  // FIXME: Use opaque loans.
  std::vector<int>* v = GetContainerPointer();
  auto it = v->begin();
  *it = 0;  // not-ok
}
void PointerToContainerTest(std::vector<int>* v) {
  // FIXME: Handle placeholder loans.
  auto it = v->begin();
  *it = 0;  // not-ok
}
}  // namespace PointerToContainer

namespace InvalidateBeforeSwap {
void InvalidateBeforeSwapIterators(std::vector<int> v1, std::vector<int> v2) {
  auto it1 = std::begin(v1); // expected-warning {{object whose reference is captured is later invalidated}}
  auto it2 = std::begin(v2);
  if (it1 == std::end(v1) || it2 == std::end(v2)) return;
  *it1 = 0;     // ok
  *it2 = 0;     // ok
  v1.clear();   // expected-note {{invalidated here}}
  *it1 = 0;     // expected-note {{later used here}}
  // FIXME: Handle invalidating functions like std::swap.
  std::swap(it1, it2);
  *it1 = 0;  // ok
  *it2 = 0;  // not-ok
}

void InvalidateBeforeSwapContainers(std::vector<int> v1, std::vector<int> v2) {
  auto it1 = std::begin(v1);  // expected-warning {{object whose reference is captured is later invalidated}}
  auto it2 = std::begin(v2);
  if (it1 == std::end(v1) || it2 == std::end(v2)) return;
  *it1 = 0;     // ok
  *it2 = 0;     // ok
  v1.clear();   // expected-note {{invalidated here}}
  *it1 = 0;     // expected-note {{later used here}}
}
}  // namespace InvalidateBeforeSwap

namespace MergeConditionBasic {
bool A();
bool B();
void SameConditionInvalidatesThenValidatesIterator() {
  std::vector<int> container;
  auto it = container.begin(); // expected-warning {{object whose reference is captured is later invalidated}}
  if (it == container.end()) return;
  const bool a = A();
  if (a) {
    container.clear();  // expected-note {{invalidated here}}
  }
  if (a) {
    it = container.begin();
    if (it == std::end(container)) return;
  }
  *it = 10;  // expected-note {{later used here}}
}
}  // namespace MergeConditionBasic

namespace IteratorWithMultipleContainers {
void MergeWithDifferentContainerValuesIteratorNotInvalidated() {
  std::vector<int> v1, v2, v3;
  auto it = std::find(v1.begin(), v1.end(), 10);
  if (Bool()) {
    it = std::find(v2.begin(), v2.end(), 10);
  } else {
    it = std::find(v3.begin(), v3.end(), 10);
  }
  v1.clear();
  *it = 20;
}

void MergeWithDifferentContainerValuesInvalidated() {
  std::vector<int> v1, v2, v3;
  auto it = std::find(v1.begin(), v1.end(), 10);
  if (Bool()) {
    it = std::find(v2.begin(), v2.end(), 10);  // expected-warning {{object whose reference is captured is later invalidated}}
  } else {
    it = std::find(v3.begin(), v3.end(), 10);
  }
  v2.clear();   // expected-note {{invalidated here}}
  *it = 20;     // expected-note {{later used here}}
}
}  // namespace IteratorWithMultipleContainers

namespace InvalidationInLoops {
void IteratorInvalidationInAForLoop(std::vector<int> v) {
  for (auto it = std::begin(v);  // expected-warning {{object whose reference is captured is later invalidated}}
       it != std::end(v);
       ++it) {  // expected-note {{later used here}}
    if (Bool()) {
      v.erase(it);  // expected-note {{invalidated here}}
    }
  }
}

void IteratorInvalidationInAWhileLoop(std::vector<int> v) {
  auto it = std::begin(v);  // expected-warning {{object whose reference is captured is later invalidated}}
  while (it != std::end(v)) {
    if (Bool()) {
      v.erase(it);  // expected-note {{invalidated here}}
    }
    ++it; // expected-note {{later used here}}
  }
}

void IteratorInvalidationInAForeachLoop(std::vector<int> v) {
  for (int& x : v) { // expected-warning {{object whose reference is captured is later invalidated}} \
                     // expected-note {{later used here}}
    if (x % 2 == 0) {
      v.erase(std::find(v.begin(), v.end(), 1)); // expected-note {{invalidated here}}
    }
  }
}
}  // namespace InvalidationInLoops

namespace StdVectorPopBack {
void StdVectorPopBackInvalid(std::vector<int> v) {
  auto it = v.begin();  // expected-warning {{object whose reference is captured is later invalidated}}
  if (it == v.end()) return;
  *it;  // ok
  v.pop_back(); // expected-note {{invalidated here}}
  *it;          // expected-note {{later used here}}
}
}  // namespace StdVectorPopBack


namespace SimpleStdFind {
void IteratorCheckedAfterFind(std::vector<int> v) {
  auto it = std::find(std::begin(v), std::end(v), 3);
  if (it != std::end(v)) {
    *it;  // ok
  }
}

void IteratorCheckedAfterFindThenErased(std::vector<int> v) {
  auto it = std::find(std::begin(v), std::end(v), 3); // expected-warning {{object whose reference is captured is later invalidated}}
  if (it != std::end(v)) {
    v.erase(it); // expected-note {{invalidated here}}
  }
  *it;  // expected-note {{later used here}}
}
}  // namespace SimpleStdFind

namespace SimpleInsert {
void UseReturnedIteratorAfterInsert(std::vector<int> v) {
  auto it = std::begin(v);
  it = v.insert(it, 10);
  if (it != std::end(v)) {
    *it;  // ok
  }
}

void UseInvalidIteratorAfterInsert(std::vector<int> v) {
  auto it = std::begin(v);  // expected-warning {{object whose reference is captured is later invalidated}}
  v.insert(it, 10);         // expected-note {{invalidated here}}
  if (it != std::end(v)) {  // not-ok // expected-note {{later used here}}
    *it;
  }
}
}  // namespace SimpleInsert

namespace SimpleStdInsert {
void IteratorValidAfterInsert(std::vector<int> v) {
  auto it = std::begin(v);
  v.insert(it, 0);
  it = std::begin(v);
  if (it != std::end(v)) {
    *it;  // ok
  }
}

void IteratorInvalidAfterInsert(std::vector<int> v, int value) {
  auto it = std::begin(v);  // expected-warning {{object whose reference is captured is later invalidated}}
  v.insert(it, 0);          // expected-note {{invalidated here}}
  *it;                      // expected-note {{later used here}}
}
}  // namespace SimpleStdInsert

namespace SimpleInvalidIterators {
void IteratorUsedAfterErase(std::vector<int> v) {
  auto it = std::begin(v);          // expected-warning {{object whose reference is captured is later invalidated}}
  for (; it != std::end(v); ++it) { // expected-note {{later used here}}
    if (*it > 3) {
      v.erase(it);                  // expected-note {{invalidated here}}
    }
  }
}

void IteratorUsedAfterPushBack(std::vector<int> v) {
  auto it = std::begin(v); // expected-warning {{object whose reference is captured is later invalidated}}
  if (it != std::end(v) && *it == 3) {
    v.push_back(4); // expected-note {{invalidated here}}
  }
  ++it;             // expected-note {{later used here}}
}
}  // namespace SimpleInvalidIterators

namespace ElementReferences {
// Testing raw pointers and references to elements, not just iterators.

void ReferenceToVectorElement() {
  std::vector<int> v = {1, 2, 3};
  int& ref = v[0];
  v.push_back(4);
  // FIXME: Detect this as a use of 'ref. (<file bug>)
  ref = 10;
  (void)ref;
}

void PointerToVectorElement() {
  std::vector<int> v = {1, 2, 3};
  int* ptr = &v[0];  // expected-warning {{object whose reference is captured is later invalidated}}
  v.resize(100);     // expected-note {{invalidated here}}
  *ptr = 10;         // expected-note {{later used here}}
}

void SelfInvalidatingMap() {
  std::unordered_map<int, int> mp;
  mp[1] = 1;
  mp[2] = mp[1];  // FIXME: Detect this. We are mising a UseFact for the assignment params.
}
} // namespace ElementReferences

namespace Strings {

void append(std::string str) {
  std::string_view view = str;  // expected-warning {{object whose reference is captured is later invalidated}}
  str += "456";                 // expected-note {{invalidated here}}
  (void)view;                   // expected-note {{later used here}}
}
void reassign(std::string str, std::string str2) {
  std::string_view view = str;  // expected-warning {{object whose reference is captured is later invalidated}}
  str = str2;                   // expected-note {{invalidated here}}
  (void)view;                   // expected-note {{later used here}}
}
} // namespace Strings

namespace ContainersAsFields {
struct S {
  std::vector<std::string> strings1;
  std::vector<std::string> strings2;
};
// FIXME: Make Paths more precise to reason at field granularity.
//        Here, we invalidate paths `s.strings2` and deeper but sibling paths
void Invalidate1Use2() {
    S s;
    auto it = s.strings1.begin();  // expected-warning {{object whose reference is captured is later invalidated}}
    s.strings2.push_back("1");     // expected-note {{invalidated here}}
    *it;                           // expected-note {{later used here}}
}
void Invalidate1UseS() {
    S s;
    S* p = &s;                 // expected-warning {{object whose reference is captured is later invalidated}}
    s.strings2.push_back("1"); // expected-note {{invalidated here}}
    (void)*p;                  // expected-note {{later used here}}
}
void ContainerAsPointer() {
    std::vector<std::string> s;                
    std::vector<std::string>* p = &s; // expected-warning {{object whose reference is captured is later invalidated}}
    p->push_back("1");                // expected-note {{invalidated here}}
    (void)*p;                         // expected-note {{later used here}}
}
} // namespace ContainersAsFields
