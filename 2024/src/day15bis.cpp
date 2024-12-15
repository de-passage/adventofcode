#include "utils.hpp"
#include <cassert>

struct vec2 {
  int x, y;

  friend constexpr vec2 operator+(const vec2 &left,
                                  const vec2 &right) noexcept {
    return {left.x + right.x, left.y + right.y};
  }

  friend constexpr vec2 operator-(const vec2 &left,
                                  const vec2 &right) noexcept {
    return {left.x - right.x, left.y - right.y};
  }

  friend constexpr bool operator==(const vec2 &left,
                                   const vec2 &right) noexcept {
    return left.x == right.x && left.y == right.y;
  }
};

enum class cell : uint8_t {
  EMPTY = 0,
  WALL = 1,
  BLOCK_LEFT = 2,
  BLOCK_RIGHT = 4,
};

bool is_block(cell c) {
  return c == cell::BLOCK_LEFT || c == cell::BLOCK_RIGHT;
}

using map_t = std::vector<std::vector<cell>>;

void move(vec2 &pos, const vec2 &dir, map_t &map) {
  auto new_pos = pos + dir;
  if (map[new_pos.y][new_pos.x] == cell::WALL) {
    return;
  }
  if (is_block(map[new_pos.y][new_pos.x])) {
    // REWORK HERE
    //
    // we need to :
    // * if horizontal move (x != 0) then same as before but rewrite the blocks
    // * if vertical move (y != 0) then we need to look for the other part of
    // the block, then
    //   check if both parts are clear. If they are we need to check for both
    //   halves to see if they can be moved (set of halves ?) if they can we go
    //   over the set and increase all the positions (set the old position to
    //   empty, the new one to whatever was in old position).
    if (dir.x != 0) {
      auto empty_pos = new_pos + dir;

      while (is_block(map[empty_pos.y][empty_pos.x])) {
        empty_pos = empty_pos + dir;
      }

      if (map[empty_pos.y][empty_pos.x] == cell::WALL) {
        return;
      }

      cell to_insert = cell::BLOCK_LEFT;

      while (empty_pos != new_pos) {
        map[empty_pos.y][empty_pos.x] = to_insert;
        empty_pos = empty_pos - dir;
        to_insert = (cell)((uint8_t)to_insert ^ 6);
        assert(to_insert == cell::BLOCK_LEFT || to_insert == cell::BLOCK_RIGHT);
      }

      map[new_pos.y][new_pos.x] = cell::EMPTY;
    } else if (dir.y != 0) {
    }
  }

  pos = new_pos;
}

DPSG_AOC_MAIN(file) {
  map_t map;
  std::vector<vec2> path;

  vec2 pos{0, 0};

  int y = 0;
  auto lines = dpsg::lines(file);
  for (auto line : lines) {
    if (line.empty()) {
      break;
    }
    map.push_back(std::vector<cell>(line.size() * 2));
    int x = 0;
    for (auto ch : line) {
      if (ch == '#') {
        map[y][x] = cell::WALL;
      } else if (ch == '.') {
        map[y][x] = cell::EMPTY;
      } else if (ch == 'O') {
        map[y][x] = cell::BLOCK_LEFT;
      } else if (ch == '@') {
        map[y][x] = cell::EMPTY;
        pos = {x, y};
      }
      x += 2;
    }
    y++;
  }

  for (auto line : lines) {
    for (auto c : line) {
      switch (c) {
      case '^':
        path.push_back({0, -1});
        break;

      case 'v':
        path.push_back({0, 1});
        break;

      case '<':
        path.push_back({-1, 0});
        break;

      case '>':
        path.push_back({1, 0});
        break;

      default:
        std::printf("Invalid character: %c\n", c);
        assert(false && "Invalid character");
      }
    }
  }

  for (auto dir : path) {
    int y = 0;
    for (auto &row : map) {
      int x = 0;
      for (auto cell : row) {
        switch (cell) {
        case cell::EMPTY:
          if (pos == vec2{x, y}) {
            std::printf("@");
          } else {
            std::printf(".");
          }
          break;

        case cell::WALL:
          std::printf("#");
          break;

        case cell::BLOCK_LEFT:
          std::printf("[");
          break;
        case cell::BLOCK_RIGHT:
          std::printf("]");
          break;
        }
        x++;
      }
      y++;
      std::printf("\n");
    }
    std::printf("Current position: %d, %d\n", pos.x, pos.y);
    std::getchar();
    move(pos, dir, map);
  }

  long long result = 0;
  for (int y = 0; y < map.size(); ++y) {
    for (int x = 0; x < map[y].size(); ++x) {
      if (map[y][x] == cell::BLOCK_LEFT) {
        result += x + y * 100;
      }
    }
  }
  std::printf("Result: %lld\n", result);
}
