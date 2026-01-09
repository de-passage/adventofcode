#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <unordered_set>

struct coord_t {
  int x, y;

  coord_t operator+(coord_t other) const { return {x + other.x, y + other.y}; }

  friend bool operator==(const coord_t &lhs, const coord_t &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
};

template<>
struct std::hash<coord_t> {
  std::size_t operator()(const coord_t &c) const {
    return std::hash<int>{}(c.x) ^ std::hash<int>{}(c.y);
  }
};

template<>
struct std::hash<std::pair<coord_t,coord_t>> {
  std::size_t operator()(const coord_t &c) const {
    return std::hash<int>{}(c.x) ^ std::hash<int>{}(c.y);
  }
};

coord_t rotate_right(coord_t dir) {
  // (0, -1) -> (1, 0) -> (0, 1) -> (-1, 0)
  if (dir.x == 0 && dir.y == -1) {
    return {1, 0};
  } else if (dir.x == 1 && dir.y == 0) {
    return {0, 1};
  } else if (dir.x == 0 && dir.y == 1) {
    return {-1, 0};
  } else if (dir.x == -1 && dir.y == 0) {
    return {0, -1};
  }
  throw std::runtime_error("Invalid direction");
}

using map_t = std::vector<std::vector<bool>>;

bool valid(coord_t c, const map_t &map) {
  return c.y >= 0 && c.y < map.size() && c.x >= 0 && c.x < map[c.y].size();
}

bool at(coord_t c, const map_t &map) { return map[c.y][c.x]; }

DPSG_AOC_MAIN(file) {
  map_t map;
  map.reserve(130);
  int x = 0, y = 0;
  coord_t guard;
  for (auto l : dpsg::lines(file)) {
    x = 0;
    map.push_back({});
    map.back().reserve(l.size());
    for (auto c : l) {
      map.back().push_back(c != '#');

      if (c == '^') {
        guard = {x, y};
      }

      x++;
    }
    y++;
  }

  int count = 0;
  coord_t dir = {0, -1};
  std::unordered_set<coord_t> visited;
  while (true) {
    visited.emplace(guard);

    auto next = guard + dir;
    if (!valid(next, map)) {
      break;
    }
    if (at(next, map) == false) {
      dir = rotate_right(dir);
      continue;
    }
    guard = next;
  }
  std::println("Count: {}", visited.size());
}
