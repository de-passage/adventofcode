#include <iostream>
#include <fstream>
#include "utils.hpp"

using namespace std;

DPSG_AOC_MAIN(file) {
  constexpr int REQUESTED_SIZE = 14;
  char vs[REQUESTED_SIZE];
  for (int i = 0; i < REQUESTED_SIZE - 1; ++i) {
    file >> vs[i];
  }
  char c;
  int x = REQUESTED_SIZE;
  while(file >> vs[REQUESTED_SIZE - 1]) {
    uint32_t acc = 1 << (vs[0] - 'a');
    for (int i = 0; i < REQUESTED_SIZE - 1; ++i) {
      vs[i]=vs[i+1];
      acc |= 1 << (vs[i] - 'a');
    }
    int bits = 0;
    while (acc != 0) {
      if (acc & 1U) bits++;
      acc >>= 1U;
    }
    if (bits == REQUESTED_SIZE) {
      cout << x << endl;
      break;
    }
    x++;
  }
}
