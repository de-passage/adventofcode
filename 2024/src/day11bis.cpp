#include "utils.hpp"
#include <chrono>
#include <cmath>
#include <print>
#include <unordered_map>

int digit_count(long t) {
  int count = 0;
  while (t > 0) {
    count++;
    t /= 10;
  }
  return count;
}

DPSG_AOC_MAIN(file) {
  std::string input;
  std::getline(file, input);

  std::unordered_map<size_t, size_t> numbers;
  for (auto i : dpsg::numbers(input)) {
    numbers[i]++;
  }

  for (int i = 0; i < 75; ++i) {
    std::unordered_map<size_t, size_t> next_numbers;

    for (auto [k, n] : numbers) {
      if (k == 0) {
        next_numbers[1] += n;
      } else {
        int c = digit_count(k);
        if (c % 2 == 0) {
          int off = std::pow(10, c / 2);
          next_numbers[k / off] += n;
          next_numbers[k % off] += n;
        } else {
          next_numbers[k * 2024] += n;
        }
      }
    }
    numbers = std::move(next_numbers);
  }

  size_t sum = 0;
  for (auto [k, n] : numbers) {
    sum += n;
  }
  std::println("Result: {}", sum);
}
