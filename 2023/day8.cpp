#include "utils.hpp"
#include <string>
#include <unordered_map>

int main(int argc, const char **argv) {
  using namespace dpsg;
  using namespace std;
  auto file = get_input("day8.txt", argc, argv);

  string instructions;
  getline(file, instructions); // directions

  struct directions {
    string left;
    string right;
  };
  unordered_map<string, directions> rules;

  string line;
  while (getline(file, line)) {
    if (line.empty())
      continue;

    auto source = next_word(line);
    if (!source) {
      std::cerr << "error parsing line: " << line << endl;
      break;
    }
    auto left = next_word(line, source->position.end);
    auto right = next_word(line, left->position.end);
    logln("source: ", source->value, " left: ", left->value, " right: ", right->value);
    rules[string(source->value)] = directions{string(left->value), string(right->value)};
  }

  string current = "AAA";
  size_t pos = 0;
  size_t steps = 0;
  while (current != "ZZZ") {
    logln("current: ", current);
    auto rule = rules[current];
    steps++;
    logln("rule: (", rule.left, ",", rule.right, ")");
    char c = instructions[pos];
    pos++;
    pos %= instructions.size();
    if (c == 'L') {
      current = rule.left;
    } else if (c == 'R') {
      current = rule.right;
    } else {
      std::cerr << "error parsing instruction: " << c << endl;
      break;
    }
  }
  std::cout << "steps: " << steps << endl;
}
