#include <iostream>
#include <fstream>
#include "utils.hpp"

using namespace std;

DPSG_AOC_MAIN(file) {
  char vs[4];
  for (int i = 0; i < 3; ++i) {
    file >> vs[i];
  }
  char c;
  int x = 3;
  while(file >> vs[3]) {
    x++;
    uint32_t acc = 1 << (vs[0] - 'a');
    for (int i = 0; i < 3; ++i) {
      vs[i]=vs[i+1];
      acc |= 1 << (vs[i] - 'a');
    }
    int bits = 0;
    while (acc != 0) {
      if (acc & 1U) bits++;
      acc >>= 1U;
    }
    if (bits == 4) {
      cout << x << endl;
      break;
    }
  }
}
