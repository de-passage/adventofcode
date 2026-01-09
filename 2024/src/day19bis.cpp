#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <unordered_map>
#include <unordered_set>

using towel_t = std::unordered_set<std::string, transparent_string_hash>;
using cache_t = std::unordered_map<std::string, size_t, transparent_string_hash>;

size_t can_form(std::string_view word, const towel_t &towels, cache_t& cache) {
  auto it = cache.find(std::string{word});
  if (it != cache.end()) {
    return it->second;
  }

  size_t sum = 0;
  for (int i = 0; i <= word.size(); ++i) {
    if (towels.contains(std::string{word.substr(0, i)})) {
      if (i == word.size()) {
        sum += 1;
      } else {
        sum += can_form(word.substr(i), towels, cache);
      }
    }
  }
  std::println("ways to form '{}': {}", word, sum);
  cache[std::string{word}] = sum;
  return sum;
}

DPSG_AOC_MAIN(file) {
  std::string line;
  std::getline(file, line);

  towel_t towels;
  for (auto word : dpsg::words(line)) {
    std::println("Adding word: {}", word);
    towels.insert(std::string{word});
  }

  getline(file, line);
  assert(line.empty());

  size_t sum = 0;
  cache_t cache;
  while (getline(file, line)) {
    size_t ok = can_form(line, towels, cache);
    std::print("{}: {}\n", line, ok);
    sum += ok;
  }
  std::println("Sum: {}", sum);
}
