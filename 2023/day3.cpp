#include "utils.hpp"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, const char *argv[]) {
  auto input = get_input("day3.txt", argc, argv);
  string line;
  vector<string> lines;

  uint64_t sum = 0;

  while (getline(input, line)) {
    lines.push_back(line);
  }

  for (size_t i = 0; i < lines.size(); i++) {
    string_view line = lines[i];
    logln(line);

    for (auto number = next_number(line); number.has_value();
         number = next_number(line, number->position.end)) {
      logln(*number);
      if (number->position.begin != 0) {
        if (line[number->position.begin - 1] != '.') {
          goto sum_it;
        }
      }
      if (number->position.end != line.size()) {
        if (line[number->position.end] != '.') {
          goto sum_it;
        }
      }
      if (i > 0) {
        auto min_range = number->position.begin > 0 ? number->position.begin - 1 : 0;
        string_view prev_line = lines[i - 1];
        auto max_range = min(prev_line.size(), number->position.end + 1);
        logln("inspecting previous line between ", range{min_range, max_range});
        for (size_t j = min_range; j < max_range; j++) {
          if (prev_line[j] != '.' && !isdigit(prev_line[j])) {
            goto sum_it;
          }
        }
      }
      if (i < lines.size() - 1) {
        auto min_range = number->position.begin > 0 ? number->position.begin - 1 : 0;
        string_view next_line = lines[i + 1];
        auto max_range = min(next_line.size(), number->position.end + 1);
        logln("inspecting next line between ", range{min_range, max_range});
        for (size_t j = min_range; j < max_range; j++) {
          if (next_line[j] != '.' && !isdigit(next_line[j])) {
            goto sum_it;
          }
        }
      }
      continue;
    sum_it:
      log(sum, '+', number->value, '=');
      sum += number->value;
      logln(sum);
    }
  }
  cout << sum << endl;

  return 0;
}
