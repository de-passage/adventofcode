#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <cstring>

using namespace std;

const auto NUMBERS = array<const char *, 10>{
    "zero", "one", "two",   "three", "four",
    "five", "six", "seven", "eight", "nine",
};

int v(char c) { return c - '0'; }

auto toDigit(string_view s) {
  if (s.size() == 1) {
    return v(s[0]);
  }
  for (int i = 0; i < 10; i++) {
    if (s == NUMBERS[i]) {
      return i;
    }
  }
  return -1;
}

int LOG_LEVEL = 0;

template <class... Args> void log(Args &&...args) {
  if (LOG_LEVEL < 1)
    return;
  (cout << ... << args);
}

template <class... Args> void logln(Args &&...args) {
  if (LOG_LEVEL < 1)
    return;
  (cout << ... << args) << endl;
}

int main(int argc, char **arg) {
  string filename = "day1.txt";

  for (int i = 1; i < argc; i++) {
    if (strcmp(arg[i], "-d") == 0)
      LOG_LEVEL = 1;
    else
      filename = arg[i];
  }

  fstream file(filename, ios::in);
  if (!file.is_open()) {
    cerr << "Could not open file " << filename << endl;
    return 1;
  }
  uint64_t sum = 0;
  string line;
  regex r(
      "zero|one|two|three|four|five|six|seven|eight|nine|1|2|3|4|5|6|7|8|9");
  while (getline(file, line)) {
    smatch m;
    log(line, "\t");
    if (!regex_search(line, m, r)) {
      logln();
      continue;
    }
    auto current = toDigit(m[0].str());
    log(m[0].str(), '(', current, ")\t");
    line = string(m[0].first + 1, line.cend());

    int last = current;
    while (regex_search(line, m, r)) {
      last = toDigit(m[0].str());
      log(m[0].str(), '(', last, ")\t");
      line = string(m[0].first + 1, line.cend());
    }
    current = current * 10 + last;
    log("current:", current, "\t", sum, '+', current, "=");
    sum += current;
    logln(sum);
  }
  cout << sum << endl;
}
