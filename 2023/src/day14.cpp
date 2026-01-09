#include "utils.hpp"

using namespace std;
using namespace dpsg;

struct mass_counter {
  size_t start = 0;
  int count = 0;
};

int main(int argc, const char **argv) {
  auto file = get_input("day14.txt", argc, argv);

  string line;
  size_t line_count = 0;
  vector<vector<mass_counter>> data;
  while (getline(file, line)) {
    if (data.size() == 0) {
      data.resize(line.size());
      for (auto i = 0u; i < line.size(); ++i) {
        data[i].emplace_back();
      }
    }

    line_count++;

    for (auto i = 0u; i < line.size(); ++i) {
      auto c = line[i];
      if (c == '.') {
        // nothing to do
      } else if (c == '#') {
        data[i].push_back({.start = line_count, .count = 0});
      } else if (c == 'O') {
        data[i].back().count++;
      }
    }
  }

  size_t mass = 0;
  for (auto &v : data) {
    for (auto &c : v) {
      auto start_value = line_count - c.start;
      auto s = start_value * (start_value + 1) / 2;
      auto end_value = start_value - c.count;
      auto e = end_value * (end_value + 1) / 2;
      mass += s - e;
    }
  }

  std::cout << mass << std::endl;


  return 0;
}
