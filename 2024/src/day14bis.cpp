#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <format>

struct vec2 {
  int64_t x;
  int64_t y;

  friend constexpr bool operator==(const vec2 &lhs, const vec2 &rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }

  friend constexpr vec2 operator+(const vec2 &lhs, const vec2 &rhs) noexcept {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
  }

  friend constexpr vec2 operator*(const vec2 &lhs, int64_t rhs) noexcept {
    return {lhs.x * rhs, lhs.y * rhs};
  }
};

template <> struct std::formatter<vec2, char> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  FormatContext::iterator format(const vec2 &v, FormatContext &ctx) const {
    return std::format_to(ctx.out(), "{{{},{}}}", v.x, v.y);
  }
};

struct robot {
  vec2 position;
  vec2 velocity;
};

DPSG_AOC_MAIN(file) {
  const int64_t HEIGHT = 103;
  const int64_t WIDTH = 101;
  const int64_t TURNS = 100;

  std::vector<robot> robots;

  for (auto line : dpsg::lines(file)) {
    auto n1 = next_number_with_sign(line);
    assert(n1.has_value());
    auto n2 = next_number_with_sign(line, n1->position.end);
    assert(n2.has_value());
    auto n3 = next_number_with_sign(line, n2->position.end);
    assert(n3.has_value());
    auto n4 = next_number_with_sign(line, n3->position.end);
    assert(n4.has_value());

    auto v1 = vec2{n1->value, n2->value};
    auto v2 = vec2{n3->value, n4->value};

    robots.push_back({v1, v2});
  }

  long turn = 0;

  while (true) {
    long current_line = 1;
    long max_line = 1;
    long line_count = 0;
    char last = 0;
    std::vector<char> grid(HEIGHT * WIDTH, '.');
    for (auto robot: robots) {
      grid[robot.position.y * WIDTH + robot.position.x] = '#';
    }
    for (int i = 0; i < HEIGHT; ++i) {
      for (int j = 0; j < WIDTH; ++j) {
        char c = grid[i * WIDTH + j];
        if (c == '#' && last == '#') {
          current_line ++;
        } else {
          max_line = std::max(max_line, current_line);
          current_line = 1;
        }
        last = c;
      }

      if (max_line > 10) {
        line_count++;
      }
    }

    if (line_count > 5) {
      for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
          dpsg::print("{}", grid[i * WIDTH + j]);
        }
        dpsg::println();
      }

      dpsg::println("Current turn {}", turn);
      getchar();
    }

    turn++;

    for (auto& robot: robots) {
      robot.position = robot.position + robot.velocity;
      robot.position = vec2 {
        (robot.position.x + WIDTH) % WIDTH,
        (robot.position.y + HEIGHT) % HEIGHT
      };
      robot.position = {
        .x = robot.position.x < 0 ? WIDTH + robot.position.x : robot.position.x,
        .y = robot.position.y < 0 ? HEIGHT + robot.position.y : robot.position.y
      };
    }
  }
}
