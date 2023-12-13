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
std::unordered_set<std::pair<T, T>> potential_smudges(const std::vector<T> &v) {
  std::unordered_set<std::pair<T, T>> smudges;
  if (v.size() <= 1) {
    return smudges;
  }


  // Look for the first equality, then check if the rest is symetric up to a
  // boundary
  for (auto i = 1u; i < v.size(); ++i) {
    // smudge is not on the symetry point, we start from a symetry point and
    // check if the rest is symetric with exactly one smudge
    if (v[i] == v[i - 1]) {

      for (auto j = 1u; (j < i) && (i + j < v.size()); ++j) {
        if (v[i - j - 1] != v[i + j]) {
          if (__builtin_popcountll(v[i - j - 1] & v[i + j]) == 1) {
            smudges.emplace(i - j - 1, v[i - j - 1] & v[i + j]);
            smudges.emplace(i + j, v[i - j - 1] & v[i + j]);
          }
          break;
        }
      }
      // Checking a potential smudge for symetry from this point
    } else if (__builtin_popcountll(v[i] & v[i - 1]) == 1) {
      bool symetric = true;
      for (auto j = 1u; (j < i) && (i + j < v.size()); ++j) {
        if (v[i - j - 1] != v[i + j]) {
          symetric = false;
          break;
        }
      }
      if (symetric) {
        smudges.emplace(i, v[i] & v[i - 1]);
        smudges.emplace(i - 1, v[i] & v[i - 1]);
      }
    }
  }
  return smudges;
}

} // namespace dpsg

#endif /* HEADER_GUARD_SYMETRY_HPP */
