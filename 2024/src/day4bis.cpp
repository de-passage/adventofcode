#include "utils.hpp"
#include <cassert>
#include "print.hpp"

bool mors(char l, char r) {
  return (l == 'M' && r == 'S') || (r == 'M' && l == 'S');
}

DPSG_AOC_MAIN(file) {

  std::vector<std::string> lines;

  for (auto line : dpsg::lines(file)) {
    lines.push_back(line);
  }

  int sum = 0;

  for (int i = 1; i < lines.size() - 1; ++i) {
    auto &line = lines[i];
    for (int j = 1; j < line.size() - 1; ++j) {
      if (line[j] == 'A' && mors(lines[i - 1][j - 1], lines[i + 1][j + 1]) &&
          mors(lines[i - 1][j + 1], lines[i + 1][j - 1])) {
        sum += 1;
        std::println("found at {} {}", i, j);
      }
    }
  }
  std::println("{}", sum);
}
