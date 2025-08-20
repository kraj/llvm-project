// RUN: %clang_cc1 -triple arm64               -std=c++26 -Wno-deprecated-builtins \
// RUN:                                        -fsyntax-only -verify %s 
// RUN: %clang_cc1 -triple arm64-apple-darwin -fptrauth-calls -fptrauth-intrinsics \
// RUN:                                       -fptrauth-vtable-pointer-address-discrimination \
// RUN:                                       -std=c++26 -Wno-deprecated-builtins \
// RUN:                                       -fsyntax-only -verify %s

// expected-no-diagnostics

#ifdef __PTRAUTH__

#define NonAddressDiscriminatedVTablePtrAttr \
  [[clang::ptrauth_vtable_pointer(process_independent, no_address_discrimination, no_extra_discrimination)]]
#define AddressDiscriminatedVTablePtrAttr \
  [[clang::ptrauth_vtable_pointer(process_independent, address_discrimination, no_extra_discrimination)]]
#define ADDR_DISC_ENABLED true
#else
#define NonAddressDiscriminatedVTablePtrAttr
#define AddressDiscriminatedVTablePtrAttr
#define ADDR_DISC_ENABLED false
#define __ptrauth(...)
#endif


typedef int* __ptrauth(1,1,1) AddressDiscriminatedPtr;
typedef __UINT64_TYPE__ __ptrauth(1,1,1) AddressDiscriminatedInt64;
struct AddressDiscriminatedFields {
  AddressDiscriminatedPtr ptr;
};
struct RelocatableAddressDiscriminatedFields trivially_relocatable_if_eligible {
  AddressDiscriminatedPtr ptr;
};
struct AddressDiscriminatedFieldInBaseClass : AddressDiscriminatedFields {
  void *newfield;
};

struct NonAddressDiscriminatedVTablePtrAttr NonAddressDiscriminatedVTablePtr {
  virtual ~NonAddressDiscriminatedVTablePtr();
  void *i;
};

struct NonAddressDiscriminatedVTablePtrAttr NonAddressDiscriminatedVTablePtr2 {
  virtual ~NonAddressDiscriminatedVTablePtr2();
  void *j;
};

struct NonAddressDiscriminatedVTablePtrAttr RelocatableNonAddressDiscriminatedVTablePtr trivially_relocatable_if_eligible {
  virtual ~RelocatableNonAddressDiscriminatedVTablePtr();
  void *i;
};

struct NonAddressDiscriminatedVTablePtrAttr RelocatableNonAddressDiscriminatedVTablePtr2 trivially_relocatable_if_eligible {
  virtual ~RelocatableNonAddressDiscriminatedVTablePtr2();
  void *j;
};

struct AddressDiscriminatedVTablePtrAttr AddressDiscriminatedVTablePtr {
  virtual ~AddressDiscriminatedVTablePtr();
  void *k;
};

struct AddressDiscriminatedVTablePtrAttr RelocatableAddressDiscriminatedVTablePtr trivially_relocatable_if_eligible {
  virtual ~RelocatableAddressDiscriminatedVTablePtr();
  void *k;
};

struct NoAddressDiscriminatedBaseClasses : NonAddressDiscriminatedVTablePtr,
                                           NonAddressDiscriminatedVTablePtr2 {
  void *l;
};

struct RelocatableNoAddressDiscriminatedBaseClasses trivially_relocatable_if_eligible :
                                           NonAddressDiscriminatedVTablePtr,
                                           NonAddressDiscriminatedVTablePtr2 {
  void *l;
};

struct AddressDiscriminatedPrimaryBase : AddressDiscriminatedVTablePtr,
                                         NonAddressDiscriminatedVTablePtr {
  void *l;
};
struct AddressDiscriminatedSecondaryBase : NonAddressDiscriminatedVTablePtr,
                                           AddressDiscriminatedVTablePtr {
  void *l;
};

