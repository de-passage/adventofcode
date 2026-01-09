#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>

using namespace std;

const auto NUMBERS = "0123456789";

int v(char c) {
  return c - '0';
}

int main() {

  fstream file("day1.txt", ios::in);
  uint64_t sum = 0;
  string line;
  while (getline(file, line)) {
    char first = line[line.find_first_of(NUMBERS)];
    char last = line[line.find_last_of(NUMBERS)];
    auto current = v(first) * 10 + v(last);
    sum += current;
  }
  cout << sum << endl;
}
