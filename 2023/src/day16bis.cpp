#include "coord.hpp"
#include "utils.hpp"
#include "day16.hpp"

#include <algorithm>
#include <queue>

DPSG_AOC_MAIN(file) {
  using namespace std;
  using namespace dpsg;

  string line;
  using grid = vector<vector<cell>>;
  grid map;
  while (getline(file, line)) {
    auto &v = map.emplace_back();
    for (auto ch : line) {
      switch (ch) {
      case '.':
        v.push_back({cell_type::empty, ray_origin::none});
        break;
      case '-':
        v.push_back({cell_type::split_horizontal, ray_origin::none});
        break;
      case '|':
        v.push_back({cell_type::split_vertical, ray_origin::none});
        break;
      case '/':
        v.push_back({cell_type::reflect_up, ray_origin::none});
        break;
      case '\\':
        v.push_back({cell_type::reflect_down, ray_origin::none});
        break;
      default:
        std::cerr << "invalid character: " << ch << std::endl;
        exit(1);
      }
    }
  }

  auto count = 0ul;
  for (auto i = 0u; i < map.size(); ++i) {
    count = std::max(count_energized_tiles(map, {.pos = {i, 0}, .origin = ray_origin::left}), count);
    count = std::max(count_energized_tiles(map, {.pos = {i, map[i].size() - 1}, .origin = ray_origin::right}), count);
  }
  for (auto i = 0u; i < map[0].size(); ++i) {
    count = std::max(count_energized_tiles(map, {.pos = {0, i}, .origin = ray_origin::up}), count);
    count = std::max(count_energized_tiles(map, {.pos = {map.size() - 1, i}, .origin = ray_origin::down}), count);
  }

  std::cout << "Max cells energized: " << (vt100::bold | vt100::green) << count << vt100::reset << std::endl;
}
