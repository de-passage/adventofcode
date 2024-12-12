#include "utils.hpp"
#include <print>
#include <stack>
#include <unordered_map>
#include <unordered_set>

struct plot {
  size_t area{0};
  size_t perimeter{0};
};
using map_t = std::vector<std::vector<char>>;

template <> struct std::hash<std::pair<int, int>> {
  std::size_t operator()(const std::pair<int, int> &p) const {
    return std::hash<int>{}(p.first) * 140 + std::hash<int>{}(p.second);
  }
};

using visited_t = std::unordered_set<std::pair<int, int>>;

void flood_fill(const map_t &map, visited_t &visited, int x, int y,
                plot &plot) {

  std::stack<std::pair<int, int>> to_visit;

  to_visit.push({x, y});
  char plot_id = map[x][y];

  while (!to_visit.empty()) {

    auto [x, y] = to_visit.top();
    to_visit.pop();
    visited.insert({x, y});
    plot.area++;

    for (auto [nx, ny] : {std::pair{0, 1}, {0, -1}, {1, 0}, {-1, 0}}) {
      int new_x = x + nx;
      int new_y = y + ny;
      bool is_visited = visited.contains({new_x, new_y});

      if (new_x < 0 || new_x >= map.size() || new_y < 0 ||
          new_y >= map[0].size()) {

        plot.perimeter++;
        continue;
      }

      char new_plot_id = map[new_x][new_y];

      if (new_plot_id != plot_id) {
        plot.perimeter++;
        continue;
      }

      if (is_visited) {
        continue;
      }
      visited.insert({new_x, new_y});
      to_visit.push({new_x, new_y});
    }
  }
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
      plot plot;
      flood_fill(map, visited, i, j, plot);
      plots.push_back({map[i][j], plot});
    }
  }

  size_t sum = 0;
  for (const auto &[id, plot] : plots) {
    sum += plot.area * plot.perimeter;
  }
  std::println("Sum: {}", sum);
  fflush(stdout);
}
