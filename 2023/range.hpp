#ifndef HEADER_GUARD_RANGE_HPP
#define HEADER_GUARD_RANGE_HPP

#include <cstddef>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <optional>
//#include <iostream>

template<class T, std::enable_if_t<std::is_integral_v<T>, int> =0 >
struct basic_range {
  T begin;
  T end;

  inline constexpr std::make_signed_t<T> size() const { return end - begin; }
  inline constexpr bool contains(size_t pos) const {
    return pos >= begin && pos < end;
  }
  template<class U>
  inline constexpr bool contains(basic_range<U> r) const {
    return r.begin >= begin && r.end <= end;
  }
};

using range = basic_range<size_t>;

template<template<class, class...>class VectorLike, class T, class ...R>
void range_merge_insert(VectorLike<basic_range<T>> &v, basic_range<T> n) {
  using namespace std;
  auto it = std::lower_bound(v.begin(), v.end(), n,
                             [](const range &left, const range &right) {
                               return left.begin < right.begin;
                             });
  if (n.begin >= n.end) return ; // empty range

  if (it > v.begin() && (it - 1)->end > n.begin) {
    auto prev = it - 1;
      //std::cout << "merging (overlap) " << *prev << " with " << n << std::endl;
      prev->end = std::max(prev->end, n.end);
      it = prev;
      it->end = std::max(it->end, n.end);
  } else {
    //std::cout << "no overlap, inserting " << n << std::endl;
    it = v.insert(it, n);
  }


  auto next = it + 1;
  while (next != v.end()) {
    //std::cout << "trying to merge " << *it << " with " << *next << std::endl;
    if (next->begin >= it->end) {
      //std::cout << "no overlap" << std::endl;
      // disjointed intervals, we may stop here
      break;
    }

    // next->begin overlaps the new range
    it->end = std::max(it->end, next->end);
    //std::cout << "merged " << *it << endl;

    v.erase(next);
    next = it + 1;
  }
}

template<class T>
inline constexpr std::optional<basic_range<T>> combine(basic_range<T> left, basic_range<T> right) noexcept {
  if (right.begin >= left.begin && right.begin <= left.end) {
    return {{left.begin, std::max(left.end, right.end)}};
  } else if (right.end >= left.begin && right.end <= left.end) {
    return {{std::min(left.begin, right.begin), left.end}};
  }
  return {};
}

template<class T>
inline constexpr bool operator==(const basic_range<T> &lhs, const basic_range<T> &rhs) noexcept {
  return lhs.begin == rhs.begin && lhs.end == rhs.end;
}

template <class T> struct std::hash<basic_range<T>> {
  size_t operator()(const basic_range<T> &r) const {
    return std::hash<T>()(r.begin) ^ (std::hash<T>()(r.end) << 1);
  }
};

template<class T>
std::ostream &operator<<(std::ostream &os, const basic_range<T> &r) {
  return os << '[' << r.begin << ".." << r.end << ')';
}
#endif // HEADER_GUARD_RANGE_HPP
