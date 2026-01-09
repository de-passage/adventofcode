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

std::pair<coord_t, coord_t> antinodes(coord_t a, coord_t b) {
  coord_t distance = a - b;
  coord_t right = a + distance;
  coord_t left = b - distance;

  return {left, right};
}

bool valid(coord_t c, coord_t bounds) {
  return c.x >= 0 && c.y >= 0 && c.x < bounds.x && c.y < bounds.y;
}

DPSG_AOC_MAIN(file) {
  antennas antennas;
  coord_set antinode_set;
  coord_t bounds = {0, 0};

  for (auto line : dpsg::lines(file)) {
    bounds.x = line.size();
    for (int x = 0; x < line.size(); x++) {
      if (line[x] != '.') {
        antennas[line[x]].insert({x, bounds.y});
      }
    }
    bounds.y++;
  }

  for (auto &[c, coords] : antennas) {
    for (auto it = coords.begin(); it != coords.end(); it++) {
      for (auto jt = std::next(it); jt != coords.end(); jt++) {
        auto [left, right] = antinodes(*it, *jt);

        if (valid(left, bounds)) {
          antinode_set.insert(left);
        }
        if (valid(right, bounds)) {
          antinode_set.insert(right);
        }
      }
    }
  }

  dpsg::println("Antinodes: {}", antinode_set.size());
}
