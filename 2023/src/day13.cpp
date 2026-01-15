#include "utils.hpp"
#include "symetry.hpp"
#include "day13.hpp"

#include <optional>
#include <type_traits>

using namespace std;
using namespace dpsg;

DPSG_AOC_MAIN(file) {

  string line;
  std::vector<uint32_t> lines;
  std::vector<uint32_t> cols;

  size_t acc = 0;
  parse_grid(get_line(file),
             with_number_vectors<size_t>([&](const auto &lines, const auto& cols) {
               auto syml = lowest_symetric_index(lines);
               auto symc = lowest_symetric_index(cols);
               logln("symetry on lines: ", syml);
               logln("symetry on cols: ", symc);
               acc += (syml * 100) + symc;
             }));
  std::cout << "Total: " << acc << std::endl;
}

