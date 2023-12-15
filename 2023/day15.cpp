#include "utils.hpp"
#include <numeric>

#include "day15.hpp"

using namespace std;
using namespace dpsg;

int main(int argc, const char** argv) {
  auto file = get_input("day15.txt", argc, argv);

  vector<int> data;
  string line;
  u64 acc = 0;
  while (getline(file, line)) {
    auto tokens = tokenizer{line, ','};
    for (auto t : tokens) {
      logln(t, " -> ", dpsg::hash(t));
      acc += dpsg::hash(t);
    }
  }
  std::cout << acc << std::endl;
}
