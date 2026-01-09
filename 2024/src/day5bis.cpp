#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <unordered_map>

bool ordered(const std::vector<int> &ns,
             std::unordered_multimap<int, int> &ordermap) {
  for (int i = 0; i < ns.size() - 1; ++i) {
    auto current = ns[i];
    auto next = ns[i + 1];
    auto p = ordermap.equal_range(current);
    if (p.first == p.second) {
      return false;
    }

    auto &[left, right] = *p.first;

    bool found = false;
    for (auto it = p.first; it != p.second; ++it) {
      if (it->second == next) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

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

  std::vector<std::vector<int>> invalids;
  for (auto &u : updates) {
    if (!ordered(u, ordermap)) {
      invalids.emplace_back(std::move(u));
    }
  }

  for (auto &inv : invalids) {
    while (!ordered(inv, ordermap)) {
      for (int i = 0; i < inv.size() - 1; ++i) {
        int current = inv[i];
        int next = inv[i + 1];

        auto r = ordermap.equal_range(next);
        if (r.first != r.second) {
          for (auto it = r.first; it != r.second; ++it) {
            if (it->second == current) {
              std::swap(inv[i], inv[i + 1]);
              goto next_iter;
            }
          }
        }
      next_iter:
      }
    }
  }

  int sum = 0;
  for (auto& inv: invalids) {
    std::print("inv: ");
    for (auto i : inv) {
      std::print("{}, ", i);
    }
    std::println();
    sum += inv[inv.size() / 2];
  }
  std::println("sum: {}", sum);
}
