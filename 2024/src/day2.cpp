#include "utils.hpp"
#include <cassert>
#include <print>

DPSG_AOC_MAIN(file) {
  int sum = 0;

  for (auto line : dpsg::lines(file)) {
    dpsg::logln("line: ", line);
    auto first = dpsg::next_number(line);
    if (!first.has_value()) {
      continue;
    }
    dpsg::log(first->value, ' ');
    auto second = dpsg::next_number(line, first->position.end);
    assert(second.has_value() && "Expected a number");
    dpsg::log(second->value, ' ');

    int dist = std::abs((int)second->value - (int)first->value);

    if (dist == 0 || dist > 3) {
      dpsg::log(" - dist not right\n");
      continue;
    }
    bool descending = second->value < first->value;

    auto remaining = std::string_view{line}.substr(second->position.end);
    size_t last = second->value;

    for (auto i: dpsg::numbers(remaining)) {
      dpsg::log(i, " ");
      int dist = std::abs((int)i - (int)last);
      if (dist == 0 || dist > 3) {
        dpsg::logln(" - dist not right");
        goto next_line;
      }
      if (i < last != descending) {
        dpsg::logln(" - direction not right");
        goto next_line;
      }
      last = i;
    }
    dpsg::logln(" - valid");
    sum += 1;

next_line:;
  }
  std::println("{}", sum);
}
