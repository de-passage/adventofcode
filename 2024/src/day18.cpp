#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <queue>
#include <unordered_set>

struct vec2 {
  long x;
  long y;

  friend constexpr bool operator==(const vec2 &a, const vec2 &b) noexcept {
    return a.x == b.x && a.y == b.y;
  }

  friend constexpr vec2 operator+(const vec2 &a, const vec2 &b) noexcept {
    return {a.x + b.x, a.y + b.y};
  }
};

struct vec2_with_weight {
  vec2 pos;
  size_t weight;
};

struct by_weight {
  constexpr bool operator()(const vec2_with_weight &a,
                            const vec2_with_weight &b) const noexcept {
    return a.weight > b.weight;
  }
};

template <> struct std::hash<vec2> {
  constexpr std::size_t operator()(const vec2 &v) const noexcept {
    return std::hash<int>{}(v.x) * 71 + std::hash<int>{}(v.y);
  }
};

constexpr size_t MAX_X = 70;
constexpr size_t MAX_Y = 70;

DPSG_AOC_MAIN(file) {
  std::unordered_set<vec2> obstacles;

  int x = 0;
  for (auto line : dpsg::lines(file)) {
    if (x++ == 1024)
      break;
    auto fn = next_number(line);
    auto sn = next_number(line, fn->position.end);
    vec2 pos{(long)fn->value, (long)sn->value};
    obstacles.insert(pos);
  }
  assert(obstacles.size() == 1024);

  vec2 pos{0, 0};
  vec2 target{MAX_X, MAX_Y};

  std::priority_queue<vec2_with_weight, std::vector<vec2_with_weight>,
                      by_weight>
      to_visit;

  to_visit.push({pos, 0});
  std::unordered_set<vec2> visited;
  visited.insert(pos);

  int iteration = 0;
  while (!to_visit.empty()) {
    auto [current, weight] = to_visit.top();
    to_visit.pop();

    if (current == target) {
      std::print("Path found with weight: {}\n", weight);
      break;
    }

    for (auto next : {vec2{1, 0}, vec2{0, 1}, vec2{-1, 0}, vec2{0, -1}}) {
      auto next_pos = current + next;
      if (next_pos.x < 0 || next_pos.y < 0 || next_pos.x > MAX_X ||
          next_pos.y > MAX_Y || obstacles.contains(next_pos) ||
          visited.contains(next_pos)) {
        continue;
      }
      to_visit.push({next_pos, weight + 1});
      visited.insert(next_pos);
    }
  }
}
