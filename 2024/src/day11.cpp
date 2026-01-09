#include "utils.hpp"
#include <cmath>
#include "print.hpp"

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

  std::vector<size_t> numbers;
  for (auto i : dpsg::numbers(input)) {
    numbers.push_back(i);
  }

  for (int i = 0; i < 25; ++i) {
    std::vector<size_t> next_numbers;
    next_numbers.reserve(numbers.size() * 2);

    for (auto n : numbers) {
      if (n == 0) {
        next_numbers.push_back(1);
      } else {
        int c = digit_count(n);
        if (c % 2 == 0) {
          int off = std::pow(10, c / 2);
          next_numbers.push_back(n / off);
          next_numbers.push_back(n % off);
        } else {
          next_numbers.push_back(n * 2024);
        }
      }
    }
    numbers = std::move(next_numbers);
  }

  std::println("Result: {}", numbers.size());
}
