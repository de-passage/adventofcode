#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <ranges>

bool is_valid(const std::vector<int> &numbers) {
  assert(numbers.size() >= 2);
  auto first = numbers[0];
  auto second = numbers[1];

  int dist = std::abs(second - first);

  if (dist == 0 || dist > 3) {
    return false;
  }
  bool descending = second < first;

  size_t last = second;

  for (auto i : numbers | std::ranges::views::drop(2)) {
    int dist = std::abs((int)i - (int)last);
    if (dist == 0 || dist > 3) {
      return false;
    }
    if (i < last != descending) {
      return false;
    }
    last = i;
  }
  return true;
}

DPSG_AOC_MAIN(file) {
  int sum = 0;

  for (auto line : dpsg::lines(file)) {
    std::vector<int> numbers;
    for (auto i : dpsg::numbers(line)) {
      numbers.push_back(i);
    }
    if (is_valid(numbers)) {
      sum += 1;
    } else {
      for (size_t i = 0; i < numbers.size(); ++i) {
        std::vector copy = numbers;
        copy.erase(copy.begin() + i);
        if (is_valid(copy)) {
          sum += 1;
          break;
        }
      }
    }
  }
  dpsg::println("{}", sum);
}
