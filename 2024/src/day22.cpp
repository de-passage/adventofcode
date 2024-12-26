#include "utils.hpp"
#include <print>

constexpr size_t mix(size_t secret, size_t x) { return secret ^ x; }
static_assert(mix(42, 15) == 37);

constexpr size_t prune(size_t x) { return x % 16777216; }
static_assert(prune(100000000) == 16113920);

DPSG_AOC_MAIN(file) {
  size_t sum = 0;
  for (auto line : dpsg::lines(file)) {
    size_t n = next_number(line)->value;
    std::print("{} -> ", n);

    for (int i = 0; i < 2000; ++i) {
      n = prune(mix(n, n * 64));
      n = prune(mix(n, n / 32));
      n = prune(mix(n, n * 2048));
    }
    std::println("{}", n);
    sum += n;
  }

  std::println("{}", sum);
}
