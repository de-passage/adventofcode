#include "utils.hpp"
#include <cassert>
#include <print>
#include <unordered_map>


DPSG_AOC_MAIN(file) {
  std::unordered_multimap<int, int> ordermap;
  auto lines = dpsg::lines(file);
  for (auto line : lines) {
    if (line.empty()) {
      break;
    }
    auto left = next_number(line).value();
    auto right = next_number(line, left.position.end).value();
    ordermap.insert({left.value, right.value});
  }

  std::vector<std::vector<int>> updates;
  for (auto line : lines) {
    updates.push_back({});
    for (auto n : dpsg::numbers(line)) {
      updates.back().push_back(n);
    }
  }

  auto sum = 0;
  for (auto &u : updates) {

    for (int i = 0; i < u.size() - 1; ++i) {
      auto current = u[i];
      auto next = u[i + 1];
      auto p = ordermap.equal_range(current);
      if (p.first == p.second) {
        goto next_iter;
        continue;
      }

      auto &[left, right] = *p.first;

      bool found = false;
      for (auto it = p.first; it != p.second; ++it) {
        if (it->second == next) {
          found = true;
          break;
        }
      }
      if (!found)  {
        goto next_iter;
      }
    }
    sum += u[u.size() / 2];
    next_iter:;
  }
  std::println("Sum: {}", sum);
}
