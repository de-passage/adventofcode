#include "utils.hpp"
#include <cassert>
#include "print.hpp"

DPSG_AOC_MAIN(file) {
  std::vector<int> left_values;
  std::vector<int> right_values;

  std::string line;
  while (getline(file, line)) {
    auto left = next_number(line);
    assert(left.has_value());
    auto right = next_number(line, left->position.end);
    assert(right.has_value());

    left_values.push_back(left->value);
    right_values.push_back(right->value);
  }

  std::ranges::sort(left_values);
  std::ranges::sort(right_values);

  int last = -1;
  int last_product = -1;
  size_t sum_of_products = 0;
  for (auto i = 0u; i < left_values.size(); ++i) {
    auto left = left_values[i];

    if (left != last) {
      auto amount_right = std::ranges::equal_range(right_values, left);
      last_product = std::distance(amount_right.begin(), amount_right.end()) * left;
    }

    sum_of_products += last_product;
  }

  std::println("{}", sum_of_products);
}
