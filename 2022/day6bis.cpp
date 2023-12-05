#include <iostream>
#include <fstream>

using namespace std;


int main(int argc, const char** argv) {
  if (argc <= 1) {
    cerr << "Gimme a file pls" << endl;
    return 1;
  }
  std::string filename=argv[1];
  ifstream file(filename);
  if (!file) {
    cerr << "Not a good file this " << filename << endl;
    return 1;
  }

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
  return 0;
}
