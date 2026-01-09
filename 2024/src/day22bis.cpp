#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include "print.hpp"
#include <unordered_map>
#include <unordered_set>

constexpr size_t mix(size_t secret, size_t x) { return secret ^ x; }
static_assert(mix(42, 15) == 37);

constexpr size_t prune(size_t x) { return x % 16777216; }
static_assert(prune(100000000) == 16113920);

struct changes {
  int8_t last[4];

  constexpr int8_t operator[](size_t s) const {
    assert(s < 4);
    return last[s];
  }

  friend constexpr bool operator==(const changes &left, const changes &right) {
    for (size_t i = 0; i < 4; ++i) {
      if (left[i] != right[i]) {
        return false;
      }
    }
    return true;
  }

  void insert(int8_t n) {
    for (int i = 1; i < 4; ++i) {
      last[i - 1] = last[i];
    }
    last[3] = n;
  }
};

template <> struct std::hash<changes> {
  size_t operator()(const changes &c) const {
    return c[0] | (c[1] << 8) | (c[2] << 16) | (c[3] << 24);
  }
};

constexpr size_t next(size_t n) {
  n = prune(mix(n, n * 64));
  n = prune(mix(n, n / 32));
  n = prune(mix(n, n * 2048));
  return n;
}

DPSG_AOC_MAIN(file) {
  std::unordered_map<changes, size_t> values;
  size_t sum = 0;
  for (auto line : dpsg::lines(file)) {
    size_t n = next_number(line)->value;
    std::unordered_set<changes> already_seen;


    changes ch = {{0,0,0,0}};
    int8_t last = n % 10;

    for (int i = 0; i < 3; ++i) {
      n = next(n);
      int8_t l = n % 10;
      int8_t c = l - last;
      ch.insert(c);
      last = l;
    }

    for (int i = 3; i < 2000; ++i) {
      n = next(n);
      int8_t l = n % 10;
      int8_t c = l - last;
      ch.insert(c);

      if (!already_seen.contains(ch)) {
        values[ch] += l;
      }

      already_seen.insert(ch);
      last = l;
    }

    sum += n;
  }

  using V = decltype(values)::value_type;
  auto m = std::max_element(
      values.begin(), values.end(),
      [](const V &left, const V &right) { return left.second < right.second; });

  assert(m != values.end());

  dpsg::println("Found: {}", m->second);
  dpsg::print("Sequence: ");
  for (int i = 0; i < 4; ++i) {
    dpsg::print("{},", m->first.last[i]);
  }
  dpsg::println();
  dpsg::println("Sum: {}", sum);
}
