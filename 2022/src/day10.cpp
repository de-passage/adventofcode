#include <algorithm>
#include <fstream>
#include <iostream>
#include "utils.hpp"

using namespace std;

void inc_cycle(int& cycle, int& sum, int registar) {
  cycle++;
  cerr << "cycle: " << cycle;
  if (cycle == 20 || (cycle-20) % 40 == 0) {
    sum+=(cycle*registar);
    cerr << "\tregister: " << registar << " strength: " << (cycle*registar) << " sum: " << sum;
  }
  cerr << endl;
}

DPSG_AOC_MAIN(file) {
  string line;
  int cycle = 0;
  int registar = 1;
  int sum = 0;
  while(getline(file, line)) {
    if (line[0] == 'a') {
      int i = std::atoi(line.substr(line.find_first_of(' ')+1).c_str());
      cerr << "addx " << i << endl;
      inc_cycle(cycle, sum, registar);
      inc_cycle(cycle, sum, registar);
      registar+=i;
    } else {
      cerr << "noop" << endl;;
      inc_cycle(cycle, sum, registar);
    }
  }
  cout << sum << endl;
}
