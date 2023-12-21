#include "utils.hpp"
#include "vt100.hpp"
#include <vector>

enum class cell {
  empty,
  wall,
  current,
};

using map_t = std::vector<std::vector<cell>>;

template <class T>
std::basic_ostream<T> &operator<<(std::basic_ostream<T> &os, const map_t &map) {
  using namespace dpsg::vt100;
  for (auto &row : map) {
    for (auto c : row) {
      switch (c) {
      case cell::empty:
        os << decorate(faint, '.');
        break;
      case cell::wall:
        os << decorate(bold|cyan, '#');
        break;
      case cell::current:
        os << decorate(red | bold, 'O');
        break;
      default:
        os << decorate(red | bold,'?');
        break;
      }
    }
    os << '\n';
  }
  return os;
}

map_t next_step(const map_t &map) {
  map_t next;
  next.resize(map.size(), std::vector<cell>(map[0].size(), cell::empty));


  for (auto x = 0u; x < map.size(); ++x) {
    for (auto y = 0u; y < map[x].size(); ++y) {
      auto c = map[x][y];
      if (c == cell::current) {
        if (x > 0 && map[x - 1][y] != cell::wall) {
          next[x - 1][y] = cell::current;
        }
        if (x < map.size() - 1 && map[x + 1][y] != cell::wall) {
          next[x + 1][y] = cell::current;
        }
        if (y > 0 && map[x][y - 1] != cell::wall) {
          next[x][y - 1] = cell::current;
        }
        if (y < map[x].size() - 1 && map[x][y + 1] != cell::wall) {
          next[x][y + 1] = cell::current;
        }
      } else if (c == cell::wall) {
        next[x][y] = c;
      }
    }
  }
  return next;
}

DPSG_AOC_MAIN(file) {
  using namespace std;
  using namespace dpsg;
  using namespace dpsg::vt100;

  string line;
  int x = 0;
  map_t map;
  while (getline(file, line)) {
    map.emplace_back(line.size(), cell::empty);
    for (auto y = 0u; y < line.size(); ++y) {
      auto c = line[y];
      switch (c) {
      case '#':
        map[x][y] = cell::wall;
        break;
      case '.':
        map[x][y] = cell::empty;
        break;
      case 'S':
        map[x][y] = cell::current;
        break;
      }
    }
    x++;
  }

  cout << decorate(vt100::reverse, "Initial state") << endl;
  std::cout << map << std::endl;
  for (int i = 0; i < 64; ++i) {
    map = next_step(map);
    cout << decorate(vt100::reverse, "After step " + std::to_string(i + 1)) << endl;
    std::cout << map;
    auto count = 0uz;
    for (auto &row : map) {
      for (auto c : row) {
        if (c == cell::current) {
          count++;
        }
      }
    }
    std::cout << decorate(vt100::reverse, "Current count: " + std::to_string(count)) << '\n' << std::endl;
  }
}
