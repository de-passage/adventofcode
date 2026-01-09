#include "utils.hpp"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

DPSG_AOC_MAIN(file) {
  using namespace dpsg;
  auto &input = file;
  string line;
  vector<string> lines;

  while (getline(input, line)) {
    lines.push_back(line);
  }

  unordered_map<range, vector<int>> gears{};

  for (size_t i = 0; i < lines.size(); i++) {
    string_view line = lines[i];
    logln(line);

    for (auto number = next_number(line); number.has_value();
         number = next_number(line, number->position.end)) {
      logln(*number);
      if (number->position.begin != 0) {
        if (line[number->position.begin - 1] == '*') {
          gears[{i, number->position.begin-1}].push_back(number->value);
        }
      }
      if (number->position.end != line.size()) {
        if (line[number->position.end] == '*') {
          gears[{i, number->position.end}].push_back(number->value);
        }
      }
      if (i > 0) {
        auto min_range = number->position.begin > 0 ? number->position.begin - 1 : 0;
        string_view prev_line = lines[i - 1];
        auto max_range = min(prev_line.size(), number->position.end + 1);
        logln("inspecting previous line between ", range{min_range, max_range});
        for (size_t j = min_range; j < max_range; j++) {
          if (prev_line[j] == '*') {
            gears[{i - 1,j}].push_back(number->value);
          }
        }
      }
      if (i < lines.size() - 1) {
        auto min_range = number->position.begin > 0 ? number->position.begin - 1 : 0;
        string_view next_line = lines[i + 1];
        auto max_range = min(next_line.size(), number->position.end + 1);
        logln("inspecting next line between ", range{min_range, max_range});
        for (size_t j = min_range; j < max_range; j++) {
          if (next_line[j] == '*') {
            gears[{i + 1,j}].push_back(number->value);
          }
        }
      }
    }
  }


  uint64_t sum = 0;
  for (const auto& gear : gears) {
    log(gear.first, " -> ");
    for(int v : gear.second) {
      log(v, ' ');
    }
    if (gear.second.size() == 2) {
      log("summing!");
      sum += gear.second[0] * gear.second[1];
    }
    logln();
  }
  cout << sum << endl;

  return 0;
}
