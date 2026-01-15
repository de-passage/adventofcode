
#include <iostream>
#include <fstream>
#include "utils.hpp"

using namespace std;

uint64_t value(char c) {
  return (c <= 'z' && c>= 'a') ? (c - 'a' + 1) : (c - 'A' + 27);
}

void print(uint64_t v) {
  for (int i = 63; i >= 0; --i) {
    cout << ((v & (1 << i)) ? 1 : 0);
  }
}

DPSG_AOC_MAIN(file) {
  string line;
  int score=0;
  while(getline(file, line)) {
    uint64_t left = 0, right = 0;
    int half = line.size() / 2;
    for (int i = 0; i < half; ++i) {
      left |= (1LL << value(line[i]));
      right |= (1LL << value(line[i + half]));
    }
    uint64_t diff = left & right;
    int idx = 0;

    while (diff >>= 1LL) {
      idx++;
    }
    score += idx;
  }
  cout << score << endl;
}
