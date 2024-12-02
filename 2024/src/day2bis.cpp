#include "utils.hpp"
#include <cassert>
#include <print>

DPSG_AOC_MAIN(file) {
  int sum = 0;

  for (auto line : dpsg::lines(file)) {
    auto first = dpsg::next_number(line);
    if (!first.has_value()) {
      continue;
    }
    auto second = dpsg::next_number(line, first->position.end);
    assert(second.has_value() && "Expected a number");

    int dist = std::abs((int)second->value - (int)first->value);

    if (dist == 0 || dist > 3) {
      continue;
    }
    bool descending = second->value < first->value;

    auto remaining = std::string_view{line}.substr(second->position.end);
    size_t last = second->value;

    for (auto i: dpsg::numbers(remaining)) {
      int dist = std::abs((int)i - (int)last);
      if (dist == 0 || dist > 3) {
        goto next_line;
      }
      if (i < last != descending) {
        goto next_line;
      }
      last = i;
    }
    sum += 1;

next_line:;
  }
  std::println("{}", sum);
}
