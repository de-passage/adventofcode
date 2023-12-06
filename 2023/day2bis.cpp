#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "utils.hpp"

using namespace std;

struct cube {
  size_t r;
  size_t g;
  size_t b;
};

ostream &operator<<(ostream &os, const cube &c) {
  os << "cube(" << c.r << ", " << c.g << ", " << c.b << ")";
  return os;
}

struct cubes {
  vector<cube> cubes;
  size_t id;
};

cubes parse_cubes(string_view line) {
  auto id = next_number(line);
  if (!id.has_value())
    return {};

  vector<cube> cubes;

  size_t pos = id->position.end;
  cube current_cube{};
  for (;;) {
    auto nb = next_number(line, pos);
    if (!nb.has_value())
      break;
    pos = nb->position.end;
    auto nr = next_word(line, pos);
    if (!nr.has_value())
      break;
    pos = nr->position.end;

    if (nr->value == "red") {
      current_cube.r = nb->value;
    } else if (nr->value == "green") {
      current_cube.g = nb->value;
    } else if (nr->value == "blue") {
      current_cube.b = nb->value;
    }

    pos = line.find_first_of(";,", pos);
    if (pos == string_view::npos) {
      cubes.push_back(current_cube);
      break;
    }
    if (line[pos] == ';') {
      cubes.push_back(current_cube);
      current_cube = {};
      pos++;
    }
  }
  return {cubes, id->value};
}

ostream &operator<<(ostream &os, const cubes &c) {
  os << "game{" << c.id << ": [";
  for (auto i = 0uz; i < c.cubes.size(); ++i) {
    if (i != 0)
      os << ", ";
    os << c.cubes[i];
  }
  return os << "]}";
}

constexpr cube MAX_VALUES = {
    .r = 12,
    .g = 13,
    .b = 14,
};

bool acceptableCube(const cube &c) {
  return c.r >= 0 && c.r <= MAX_VALUES.r && c.g >= 0 && c.g <= MAX_VALUES.g &&
         c.b >= 0 && c.b <= MAX_VALUES.b;
}

cube min(const cube &left, const cube &right) {
  return {
    .r = min(left.r, right.r),
    .g = min(left.g, right.g),
    .b = min(left.b, right.b),
  };
}

cube max(const cube &left, const cube &right) {
  return {
    .r = max(left.r, right.r),
    .g = max(left.g, right.g),
    .b = max(left.b, right.b),
  };
}

int main(int argc, const char **argv) {

  fstream file = get_input("day2.txt", argc, argv);
  uint64_t sum = 0;

  string line;
  while (getline(file, line)) {
    auto cubes = parse_cubes(line);
    cube min_values = {0, 0, 0};
    log(cubes, " -> ");
    for (const auto &c : cubes.cubes) {
      min_values = max(min_values, c);
    }
    logln("computed min: ", min_values);
    sum += min_values.r * min_values.g * min_values.b;
  }
  cout << sum << endl;
}
