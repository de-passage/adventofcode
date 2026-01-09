#include "utils.hpp"
#include "coord.hpp"

enum class direction { up, down, left, right };
direction to_direction(char c) {
  switch (c) {
  case '3':
    return direction::up;
  case '1':
    return direction::down;
  case '2':
    return direction::left;
  case '0':
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

int64_t from_hex(std::string_view str) {
  int64_t acc = 0;
  for (auto i = 0; i < str.size(); ++i) {
    acc *= 16;
    auto c = str[i];
    if (c >= '0' && c <= '9') {
      acc += c - '0';
    } else if (c >= 'a' && c <= 'f') {
      acc += c - 'a' + 10;
    }
  }
  return acc;
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

DPSG_AOC_MAIN(file) {
  using namespace std;
  using namespace dpsg;
  using namespace dpsg::vt100;

  string line;

  coordinates last{0, 0};
  int64_t area = 0;
  uint64_t steps = 0;
  while (getline(file, line)) {
    auto beg = line.find_first_of("#");

    if (beg == string::npos) {
      std::cerr << "Invalid line: " << line << '\n';
      exit(1);
    }

    auto v = line.substr(beg + 1, 5);
    auto direction = to_direction(line[line.find_first_of(")") - 1]);
    auto distance = from_hex(v);

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
