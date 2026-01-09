#ifndef HEADER_GUARD_ALGORITHMS_HPP
#define HEADER_GUARD_ALGORITHMS_HPP

#include <algorithm>
#include <numeric>
#include <type_traits>

namespace dpsg {

template <template<class, class...> class VectorLike,
         class T, class U, class F, std::enable_if_t<std::is_invocable_v<F, U>, int> = 0, class... Args>
VectorLike<T, Args...> map(VectorLike<U, Args...> &v, T (*f)(U)) {
  VectorLike<T, Args...> result;
  result.reserve(v.size());
  std::transform(v.begin(), v.end(), std::back_inserter(result), f);
  return result;
}

template<template<class, class...> class VectorLike, class T, class P, std::enable_if_t<std::is_invocable_v<P, T>, int> = 0, class... Args>
constexpr bool all(const VectorLike<T, Args...>& v, P p) {
  using std::begin; using std::end;
  return std::all_of(begin(v), end(v), p);
}

template<template<class, class...> class VectorLike, class T, class ...Args>
constexpr T sum(const VectorLike<T, Args...>& v) {
  using std::begin; using std::end;
  return std::accumulate(begin(v), end(v), T{});
}

template<template<class, class...> class VectorLike, class T, class U, class Op, class ...Args>
constexpr T foldl(const VectorLike<T, Args...>& v, U init, Op f) {
  using std::begin; using std::end;
  return std::accumulate(begin(v), end(v), init, f);
}

} // namespace dpsg

#endif // HEADER_GUARD_ALGORITHMS_HPP
