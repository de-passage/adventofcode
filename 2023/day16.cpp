#include "utils.hpp"

using namespace std;
using namespace dpsg;

enum class cell_type : char {
  empty = 0, // .
  split_horizontal = 2, // -
  split_vertical = 3, // |
  reflect_up = 4,   //
  reflect_down = 5, // backslash
};

enum class ray_origin : char {
  none = 0, up = 1, down = 2, left = 4, right = 8
};

struct cell {
  cell_type cell: 3;
  ray_origin energized: 4;
};

int main(int argc, const char** argv) {
  auto file = get_input("day16.txt", argc, argv);


  string line;
  using grid = vector<vector<cell>>;
  grid map;
  while (getline(file, line)) {
    auto& v = map.emplace_back();
    for (auto ch : line ) {
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
          throw runtime_error("invalid input");
      }
    }
  }


  return 0;
}
