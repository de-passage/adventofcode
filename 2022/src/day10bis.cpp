#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

void inc_cycle(int& cycle, int registar) {
  int mod40 = (cycle-1) %40;
  if (mod40 == registar-1 || mod40 == registar || mod40 == registar+1) cout << '#';
  else cout << '.';
  if (cycle %40 == 0) cout << endl;
  cycle++;
}

int main(int argc, const char **argv) {
  if (argc <= 1) {
    return 1;
  }
  std::string filename = argv[1];
  ifstream file(filename);
  if (!file) {
    return 1;
  }

  string line;
  int cycle = 1;
  int registar = 1;
  while(getline(file, line)) {
    if (line[0] == 'a') {
      int i = atoi(line.substr(line.find_first_of(' ')+1).c_str());
      inc_cycle(cycle, registar);
      inc_cycle(cycle, registar);
      registar+=i;
    } else {
      inc_cycle(cycle, registar);
    }
  }
}
