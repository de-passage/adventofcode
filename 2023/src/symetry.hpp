#ifndef HEADER_GUARD_SYMETRY_HPP
#define HEADER_GUARD_SYMETRY_HPP

#include "utils.hpp"
#include <bitset>
#include <popcntintrin.h>
#include <unordered_set>
#include <vector>

namespace dpsg {

template <typename T> size_t lowest_symetric_index(const std::vector<T> &v) {
  if (v.size() <= 1) {
    return 0;
  }

  // Look for the first equality, then check if the rest is symetric up to a
  // boundary
  for (auto i = 1u; i < v.size(); ++i) {
    if (v[i] == v[i - 1]) {
      logln("potential symetry at ", i);
      bool symetric = true;
      for (auto j = 1u; (j < i) && (i + j < v.size()); ++j) {

        if (v[i - j - 1] != v[i + j]) {
          logln("not symetric at ", i, " with ", j, " (=", i + j, "");
          symetric = false;
          break;
        }
      }
      if (symetric) {
        return i;
      }
    }
  }
  return 0;
}

}

template<class T>
  struct std::hash<std::pair<T,T>> {
    size_t operator()(const std::pair<T,T>& p) const {
      return std::hash<T>()(p.first) ^ std::hash<T>()(p.second);
    }
  };

namespace dpsg {
template <typename T>
size_t lowest_index_with_smudge(const std::vector<T> &v, size_t reflpoint) {
  if (v.size() <= 1) {
    return 0;
  }

  // Look for the first equality, then check if the rest is symetric up to a
  // boundary
  for (auto i = 1u; i < v.size(); ++i) {
    if (i == reflpoint) {
      continue;
    }
    auto pc = __builtin_popcount(v[i] ^ v[i - 1]);
    if (v[i] == v[i - 1] || pc == 1) {
      logln("potential symetry at ", i);
      bool symetric = true;
      bool smudged = pc == 1;

      for (auto j = 1u; (j < i) && (i + j < v.size()); ++j) {
        if (v[i - j - 1] != v[i + j]) {
          if (!smudged && __builtin_popcount(v[i - j - 1] ^ v[i + j]) == 1) {
            smudged = true;
          } else {
            logln("not symetric at ", i, " with ", j, " (=", i + j, "");
            symetric = false;
            break;
          }
        }
      }

      if (symetric && smudged) {
        return i;
      }
    }
  }
  return 0;

}

} // namespace dpsg

#endif /* HEADER_GUARD_SYMETRY_HPP */
