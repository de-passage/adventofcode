#ifndef HEADER_GUARD_ALGORITHMS_HPP
#define HEADER_GUARD_ALGORITHMS_HPP

#include <algorithm>
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
constexpr bool all(const VectorLike<T>& v, P p) {
  using std::begin; using std::end;
  return std::all_of(begin(v), end(v), p);
}

} // namespace dpsg

#endif // HEADER_GUARD_ALGORITHMS_HPP
