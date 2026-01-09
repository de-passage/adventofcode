#include "utils.hpp"
#include "print.hpp"
#include <stack>
#include <unordered_set>

struct plot {
  size_t area{0};
  size_t sides{0};
};
using map_t = std::vector<std::vector<char>>;

template <> struct std::hash<std::pair<int, int>> {
  std::size_t operator()(const std::pair<int, int> &p) const {
    return std::hash<int>{}(p.first) * 140 + std::hash<int>{}(p.second);
  }
};

using visited_t = std::unordered_set<std::pair<int, int>>;

bool valid(const map_t &map, int x, int y) {
  return x >= 0 && x < map.size() && y >= 0 && y < map[0].size();
}
using coord = std::pair<int, int>;

plot flood_fill(const map_t &map, visited_t &visited, int x, int y) {

  plot plot;
  std::stack<coord> to_visit;

  to_visit.push({x, y});
  char plot_id = map[x][y];

  while (!to_visit.empty()) {

    auto [x, y] = to_visit.top();
    to_visit.pop();
    visited.insert({x, y});
    plot.area++;

    using tp = std::tuple<coord, coord, coord>;

    for (auto [p1, p2, p3] : {tp{{-1, 0}, {-1, -1}, {0, -1}},
                              {{0, 1}, {-1, 1}, {-1, 0}},
                              {{1, 0}, {1, 1}, {0, 1}},
                              {{0, -1}, {1, -1}, {1, 0}}}) {

      coord p[3] = {p1, p2, p3};
      bool side[3] = {false};
      int diff_sides = 0;
      for (int i = 0; i < 3; ++i) {
        auto [nx, ny] = p[i];
        auto px = x + nx;
        auto py = y + ny;
        if (!valid(map, px, py) || map[px][py] != plot_id) {
          diff_sides++;
          side[i] = true;
        }
      }

      // 3 outside = corner, 1 outside that is the middleone = corner
      if (diff_sides == 3 || (diff_sides == 1 && side[1]) || (diff_sides == 2 && !side[1])) {
        plot.sides++;
      }
    }

    for (auto [nx, ny] : {coord{0, 1}, {0, -1}, {1, 0}, {-1, 0}}) {
      int new_x = x + nx;
      int new_y = y + ny;
      bool is_visited = visited.contains({new_x, new_y});

      if (new_x < 0 || new_x >= map.size() || new_y < 0 ||
          new_y >= map[0].size()) {
        continue;
      }

      if (map[new_x][new_y] != plot_id) {
        continue;
      }

      if (is_visited) {
        continue;
      }

      visited.insert({new_x, new_y});
      to_visit.push({new_x, new_y});
    }
  }
  return plot;
}

DPSG_AOC_MAIN(file) {
  map_t map;
  std::vector<std::pair<char, plot>> plots;

  for (const auto &line : dpsg::lines(file)) {
    map.emplace_back(line.begin(), line.end());
  }

  visited_t visited(map.size() * map[0].size());

  for (int i = 0; i < map.size(); ++i) {
    for (int j = 0; j < map[i].size(); ++j) {
      if (visited.contains({i, j})) {
        continue;
      }
      plot plot = flood_fill(map, visited, i, j);
      plots.push_back({map[i][j], plot});
    }
  }

  size_t sum = 0;
  for (const auto &[id, plot] : plots) {
    sum += plot.area * plot.sides;
  }
  std::println("Sum: {}", sum);
  fflush(stdout);
}