struct RelocatableAddressDiscriminatedPrimaryBase : RelocatableAddressDiscriminatedVTablePtr,
                                         RelocatableNonAddressDiscriminatedVTablePtr {
  void *l;
};
struct RelocatableAddressDiscriminatedSecondaryBase : RelocatableNonAddressDiscriminatedVTablePtr,
                                           RelocatableAddressDiscriminatedVTablePtr {
  void *l;
};
struct EmbdeddedAddressDiscriminatedPolymorphicClass {
  AddressDiscriminatedVTablePtr field;
};
struct RelocatableEmbdeddedAddressDiscriminatedPolymorphicClass trivially_relocatable_if_eligible {
  AddressDiscriminatedVTablePtr field;
};

#define ASSERT_BUILTIN_EQUALS(Expression, Predicate, Info) \
  static_assert((Expression) == (([](bool Polymorphic, bool AddrDisc, bool Relocatable, bool HasNonstandardLayout){ return (Predicate); })Info), #Expression);
  //, #Expression " did not match " #Predicate);


#define TEST_BUILTINS(Builtin, Predicate) \
  ASSERT_BUILTIN_EQUALS(Builtin(AddressDiscriminatedPtr), Predicate, (false, ADDR_DISC_ENABLED, true, false)) \
  ASSERT_BUILTIN_EQUALS(Builtin(AddressDiscriminatedInt64), Predicate, (false, ADDR_DISC_ENABLED, true, false))\
  ASSERT_BUILTIN_EQUALS(Builtin(AddressDiscriminatedFields), Predicate, (false, ADDR_DISC_ENABLED, true, false))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableAddressDiscriminatedFields), Predicate, (false, ADDR_DISC_ENABLED, true, false))\
  ASSERT_BUILTIN_EQUALS(Builtin(AddressDiscriminatedFieldInBaseClass), Predicate, (false, ADDR_DISC_ENABLED, true, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(NonAddressDiscriminatedVTablePtr), Predicate, (true, false, false, false))\
  ASSERT_BUILTIN_EQUALS(Builtin(NonAddressDiscriminatedVTablePtr2), Predicate, (true, false, false, false))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableNonAddressDiscriminatedVTablePtr), Predicate, (true, false, true, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableNonAddressDiscriminatedVTablePtr2), Predicate, (true, false, true, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(AddressDiscriminatedVTablePtr), Predicate, (true, ADDR_DISC_ENABLED, false, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableAddressDiscriminatedVTablePtr), Predicate, (true, ADDR_DISC_ENABLED, true, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(NoAddressDiscriminatedBaseClasses), Predicate, (true, false, false, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableNoAddressDiscriminatedBaseClasses), Predicate, (true, false, false, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(AddressDiscriminatedPrimaryBase), Predicate, (true, ADDR_DISC_ENABLED, false, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(AddressDiscriminatedSecondaryBase), Predicate, (true, ADDR_DISC_ENABLED, false, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableAddressDiscriminatedPrimaryBase), Predicate, (true, ADDR_DISC_ENABLED, true, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableAddressDiscriminatedSecondaryBase), Predicate, (true, ADDR_DISC_ENABLED, true, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(EmbdeddedAddressDiscriminatedPolymorphicClass), Predicate, (true, ADDR_DISC_ENABLED, false, true))\
  ASSERT_BUILTIN_EQUALS(Builtin(RelocatableEmbdeddedAddressDiscriminatedPolymorphicClass), Predicate, (true, ADDR_DISC_ENABLED, false, true))

TEST_BUILTINS(__is_pod, !(Polymorphic || AddrDisc || HasNonstandardLayout))
TEST_BUILTINS(__is_standard_layout, !(Polymorphic || HasNonstandardLayout))
TEST_BUILTINS(__has_trivial_move_constructor, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__has_trivial_copy, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__has_trivial_assign, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__has_trivial_move_assign, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__is_trivial, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__is_trivially_copyable, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__is_trivially_copyable, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__is_trivially_relocatable, !((Polymorphic) || AddrDisc))
TEST_BUILTINS(__builtin_is_cpp_trivially_relocatable, !((Polymorphic && !Relocatable) || AddrDisc))
TEST_BUILTINS(__builtin_is_replaceable, !(Polymorphic || AddrDisc))
TEST_BUILTINS(__is_bitwise_cloneable, !AddrDisc);

#define ASSIGNABLE_WRAPPER(Type) __is_trivially_assignable(Type&, Type)
TEST_BUILTINS(ASSIGNABLE_WRAPPER, !(Polymorphic || AddrDisc))
