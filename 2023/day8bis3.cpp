#include "algorithms.hpp"
#include "utils.hpp"
#include "bit_array.hpp"

#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <numeric>

using node = uint16_t;

inline constexpr bool end_with_z(node n) noexcept {
  constexpr node Z = 'Z' - 'A';
  return n % 26 == Z;
}

inline constexpr node fromString(std::string_view s) noexcept {
  node r = s[0] - 'A';
  r *= 26;
  int s1 = s[1] - 'A';
  r += s1;
  r *= 26;
  int s2 = s[2] - 'A';
  r += s2;
  return r;
}

inline std::string to_string(node n) {
  std::string s;
  s.resize(3);
  s[0] = (n / (26 * 26)) + 'A';
  s[1] = ((n / 26) % 26) + 'A';
  s[2] = (n % 26) + 'A';
  return s;
}

int main(int argc, const char **argv) {
  using namespace std;
  using namespace dpsg;
  auto file = get_input("day8.txt", argc, argv);

  string instruction_str;
  instruction_str.reserve(300);
  getline(file, instruction_str); // directions
  bit_array<300> instructions;
  size_t instruction_size = instruction_str.size();
  for (size_t i = 0; i < instruction_size; ++i) {
    instructions[i] = (instruction_str[i] == 'R');
  }

  logln([&](ostream&out) {
    for (size_t i = 0; i < instruction_size; ++i) {
      out << ((instructions[i] == 1) ? 'R' : 'L');
    }
    });

  using directions = node[2];
  array<directions, 26 * 26 * 26> rules = {};

  vector<node> nodes;

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
    node source_node = fromString(source->value);
    node left_node = fromString(left->value);
    node right_node = fromString(right->value);
    if (source->value.back() == 'A') {
      nodes.emplace_back(source_node);
    }
    logln("source: ", to_string(source_node), " left: ", to_string(left_node),
          " right: ", to_string(right_node));
    rules[source_node][0] = left_node;
    rules[source_node][1] = right_node;
  }

  log("Starting at: [ ");
  for (size_t i = 0; i < nodes.size(); ++i) {
    if (i > 0) {
      log(", ");
    }
    log(to_string(nodes[i]));
  }
  logln(" ]");

  struct end_point {
    size_t s;
    node n;
  };
  struct data {
    size_t loop_length;
    node loop_node;
    node start_node;
    size_t total_steps;
    size_t loop_node_position;
    std::vector<end_point> end_points;
  };

  const auto log_node = [](auto n) { return [n]() { return to_string(n); }; };

  union visitation {
    struct {
      size_t left;
      size_t right;
    } s;
    size_t a[2];
  };
  array<visitation, 26*26*26> vs;
  array<visitation, 26*26*26> met_at;
  const auto log_status = [](size_t n){
      return [n](ostream& os) { n == size_t(-1) ? os << "not visited" : os << n; };
  };

  std::vector<data> data_store;
  for (auto start : nodes) {
    auto &data = data_store.emplace_back();
    data.start_node = start;
    memset(&vs, -1, sizeof(vs));
    memset(&met_at, -1, sizeof(met_at));
    size_t idx = 0;
    auto n = start;
    size_t steps = 0;
    for (;;) {
      logln("Visiting: ", log_node(n));
      logln("Visitation status: ", log_status(vs[n].s.left), ", ", log_status(vs[n].s.right));
      int instr = instructions[idx];
      if (vs[n].a[instr] == idx) {
        data.loop_node = n;
        data.total_steps = steps;
        data.loop_node_position = met_at[n].a[instr];
        data.loop_length = data.total_steps - data.loop_node_position;
        cout << to_string(start) << " loops at " << to_string(n) << " after "
             << steps << " steps\n"
             << "distance is " << data.loop_length << endl;
        break;
      }

      if (end_with_z(n)) {
        logln("found something: ", log_node(n));
        data.end_points.push_back({.s = steps, .n = n});
      }

      logln("Going ", [instr]() { return instr ? "Right" : "Left"; }, " at instruction ", idx);
      vs[n].a[instructions[idx]] = idx;
      met_at[n].a[instructions[idx]] = steps;
      idx++;
      idx %= instruction_size;
      logln([&](ostream& out) {
          out << " -> " << to_string(rules[n][0]) << " | " << to_string(rules[n][1]) << std::endl;
          });
      n = rules[n][instr];
      logln("Updating to: ", log_node(n));
      steps++;
    }
  }

  for (auto &p : data_store) {
    std::cout << "start: " << to_string(p.start_node) << " , loop: " << to_string(p.loop_node) << " , steps: " << p.total_steps << " , loop length: " << p.loop_length << " , loop start: " << p.loop_node_position <<  " , ends: ";
    for (auto &p : p.end_points) {
      std::cout << "{" << to_string(p.n) << "," << p.s << "}, ";
    }
    std::cout << std::endl;
  }

  auto l = data_store[0].loop_length;
  auto g = data_store[0].loop_length;
  for (size_t i = 1; i < data_store.size(); ++i) {
    auto &p = data_store[i];
    l = std::lcm(l, p.loop_length);
    g = std::gcd(g, p.loop_length);
  }
  cout << "lcm: " << l << "gcm: " << g << endl;
}
