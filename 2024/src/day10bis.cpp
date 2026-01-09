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
};

template <> struct std::hash<coord_t> {
  size_t operator()(const coord_t &c) const {
    return std::hash<int>()(c.x) * 54 + std::hash<int>()(c.y);
  }
};

struct cell {
  std::vector<coord_t> reachable_trailheads;
  int height;
};

int dfs(const std::vector<std::vector<cell>> &trails, int x, int y, int& score) {

  auto &current = trails[y][x];
  for (auto [xp, yp] : {coord_t{-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
    int nx = x + xp;
    int ny = y + yp;
    if (nx < 0 || ny < 0 || nx >= trails[0].size() || ny >= trails.size()) {
      continue;
    }

    auto& next = trails[ny][nx];

    if (current.height == 8 && next.height == 9) {
      score += 1;
    } else if (next.height == current.height + 1) {
      dfs(trails, nx, ny, score);
    }
  }
  return score;
}

DPSG_AOC_MAIN(file) {

  std::vector<std::vector<cell>> trails;
  std::vector<coord_t> to_visit;

  int y = 0, x = 0;
  for (auto &line : dpsg::lines(file)) {
    trails.push_back({});
    for (char c : line) {
      int v = c - '0';
      trails.back().push_back({{}, v});
      if (v == 0) {
        to_visit.push_back({x, y});
      }

      x++;
    }
    x = 0;
    y++;
  }

  int sum = 0;
  for (auto [x, y] : to_visit) {
    std::unordered_set<coord_t> reachable_trailheads;
    int score = 0;
    dfs(trails, x, y, score);
    sum += score;
  }
  dpsg::println("Result: {}", sum);
}
