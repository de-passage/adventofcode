#include "utils.hpp"
#include <cassert>
#include <chrono>
#include <print>
#include <unordered_set>

struct coord_t {
  int x, y;

  coord_t operator+(coord_t other) const { return {x + other.x, y + other.y}; }

  friend bool operator==(const coord_t &lhs, const coord_t &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
};

template <> struct std::hash<coord_t> {
  std::size_t operator()(const coord_t &c) const {
    return std::hash<int>{}(c.x) ^ std::hash<int>{}(c.y);
  }
};

template <> struct std::hash<std::pair<coord_t, coord_t>> {
  std::size_t operator()(const std::pair<coord_t, coord_t> &c) const {
    return std::hash<coord_t>{}(c.first) ^ std::hash<coord_t>{}(c.second);
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

using loop_t = std::unordered_set<std::pair<coord_t, coord_t>>;

bool is_loop(const map_t &map, coord_t guard) {
  loop_t loops;
  coord_t dir = {0, -1};
  while (true) {
    if (loops.contains({guard, dir})) {
      return true;
    }

    auto next = guard + dir;
    if (!valid(next, map)) {
      return false;
    }

    if (at(next, map) == false) {
      dir = rotate_right(dir);
      continue;
    } else {
      loops.emplace(guard, dir);
    }

    guard = next;
  }
}

std::unordered_set<coord_t> traverse(const map_t &map, coord_t guard) {
  std::unordered_set<coord_t> visited;
  coord_t dir = {0, -1};
  while (true) {
    visited.emplace(guard);

    auto next = guard + dir;
    if (!valid(next, map)) {
      return visited;
    }

    if (at(next, map) == false) {
      dir = rotate_right(dir);
      continue;
    }

    guard = next;
  }
}

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

  coord_t dir = {0, -1};
  int count = 0;

  std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();

  auto visited = traverse(map, guard);
  int n = 0;
  int m = visited.size();
  for (auto p : visited) {
    if (n++ % 100 == 0) {
      std::println("Progress: {}/{} (count so far: {})", n, m, count);
    }
    if (guard == p) {
      continue;
    }

    map[p.y][p.x] = false;
    if (is_loop(map, guard)) {
      count++;
    }
    map[p.y][p.x] = true;
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::println(
      "Count: {}\nTook {}", count,
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
}
