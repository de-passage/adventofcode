#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "utils.hpp"

using namespace std;

const auto NUMBERS = "0123456789";
const auto LETTERS = "abcdefghijklmnopqrstuvwxyz";

int atoi(string_view str, size_t pos, size_t end) {
  int result = 0;
  for (size_t i = pos; i < end; ++i) {
    result *= 10;
    result += str[i] - '0';
  }
  return result;
}

struct range {
  size_t begin;
  size_t end;
};

range find_range(string_view str, const char *pattern, size_t pos) {
  auto begin = str.find_first_of(pattern, pos);
  auto end = string_view::npos;
  if (begin != string_view::npos) {
    end = str.find_first_not_of(pattern, begin);
    if (end == string_view::npos) {
      end = str.size();
    }
  }
  return {begin, end};
}

bool isValid(range r) { return r.begin != string_view::npos; }

template <class T> struct ranged {
  T value;
  range position;
};

optional<ranged<int>> next_number(string_view str, size_t pos = 0) {
  auto r = find_range(str, NUMBERS, pos);
  if (isValid(r)) {
    return {{atoi(str, r.begin, r.end), r}};
  }
  return {};
}

optional<ranged<string_view>> next_word(string_view str, size_t pos = 0) {
  auto r = find_range(str, LETTERS, pos);
  if (isValid(r)) {
    return {{str.substr(r.begin, r.end - r.begin), r}};
  }
  return {};
}

struct cube {
  int r;
  int g;
  int b;
};

ostream &operator<<(ostream &os, const cube &c) {
  os << "cube(" << c.r << ", " << c.g << ", " << c.b << ")";
  return os;
}

struct cubes {
  vector<cube> cubes;
  int id;
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

int main(int argc, char **argv) {

  fstream file = get_input("day2.txt", argc, argv);
  uint64_t sum = 0;

  string line;
  while (getline(file, line)) {
    auto cubes = parse_cubes(line);
    log(cubes);
    if (all_of(cubes.cubes.begin(), cubes.cubes.end(), acceptableCube)) {
      sum += cubes.id;
      logln("accepted -> ", sum);
    } else {
      logln("rejected");
    }
  }
  cout << sum << endl;
}
