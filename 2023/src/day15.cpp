#include "utils.hpp"
#include <numeric>

#include "day15.hpp"

using namespace std;
using namespace dpsg;

DPSG_AOC_MAIN(file) {

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
