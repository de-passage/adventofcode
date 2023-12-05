#include "utils.hpp"
#include <iostream>
#include <string>
#include <vector>


using namespace std;

int main(int argc, char *argv[]) {
  auto input = get_input("day3.txt", argc, argv);
  string line;
  vector<string> lines;

  while (getline(input, line)) {
    lines.push_back(line);
  }

  return 0;
}
