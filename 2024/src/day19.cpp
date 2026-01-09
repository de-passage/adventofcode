#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <unordered_set>

using towel_t = std::unordered_set<std::string, transparent_string_hash>;

bool can_form(std::string_view word, const towel_t& towels) {
  for (int i = 0; i <= word.size(); ++i) {
    if (towels.contains(std::string{word.substr(0, i)}) &&
        (i == word.size() || can_form(word.substr(i), towels))) {
      return true;
    }
  }
  return false;
}

DPSG_AOC_MAIN(file) {
  std::string line;
  std::getline(file, line);

  towel_t towels;
  for(auto word: dpsg::words(line)) {
    dpsg::println("Adding word: {}", word);
    towels.insert(std::string{word});
  }

  getline(file, line);
  assert(line.empty());

  int sum = 0;
  while (getline(file, line)) {
    bool ok = can_form(line, towels);
    dpsg::print("{}: {}\n", line, ok);
    sum += ok ? 1 : 0;
  }
  dpsg::println("Sum: {}", sum);
}
