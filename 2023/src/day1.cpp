#include "utils.hpp"
#include <iostream>
#include <cstdint>
#include <string>

DPSG_AOC_MAIN(file) {
  uint64_t sum = 0;
  std::string line;

  while (std::getline(file, line)) {
    char first = line[line.find_first_of(dpsg::NUMBERS)];
    char last = line[line.find_last_of(dpsg::NUMBERS)];
    auto current = (first - '0') * 10 + (last - '0');
    sum += current;
  }
  std::cout << sum << std::endl;
}
