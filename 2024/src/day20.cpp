#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <queue>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

struct vec2 {
  int x, y;

  friend constexpr bool operator==(vec2 lhs, vec2 rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }

  friend constexpr vec2 operator+(vec2 lhs, vec2 rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
  }
};

template <> struct std::hash<vec2> {
  std::size_t operator()(vec2 const &v) const noexcept {
    return std::hash<int>{}(v.x) * 53365 ^ std::hash<int>{}(v.y);
  }
};

struct pos {
  vec2 position;
  int cheat_ = 0;
  int steps = 0;

  bool can_cheat() const { return cheat_ == 0; }

  bool is_cheating() const { return cheat_ == 1; }

  void cheat() {
    assert(cheat_ < 2);
    ++cheat_;
  }

  friend constexpr bool operator==(pos lhs, pos rhs) {
    return lhs.position == rhs.position && lhs.cheat_ == rhs.cheat_;
  }

  friend constexpr pos operator+(pos lhs, vec2 rhs) {
    return {lhs.position + rhs, lhs.cheat_, lhs.steps + 1};
  }
};

template <> struct std::hash<pos> {
  std::size_t operator()(pos const &p) const noexcept {
    return std::hash<vec2>{}(p.position) * 53365 ^ std::hash<int>{}(p.cheat_);
  }
};

template <> struct std::hash<std::pair<vec2, vec2>> {
  std::size_t operator()(std::pair<vec2, vec2> const &p) const noexcept {
    return std::hash<vec2>{}(p.first) * 53365 ^ std::hash<vec2>{}(p.second);
  }
};

size_t manhattan_distance(vec2 a, vec2 b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

DPSG_AOC_MAIN(file) {
  std::vector<std::vector<int>> weights;
  vec2 start;
  vec2 end;
  for (auto line : dpsg::lines(file)) {
    weights.emplace_back();
    auto &row = weights.back();
    row.reserve(line.size());
    for (int x = 0; x < line.size(); ++x) {
      if (line[x] == 'S') {
        start = {x, static_cast<int>(weights.size() - 1)};
        row.push_back(0);
      } else if (line[x] == 'E') {
        end = {x, static_cast<int>(weights.size() - 1)};
        row.push_back(-1);
      } else if (line[x] == '#') {
        row.push_back(std::numeric_limits<int>::max());
      } else {
        row.push_back(-1);
      }
    }
  }
  assert((start != vec2{0, 0}));
  assert((end != vec2{0, 0}));
  const auto at = [&](vec2 v) { return weights[v.y][v.x]; };
  const auto is_wall = [&](vec2 v) {
    return at(v) == std::numeric_limits<int>::max();
  };
  const auto out_of_bounds = [&](vec2 v) {
    return v.x < 0 || v.y < 0 || v.y >= weights.size() ||
           v.x >= weights[v.y].size();
  };

  vec2 current = start;
  int current_step = 0;
  while (current != end) {

    for (auto dir : {vec2{0, 1}, vec2{1, 0}, vec2{0, -1}, vec2{-1, 0}}) {
      auto next = current + dir;
      if (out_of_bounds(next) || is_wall(next)) {
        continue;
      }

      if (at(next) == -1) {
        weights[next.y][next.x] = ++current_step;
        current = next;
        goto end;
      }
    }
    std::fflush(stdout);
    std::println(stderr, "No path found from ({},{})", current.x, current.y);
    exit(1);
  end:;
  }
  const int max_score = current_step;

  current = start;
  current_step = 0;
  std::unordered_set<std::pair<vec2, vec2>> shortcuts;

  while (current != end) {

    for (auto step : {vec2{2, 0}, vec2{0, 2}, vec2{-2, 0}, vec2{0, -2},
                      vec2{1, 1}, vec2{1, -1}, vec2{-1, 1}, vec2{-1, -1}}) {
      auto next = current + step;
      if (out_of_bounds(next)) {
        continue;
      }
      if (is_wall(next)) {
        continue;
      }
      auto step_value = at(next) - at(current);
      if (step_value > 2) {
        shortcuts.insert({current, next});
      }
    }

    // Move onto the next cell
    for (auto dir : {vec2{0, 1}, vec2{1, 0}, vec2{0, -1}, vec2{-1, 0}}) {
      auto next = current + dir;
      if (out_of_bounds(next) || is_wall(next)) {
        continue;
      }

      if (at(next) > at(current)) {
        current = next;
        break;
      }
    }
  }

  std::unordered_map<int, int> counted_shortcuts;
  for (auto [from, to] : shortcuts) {
    auto steps = at(to) - at(from);
    ++counted_shortcuts[steps - 2];
  };

  std::vector<std::pair<int, int>> sorted_shortcuts;
  sorted_shortcuts.reserve(counted_shortcuts.size());
  std::ranges::copy(counted_shortcuts, std::back_inserter(sorted_shortcuts));
  std::ranges::sort(sorted_shortcuts, std::greater<>{});

  size_t over_100 = 0;
  for (auto [steps, count] : sorted_shortcuts) {
    if (steps >= 100) {
      over_100 += count;
    }
  }
  std::println("Shortcuts over 100 steps: {}", over_100);
}
