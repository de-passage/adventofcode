#include "utils.hpp"
#include "symetry.hpp"
#include "day13.hpp"

#include <optional>
#include <type_traits>
#include <iomanip>

using namespace std;
using namespace dpsg;

int main(int argc, const char **argv) {

  auto file = get_input("day13.txt", argc, argv);

  string line;
  std::vector<uint32_t> lines;
  std::vector<uint32_t> cols;

  clog << setbase(16);
  size_t acc = 0;
  parse_grid(get_line(file),
             with_number_vectors<size_t>([&](const auto &lines, const auto& cols) {
               auto syml = potential_smudges(lines);
               auto symc = potential_smudges(cols);
               logln("potential smudges (lines): ", syml);
               logln("potential smudges (cols): ", symc);
             }));
  std::cout << "Total: " << acc << std::endl;
}

