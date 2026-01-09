#include "algorithms.hpp"
#include "utils.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

bool end_with_z(const std::string& s) {
  return s.back() == 'Z';
}

DPSG_AOC_MAIN(file) {
  using namespace std;

  string instructions;
  getline(file, instructions); // directions

  struct directions {
    string left;
    string right;
  };
  unordered_map<string, directions> rules;

  vector<string> begins;

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    auto source = next_alnum_sequence(line);
    if (!source) {
      std::cerr << "error parsing line (source): " << line << endl;
      break;
    }

    auto left = next_alnum_sequence(line, source->position.end);
    if (!left) {
      std::cerr << "error parsing line (left): " << line << endl;
      break;
    }
    auto right = next_alnum_sequence(line, left->position.end);
    if (!right) {
      std::cerr << "error parsing line (right): " << line << endl;
      break;
    }
    if (source->value.back() == 'A') {
      begins.emplace_back(source->value);
    }
    logln("source: ", source->value, " left: ", left->value, " right: ", right->value);
    rules[string(source->value)] = directions{string(left->value), string(right->value)};
  }

  size_t pos = 0;
  size_t steps = 0;
  while (!dpsg::all(begins, end_with_z)) {
    logln("Current values: ", begins);
    steps++;
    if (steps == 0) {
      std::cerr << "too many steps, overflowed" << endl;
      break;
    }
    if (steps % 100000000 == 0) {
      std::cout << "step: " << steps << endl;
    }
    char c = instructions[pos];
    logln("instruction: ", c, "(" , pos , ")");

    for (auto& current : begins) {
      logln("current: ", current);
      auto rule = rules[current];
      log("rule: (", rule.left, ",", rule.right, ")");
      if (c == 'L') {
        current = rule.left;
      } else if (c == 'R') {
        current = rule.right;
      } else {
        std::cerr << "error parsing instruction: " << c << endl;
        break;
      }
      logln(" -> ", current);
    }
    pos++;
    pos %= instructions.size();
  }
  std::cout << "steps: " << steps << endl;
}
