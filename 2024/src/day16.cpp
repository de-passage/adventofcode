#include "utils.hpp"
#include <cassert>
#include <print>
#include <queue>
#include <unordered_set>

struct vec2 {
  int x, y;
  friend constexpr bool operator==(const vec2 &a, const vec2 &b) {
    return a.x == b.x && a.y == b.y;
  }
  friend constexpr vec2 operator+(const vec2 &a, const vec2 &b) {
    return {a.x + b.x, a.y + b.y};
  }
};

constexpr vec2 rotate_left(const vec2 &v) { return {-v.y, v.x}; }
constexpr vec2 rotate_right(const vec2 &v) { return {v.y, -v.x}; }

size_t hash_size_t(size_t a, size_t b) {
  std::size_t left = std::hash<size_t>{}(a);
  std::size_t right = std::hash<size_t>{}(b);
  return left ^ (right + 0x9e3779b9 + (left << 6) + (left >> 2));
}

template <> struct std::hash<vec2> {
  std::size_t operator()(const vec2 &v) const { return hash_size_t(v.x, v.y); }
};

template<> struct std::hash<std::pair<vec2, vec2>> {
  std::size_t operator()(const std::pair<vec2, vec2>& p) const {
    return hash_size_t(hash_size_t(p.first.x, p.first.y), hash_size_t(p.second.x, p.second.y));
  }
};

struct vec2_with_weight {
  vec2 v;
  size_t weight{0};
  vec2 direction;

  friend constexpr bool operator==(const vec2_with_weight &a,
                                   const vec2_with_weight &b) {
    return a.v == b.v && a.weight == b.weight;
  }
};

struct by_weight {
  bool operator()(const vec2_with_weight &a, const vec2_with_weight &b) const {
    return a.weight > b.weight;
  }
};

DPSG_AOC_MAIN(file) {
  std::vector<std::vector<bool>> map;
  vec2 start, end;

  for (auto line : dpsg::lines(file)) {
    map.emplace_back();
    for (auto c : line) {
      if (c == 'S') {
        start = {static_cast<int>(map.back().size()),
                 static_cast<int>(map.size() - 1)};
      } else if (c == 'E') {
        end = {static_cast<int>(map.back().size()),
               static_cast<int>(map.size() - 1)};
      }
      map.back().push_back(c != '#');
    }
  }

  std::priority_queue<vec2_with_weight, std::vector<vec2_with_weight>,
                      by_weight>
      to_visit;
  std::unordered_set<std::pair<vec2, vec2>> visited;

  to_visit.push({start, 0, {1, 0}});

  while (!to_visit.empty()) {
    auto current = to_visit.top();
    to_visit.pop();


    if (current.v == end) {
      std::println("Found path with weight {}", current.weight);
      break;
    }

    if (visited.contains({current.v, current.direction})) {
      continue;
    }

    visited.insert({current.v, current.direction});

    for (auto [dir, weight] : {std::pair{rotate_left(current.direction), 1001},
                               {current.direction, 1},
                               {rotate_right(current.direction), 1001}}) {
      auto next = current.v + dir;
      if (next.x < 0 || next.y < 0 || next.y >= map.size() ||
          next.x >= map[next.y].size() || !map[next.y][next.x]) {
        continue;
      }

      to_visit.push({next, current.weight + weight, dir});
    }
  }
}
