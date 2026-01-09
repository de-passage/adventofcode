#include "utils.hpp"
#include <cassert>
#include <chrono>
#include "print.hpp"
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
    return std::hash<int>{}(c.x) * 130 + std::hash<int>{}(c.y);
  }
};

template <> struct std::hash<std::pair<coord_t, coord_t>> {
  std::size_t operator()(const std::pair<coord_t, coord_t> &c) const {
    return std::hash<coord_t>{}(c.first) ^ std::hash<coord_t>{}(c.second);
  }
};

enum class dir_t {
  up,
  right,
  down,
  left,
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

template <class F> void time(F f) {
  using namespace std::chrono;
  using clock = steady_clock;
  using tp = clock::time_point;

  tp start = clock::now();
  f();
  tp end = clock::now();
  dpsg::println("Took {}", duration_cast<microseconds>(end - start));
}

using edge = std::pair<coord_t, coord_t>;
using directed_edges = std::vector<edge>;

struct graph {
  directed_edges edges[4];

  directed_edges &operator[](dir_t d) { return edges[static_cast<int>(d)]; }
};

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

  time([&] {
    // upward paths.
    // Ends at (0, 1) before an obstacle. Unless there is an obstacle in the
    // cell start at an empty cell that is (-1, > y) (1 left & below the
    // obstacle).
    // In other words, we keep a list the width of the map indicating
    // the y coordinate of the last obstacle. When we meet an obstacle, we
    // update the list, and add an edge at x + 1
    //
    // In the same path, we can also find rightward paths, by recording the
    // obstacles in the last line (which are simply the indices of the y
    // coordinates of the bottom-most obstacles that are equal to the last line
    // index).
    //
    // leftward paths are similar, except that the obstacle is below the start
    // vertex, we'll need to add paths for y-1 when we meet an obstacle. The
    // edge will go from current x to the x of the next obstacle on the left on
    // the last line. This info can be saved in a single variable updated at
    // each increase of x.
    //
    // for downward paths, we need to keep track of all the obstacles on each
    // line...
    // does this mean adding a list (1 per col) of list of indices that is
    // cleared at each obstacle? We can keep track in the final list, with
    // placeholder values
    //
    // I use -2 to indicate no obstacle because -1 will be a valid result in the
    // calculations below (and would result in incorrect edges "turning" from
    // the edge of the map).
    std::vector<int> last_obstacle(map[0].size(), -2);
    assert(last_obstacle.size() == map[0].size());
    std::vector<directed_edges> upward_edges(map[0].size());
    std::vector<directed_edges> rightward_edges(map.size());
    std::vector<directed_edges> leftward_edges(map.size());

    // we go over each line
    for (int y = 0; y < map.size(); y++) {
      // bookkeeping
      // this is the index of the cell after the last obstacle on this line.
      int idx_last_obstacle = 0;
      int idx_rightmost_obstacle = -1;

      // we go over each cell in the line
      for (int x = 0; x < map[y].size(); x++) {
        // we keep track of the right-most obstacle on the previous line that is
        // less than the current x;
        if (last_obstacle[x] == (y - 1)) {
          idx_rightmost_obstacle = x;
        }

        // if it's an obstacle
        if (!map[y][x]) {
          // we update the current column to record the y coordinate of the
          // current bottom-most known obstacle.
          last_obstacle[x] = y;

          // If there is no obstacle right after it, we add an edge (upward)
          // starting from the right of the obstacle, going up to the
          // bottom-most known obstacle on the same column
          if (x < map[y].size() && map[y][x + 1]) {
            upward_edges[x + 1].push_back(
                {{x + 1, y}, {x + 1, last_obstacle[x + 1]}});
          }

          // going from the last obstacle to the current one, we add an edge
          // (rightward) for each obstacle that was on the previous line.
          for (; idx_last_obstacle < x; idx_last_obstacle++) {
            if (last_obstacle[idx_last_obstacle] == (y - 1)) {
              rightward_edges[y].push_back(
                  {{idx_last_obstacle, y}, {(x - 1), y}});
            }
          }

          // we add an edge (leftward) going from the current x at y-1 to the x
          // of the right-most obstacle on the previous line, unless it is right
          // above the current obstacle.
          if (idx_rightmost_obstacle != x) {
            leftward_edges[y - 1].push_back(
                {{x, y - 1}, {idx_rightmost_obstacle, y - 1}});
          }

        } // if (!map[y][x])
      } // loop over each cell in the line

      // Complete the edges going out the right side of the map.
      for (; idx_last_obstacle < map[y].size(); idx_last_obstacle++) {
        if (last_obstacle[idx_last_obstacle] == (y - 1)) {
          rightward_edges[y].push_back(
              {{idx_last_obstacle, y}, {(int)map[y].size(), y}});
        }
      }

    } // loop over each line

    for (int i = 0; i < upward_edges.size(); i++) {
      dpsg::print("Edges on column: {}\n", i);
      for (auto [a, b] : upward_edges[i]) {
        dpsg::print("({}, {}) -> ({}, {}), ", a.x, a.y, b.x, b.y);
      }
      dpsg::println();
    }
    for (int i = 0; i < rightward_edges.size(); i++) {
      dpsg::print("Edges on line: {}\n", i);
      for (auto [a, b] : rightward_edges[i]) {
        dpsg::print("({}, {}) -> ({}, {}), ", a.x, a.y, b.x, b.y);
      }
      dpsg::println();
    }
  });
}
