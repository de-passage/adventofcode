#include <iostream>
#include <fstream>
#include <cassert>
using namespace std;

int atoi(const char* b, const char* e) {
  int acc = 0;
  for (int i = 0; b + i < e; ++i) {
    char c = *(b+i);
    assert(c <= '9' && c >= '0');
    acc*=10;
    acc+=c-'0';
  }
  return acc;
}

int main(int argc, const char** argv) {
  std::string filename=string(argv[0]) + ".txt";
  if (argc > 1) {
    filename=argv[1];
  }
  ifstream file(filename);
  string line;
  int count = 0;
  while(getline(file, line)) {
    size_t d1 = line.find_first_of('-');
    size_t co = line.find_first_of(',');
    size_t d2 = line.find_first_of('-', co);
    int bounds[4] = {
      atoi(line.data(), line.data() + d1),
      atoi(line.data() + d1 + 1, line.data() + co),
      atoi(line.data() + co + 1, line.data() + d2),
      atoi(line.data() + d2 + 1, line.data() + line.size()),
    };
    if ((bounds[0] <= bounds[2] && bounds[1] >= bounds[3])
      || (bounds[0] >= bounds[2] && bounds[1] <= bounds[3])
      || (bounds[0] <= bounds[2] && bounds[1] >= bounds[2])
      || (bounds[2] <= bounds[0] && bounds[3] >= bounds[0])) {
      ++count;
    }
  }
  cout << count << endl;
}



