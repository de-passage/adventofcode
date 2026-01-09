#include "utils.hpp"
#include "coord.hpp"

enum class direction { up, down, left, right };
direction to_direction(char c) {
  switch (c) {
  case 'U':
    return direction::up;
  case 'D':
    return direction::down;
  case 'L':
    return direction::left;
  case 'R':
    return direction::right;
  default:
    std::cerr << "Invalid direction " << c << '\n';
    exit(1);
  }
}

std::ostream& operator<<(std::ostream& out, direction d) {
  switch (d) {
  case direction::up:
    return out << "↑";
  case direction::down:
    return out << "↓";
  case direction::left:
    return out << "←";
  case direction::right:
    return out << "→";
  }
}

template<class T>
struct colored {
  const T& obj;
};
template<class T>
colored(const T&) -> colored<T>;

std::ostream& operator<<(std::ostream& out, colored<direction> d) {
  using namespace dpsg::vt100;
  return out << (bold|magenta) << d.obj << reset;
}

using coordinates = dpsg::basic_coord<int64_t>;

coordinates move_point(coordinates from, direction d, int64_t distance) {
  switch (d) {
  case direction::up:
    return {from.x, from.y + distance};
  case direction::down:
    return {from.x, from.y - distance};
  case direction::left:
    return {from.x - distance, from.y};
  case direction::right:
    return {from.x + distance, from.y};
  }
}

int main(int argc, const char ** argv) {
  using namespace std;
  using namespace dpsg;
  using namespace dpsg::vt100;

  auto file = get_input("day18.txt", argc, argv);

  string line;

  coordinates last{0, 0};
  int64_t area = 0;
  uint64_t steps = 0;
  while (getline(file, line)) {
    auto direction = to_direction(line[0]);
    auto distance_str = next_number(line.substr(2));
    if (! distance_str) {
      std::cerr << "Invalid line " << line << '\n';
      exit(1);
    }
    auto distance = distance_str->value;


    logln("Direction: ", colored{direction}, " Distance: ", distance);
    auto new_coord = move_point(last, direction, distance);
    logln("New coord: ", new_coord);
    area += (last.x * new_coord.y - last.y * new_coord.x);
    steps += distance;
    last = new_coord;
  }

  area = std::abs(area) / 2;
  logln("Area: ", green|bold , area, reset);
  logln("Steps: ", green|bold,steps, reset);
  auto pick = area - (steps / 2) + 1;
  logln("Points inside: ", green|bold, pick, reset);
  auto total = pick + steps;
  std::cout << "Total: " << (green|bold|vt100::reverse) << total << '\n';
}
