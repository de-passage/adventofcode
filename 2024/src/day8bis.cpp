#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <unordered_map>
#include <unordered_set>

struct coord_t {
  int x;
  int y;

  friend constexpr bool operator==(const coord_t &lhs,
                                   const coord_t &rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }

  friend constexpr coord_t operator-(const coord_t &lhs,
                                     const coord_t &rhs) noexcept {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
  }

  friend constexpr coord_t operator+(const coord_t &lhs,
                                     const coord_t &rhs) noexcept {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
  }

  friend constexpr coord_t abs(const coord_t &c) noexcept {
    return {std::abs(c.x), std::abs(c.y)};
  }
};

template <> struct std::hash<coord_t> {
  constexpr std::size_t operator()(const coord_t &c) const noexcept {
    return std::hash<int>{}(c.x) * 50 + std::hash<int>{}(c.y);
  }
};

using coord_set = std::unordered_set<coord_t>;
using antennas = std::unordered_map<char, coord_set>;

bool valid(coord_t c, coord_t bounds) {
  return c.x >= 0 && c.y >= 0 && c.x < bounds.x && c.y < bounds.y;
}

coord_set antinodes(coord_t a, coord_t b, coord_t bounds) {
  coord_set result;
  coord_t distance = a - b;

  coord_t next = a;
  while (valid(next, bounds)) {
    result.insert(next);
    next = next + distance;
  }

  next = b;
  while (valid(next, bounds)) {
    result.insert(next);
    next = next - distance;
  }

  return result;
}

DPSG_AOC_MAIN(file) {
  antennas antennas;
  coord_set antinode_set;
  coord_t bounds = {0, 0};
  std::unordered_map<coord_t, char> symbols;

  for (auto line : dpsg::lines(file)) {
    bounds.x = line.size();
    for (int x = 0; x < line.size(); x++) {
      if (line[x] != '.') {
        antennas[line[x]].insert({x, bounds.y});
        symbols[coord_t{x, bounds.y}] = line[x];
      }
    }
    bounds.y++;
  }

  for (auto &[c, coords] : antennas) {
    for (auto it = coords.begin(); it != coords.end(); it++) {
      for (auto jt = std::next(it); jt != coords.end(); jt++) {
        auto r = antinodes(*it, *jt, bounds);
        antinode_set.merge(r);
      }
    }
  }

  if (get_log_level() >= log_level::debug) {
    for (int y = 0; y < bounds.y; ++y) {
      for (int x = 0; x < bounds.x; ++x) {
        if (symbols.contains(coord_t{x, y})) {
          dpsg::print("{}", symbols[coord_t{x, y}]);
        } else if (antinode_set.contains(coord_t{x, y})) {
          dpsg::print("#");
        } else {
          dpsg::print(".");
        }
      }
      dpsg::println();
    }
  }
  dpsg::println("Antinodes: {}", antinode_set.size());
}
