#include "utils.hpp"
#include <cassert>
#include <print>

DPSG_AOC_MAIN(file) {
  std::vector<int> left_values;
  std::vector<int> right_values;

  std::string line;

  while (std::getline(file, line)) {
    auto left = next_number(line);
    assert(left.has_value());
    auto right = next_number(line, left->position.end);
    assert(right.has_value());

    left_values.push_back(left->value);
    right_values.push_back(right->value);
  }

  std::ranges::sort(left_values);
  std::ranges::sort(right_values);

  size_t sum_of_distances = 0;
  for (auto i = 0u; i < left_values.size(); ++i) {
    auto left = left_values[i];
    auto right = right_values[i];

    sum_of_distances += std::abs(right - left);
  }
  std::println("{}", sum_of_distances);
}
