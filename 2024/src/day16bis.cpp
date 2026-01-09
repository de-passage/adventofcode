#include "utils.hpp"
#include <cassert>
#include <limits>
#include "print.hpp"
#include <queue>
#include <unordered_map>
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

template <> struct std::hash<std::pair<vec2, vec2>> {
  std::size_t operator()(const std::pair<vec2, vec2> &p) const {
    return hash_size_t(hash_size_t(p.first.x, p.first.y),
                       hash_size_t(p.second.x, p.second.y));
  }
};

struct vec2_with_weight {
  vec2 v;
  size_t weight{0};
  vec2 direction;
  std::unordered_set<vec2> visited;

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

  to_visit.push({start, 0, {1, 0}, {start}});

  size_t max_distance = std::numeric_limits<size_t>::max();
  std::unordered_set<vec2> positions;

  std::unordered_map<std::pair<vec2, vec2>, size_t> shortest_distances;

  while (!to_visit.empty()) {
    auto current = to_visit.top();
    to_visit.pop();
    current.visited.insert(current.v);
    auto best_distance_it =
        shortest_distances.find({current.v, current.direction});

    if (best_distance_it != shortest_distances.end() &&
        best_distance_it->second < current.weight) {
      continue;
    } else {
      shortest_distances[{current.v, current.direction}] = current.weight;
    }

    if (current.v == end) {
      std::fflush(stdout);
      max_distance = std::min(max_distance, current.weight);
      if (current.weight <= max_distance) {
        positions.merge(std::move(current.visited));
      }
    } else if (current.weight >= max_distance) {
      continue;
    }

    for (auto [dir, weight] :
         {std::pair{rotate_left(current.direction), 1001UZ},
          {current.direction, 1},
          {rotate_right(current.direction), 1001}}) {
      auto next = current.v + dir;
      if (next.x < 0 || next.y < 0 || next.y >= map.size() ||
          next.x >= map[next.y].size() || !map[next.y][next.x]) {
        continue;
      }

      vec2_with_weight new_p{
          .v = next,
          .weight = weight + current.weight,
          .direction = dir,
          .visited = current.visited,
      };
      to_visit.push(std::move(new_p));
    }
  }
  dpsg::println("Best path: {}", max_distance);
  dpsg::println("Answer: {}", positions.size());
}
