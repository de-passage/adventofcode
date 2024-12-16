#include "utils.hpp"
#include <cassert>
#include <print>

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

  size_t top_left = 0;
  size_t top_right = 0;
  size_t bottom_left = 0;
  size_t bottom_right = 0;

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

    auto after_100 = v1 + v2 * TURNS;
    after_100 = vec2{
        .x = after_100.x % WIDTH,
        .y = after_100.y % HEIGHT,
    };
    after_100 = {
        .x = after_100.x < 0 ? WIDTH + after_100.x : after_100.x,
        .y = after_100.y < 0 ? HEIGHT + after_100.y : after_100.y,
    };

    std::print("Initial: {}, {}\n", v1, v2);
    std::print("After {} seconds: {}\n", TURNS, after_100);

    if (after_100.x < WIDTH / 2) {
      if (after_100.y < HEIGHT / 2) {
        top_left++;
        std::println("Top left");
      } else if (after_100.y > HEIGHT / 2) {
        bottom_left++;
        std::println("Bottom left");
      }
    } else if (after_100.x > WIDTH / 2) {
      if (after_100.y < HEIGHT / 2) {
        top_right++;
        std::println("Top right");
      } else if (after_100.y > HEIGHT / 2) {
        bottom_right++;
        std::println("Bottom right");
      }
    }
    std::println();
  }
  std::println("Top left: {}", top_left);
  std::println("Top right: {}", top_right);
  std::println("Bottom left: {}", bottom_left);
  std::println("Bottom right: {}", bottom_right);
  std::println("Score: {}", top_left * top_right * bottom_left * bottom_right);
}
