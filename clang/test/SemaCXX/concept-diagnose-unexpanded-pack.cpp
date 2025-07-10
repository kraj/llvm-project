// RUN: %clang_cc1 -fsyntax-only -std=c++20 -verify %s

template <typename T> void foo();
template <class... Ts>
concept ConceptA = requires { foo<Ts>(); };
// expected-error@-1 {{expression contains unexpanded parameter pack 'Ts'}}

template <class>
concept ConceptB = ConceptA<int>;

template <ConceptB Foo> void bar(Foo);

void test() { bar(1); }
