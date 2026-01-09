#include "utils.hpp"
#include "day12.hpp"

using namespace std;
using namespace dpsg;

int main(int argc, const char **argv) {
  auto file = get_input("day12.txt", argc, argv);

  string line;
  size_t s = 0;
  while (getline(file, line)) {
    auto [states, groups] = process_input_line(line);
    s += count_possibilities(states, groups);
  }
  std::cout << s << std::endl;
}
