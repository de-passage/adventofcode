#ifndef HEADER_GUARD_ALGORITHMS_HPP
#define HEADER_GUARD_ALGORITHMS_HPP

#include <algorithm>
#include <type_traits>

namespace dpsg {

template <class T, class U, class F, std::enable_if_t<std::is_invocable_v<F, U>, int> = 0>
std::vector<T> map(const std::vector<U> &v, T (*f)(U)) {
  std::vector<T> result;
  result.reserve(v.size());
  std::transform(v.begin(), v.end(), std::back_inserter(result), f);
  return result;
}

template<class T, class P, std::enable_if_t<std::is_invocable_v<P, T>, int> = 0>
constexpr bool all(const std::vector<T>& v, P p) {
  using std::begin; using std::end;
  return std::all_of(begin(v), end(v), p);
}

} // namespace dpsg

#endif // HEADER_GUARD_ALGORITHMS_HPP
