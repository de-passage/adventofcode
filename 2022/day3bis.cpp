#include <iostream>
#include <fstream>

using namespace std;

uint64_t value(char c) {
  return (c <= 'z' && c>= 'a') ? (c - 'a' + 1) : (c - 'A' + 27);
}

void print(uint64_t v) {
  for (int i = 63; i >= 0; --i) {
    cout << ((v & (1 << i)) ? 1 : 0);
  }
}

int main(int argc, const char** argv) {
  std::string filename="day3.txt";
  if (argc > 1) {
    filename=argv[1];
  }
  ifstream file(filename);
  string line;
  int score=0;
  uint64_t group[3];
  int idx = 0;

  while(getline(file, line)) {
    for (char c : line) {
      group[idx] |= (1LL << value(c));
    }

    if (++idx == 3) {
      uint64_t found = group[0] & group[1] & group[2];
      while (idx--) {
        group[idx] = 0;
      }
      int s = 0;
      while (found >>= 1) {
        s++;
      }
      score+=s;
      idx=0;
    }
  }
  cout << score << endl;
}
