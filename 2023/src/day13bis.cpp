#include "utils.hpp"
#include "symetry.hpp"
#include "day13.hpp"

#include <optional>
#include <type_traits>
#include <iomanip>

using namespace std;
using namespace dpsg;

DPSG_AOC_MAIN(file) {

  string line;
  std::vector<uint32_t> lines;
  std::vector<uint32_t> cols;

  clog << setbase(16);
  size_t acc = 0;
  parse_grid(get_line(file),
             with_number_vectors<size_t>([&](const auto &lines, const auto& cols) {
               auto syml = lowest_symetric_index(lines);
               logln("symetry  (lines): ", syml);
               auto symc = lowest_symetric_index(cols);
               logln("symetry   (cols): ", symc);

               auto sml = lowest_index_with_smudge(lines, syml);
               logln("potential smudges (lines): ", sml);
               auto smc = lowest_index_with_smudge(cols, symc);
               logln("potential smudges (cols): ", smc);

               acc += sml * 100 + smc;
             }));
  std::cout << "Total: " << acc << std::endl;
}

