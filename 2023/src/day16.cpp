#include "coord.hpp"
#include "utils.hpp"
#include "day16.hpp"

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

  auto count = count_energized_tiles(map, ray { .pos = {0, 0}, .origin = ray_origin::left });

  std::cout << "Total cells energized: " << (vt100::bold | vt100::green) << count << vt100::reset << std::endl;
}
