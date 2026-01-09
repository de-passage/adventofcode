#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <ranges>
#include <unordered_map>
#include <unordered_set>

struct hash_me {
  std::size_t operator()(const std::vector<std::string> &t) const {
    size_t h = 0;
    for (auto &s : t) {
      h ^= std::hash<std::string>{}(s);
    }
    return h;
  }
};

DPSG_AOC_MAIN(file) {
  std::unordered_multimap<std::string, std::string> connections;
  for (auto line : dpsg::lines(file)) {
    auto first = next_word(line);
    assert(first);
    auto second = next_word(line, first->position.end);
    assert(second);

    connections.emplace(first->value, second->value);
    connections.emplace(second->value, first->value);
  }

  std::unordered_set<std::vector<std::string>, hash_me> networks;

  for (const auto &[k, v] : connections) {
    if (k[0] != 't') {
      continue;
    }
    for (auto [f, e] = connections.equal_range(v); f != e; ++f) {
      bool found = false;
      for (auto [f2, e2] = connections.equal_range(f->second); f2 != e2; ++f2) {
        if (f2->second == k) {
          found = true;
          break;
        }
      }

      if (!found) {
        continue;
      }

      std::vector s{k, v, f->second};
      std::ranges::sort(s);
      networks.insert(s);
    }
  }

  for (const auto &network : networks) {
    for (const auto &node : network) {
      dpsg::print("{} ->", node);
    }
    dpsg::println();
  }
  dpsg::println("Found {} networks", networks.size());
}
