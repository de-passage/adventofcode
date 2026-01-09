#include "utils.hpp"
#include "algorithms.hpp"
#include <string>
#include <vector>
#include <iostream>

int main(int argc, const char** argv) {
  using namespace std;
  using namespace dpsg;

  auto file = get_input("day11.txt", argc, argv);

  size_t line_idx = 0;
  string line;
  vector<size_t> line_offset;
  vector<size_t> column_offset;
  vector<pair<size_t, size_t>>  galaxies;


  while (std::getline(file, line)) {
    line_offset.push_back(0);
    column_offset.resize(line.size(), 0);
    for (size_t col = 0; col < line.size(); ++col) {
      if (line[col] == '#') {
        line_offset[line_idx]++;
        column_offset[col]++;
        galaxies.emplace_back(line_idx, col);
      }
    }

    line_idx++;
  }

  line_offset[0] = line_offset[0] == 0 ? 1 : 0;
  for (size_t i = 0; i < line_offset.size(); ++i) {
    if (line_offset[i] == 0) {
      line_offset[i] = line_offset[i - 1] + 2;
    } else {
      line_offset[i] = line_offset[i - 1] + 1;
    }
  }

  column_offset[0] = column_offset[0] == 0 ? 1 : 0;
  for (size_t i = 0; i < column_offset.size(); ++i) {
    if (column_offset[i] == 0) {
      column_offset[i] = column_offset[i - 1] + 2;
    } else {
      column_offset[i] = column_offset[i - 1] + 1;
    }
  }

  size_t distances = 0;
  for (size_t i = 0; i < galaxies.size(); ++i) {
    for (size_t j = i + 1; j < galaxies.size(); ++j) {
      auto [x1, y1] = galaxies[i];
      auto [x2, y2] = galaxies[j];
      x1 = line_offset[x1];
      x2 = line_offset[x2];
      y1 = column_offset[y1];
      y2 = column_offset[y2];
      auto d = abs(int64_t(x1 - x2)) + abs(int64_t(y1 - y2));
      distances += d;

      logln("Distance between (", x1, ", ", y1, ") and (", x2, ", ", y2, ") is ", d);
      logln("Total distance is ", distances);
    }
  }
  std::cout << "Total distance is " << distances << std::endl;
}
