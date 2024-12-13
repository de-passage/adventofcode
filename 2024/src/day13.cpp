#include "utils.hpp"
#include <cassert>
#include <print>
#include <queue>
#include <unordered_set>

struct coord {
  size_t x, y;
  friend bool operator==(const coord &a, const coord &b) {
    return a.x == b.x && a.y == b.y;
  }
  friend coord operator+(const coord &a, const coord &b) {
    return {a.x + b.x, a.y + b.y};
  }
};

size_t hash_size_t(size_t a, size_t b) {
  std::size_t left = std::hash<size_t>{}(a);
  std::size_t right = std::hash<size_t>{}(b);
  return left ^ (right + 0x9e3779b9 + (left << 6) + (left >> 2));
}

template <> struct std::hash<coord> {
  size_t operator()(const coord &c) const { return hash_size_t(c.x, c.y); }
};

size_t distance_squared(const coord &a, const coord &b) {
  return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

struct coord_with_n {
  coord co;
  size_t a{0}, b{0};

  friend bool operator==(const coord_with_n &a, const coord_with_n &b) {
    return a.co == b.co && a.a == b.a && a.b == b.b;
  }
};

template <> struct std::hash<coord_with_n> {
  size_t operator()(const coord_with_n &c) const {
    return hash_size_t(hash_size_t(c.co.x, c.co.y), hash_size_t(c.a, c.b));
  }
};

struct by_distance {
  coord target;
  bool operator()(const coord_with_n &a, const coord_with_n &b) const {
    return distance_squared(a.co, target) < distance_squared(b.co, target);
  }
};

struct problem {
  coord left, right, target;
};

DPSG_AOC_MAIN(file) {
  enum { left, right, target } state = left;

  std::vector<problem> problems;
  problem current;

  for (auto line : dpsg::lines(file)) {
    if (line.empty()) {
      assert(state == target);
      state = left;
      continue;
    }

    auto on1 = next_number(line);
    assert(on1.has_value());
    auto on2 = next_number(line, on1->position.end);
    assert(on2.has_value());
    switch (state) {
    case left: {
      current.left = {on1->value, on2->value};
      state = right;
      break;
    }
    case right:
      state = target;
      current.right = {on1->value, on2->value};
      break;
    case target:
      current.target = {on1->value, on2->value};
      problems.push_back(current);
      break;
    }
  }

  size_t token = 0;

  for (auto &p : problems) {
    auto [left, right, target] = p;
    auto [lx, ly] = left;
    auto [rx, ry] = right;
    auto [tx, ty] = target;

    std::unordered_set<coord_with_n> visited;
    std::priority_queue<coord_with_n, std::vector<coord_with_n>, by_distance>
        to_visit{{target}};
    coord_with_n current = {{0, 0}, 0, 0};

    to_visit.push(current);

    while (!to_visit.empty()) {
      current = to_visit.top();
      to_visit.pop();

      if (current.co == target) {
        token += current.a * 3 + current.b;
        break;
      } else {
      }
      if (current.a > 100 && current.b > 100) {
        break;
      }
      if (current.co.x > tx || current.co.y > ty) {
        continue;
      }

      if (visited.contains(current)) {
        continue;
      }

      visited.insert(current);

      auto [x, y] = current.co;

      to_visit.push({{x + lx, y + ly}, current.a + 1, current.b});
      to_visit.push({{x + rx, y + ry}, current.a, current.b + 1});
    }
  }
  std::println("total: {}", token);
}
