// RUN: %clang_cc1 -std=c++26 -emit-llvm %s -o - -triple=x86_64-apple-darwin9

namespace std {
class partial_ordering {};
class strong_ordering {
public:
  operator partial_ordering();
};
using size_t = decltype(sizeof(int));
template <size_t> struct __priority_tag {};
namespace __partial_order {
struct __fn {
  template <class _Tp, class _Up>
  constexpr auto __go(_Tp __t, _Up __u, __priority_tag<2>)
      -> decltype(partial_ordering(partial_order(__t, __u))) {}
  template <class _Tp, class _Up> auto operator()(_Tp __t, _Up __u) {
    __go(__t, __u, __priority_tag<2>());
  }
};
}
auto partial_order = __partial_order::__fn{};
namespace {
struct A {};
strong_ordering partial_order(A, A) {
  A a;
  std::partial_order(a, a);
}
}
}
