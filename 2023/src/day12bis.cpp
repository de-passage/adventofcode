#include "utils.hpp"
#include "day12.hpp"

using namespace std;
using namespace dpsg;

DPSG_AOC_MAIN(file) {

  string line;
  size_t s = 0;
  int c = 0;
  while (getline(file, line)) {
    log('(', c++, ") Possibilities for \"", line,"\":\t\t ");
    auto [states, groups] = process_input_line(line);
    std::vector<State> many_states;
    std::vector<int> many_groups;
    for (int i = 0; i < 5; ++i) {
      if (i > 0) {
        many_states.push_back(State::Unknown);
      }
      many_states.insert(many_states.end(), states.begin(), states.end());
      many_groups.insert(many_groups.end(), groups.begin(), groups.end());
    }
    auto p = count_possibilities(many_states, many_groups);
    s+= p;
    logln(p, "\t\t(", s, " total)");

  }
  std::cout << s << std::endl;
}
