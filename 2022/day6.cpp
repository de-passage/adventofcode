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
  return 0;
}
