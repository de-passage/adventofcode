#include "algorithms.hpp"
#include "utils.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

template <size_t S> struct bit_array {
  uint8_t values[(S + 7) / 8];

  inline constexpr uint8_t operator[](size_t i) const noexcept {
    return (values[i / 8] >> (i % 8)) & 1;
  }

  struct proxy {
    uint8_t *const dest;
    const uint8_t offset;
    const uint8_t mask = 1 << offset;
    constexpr inline const proxy &operator=(uint8_t v) const noexcept {
     // std::cout << std::hex << "ptr("<< (size_t)dest <<") dest: " << int(*dest) << "\tmask: " << (int)mask << "\t~mask: " << (int)(~mask) << "\t(dest&~mask): " << (int)(*dest&~mask) << "\tv: " << (int)v << "\toffset: " << (int)offset << "\tv<<offset: " << int(v<<offset) << " -> ";
      *dest = (*dest & ~mask) | (v << offset);
      // std::cout << int(*dest) << std::endl;
      return *this;
    }
    constexpr inline void set() const noexcept { *dest |= mask; }
    constexpr inline void unset() const noexcept { *dest &= ~mask; }
    constexpr inline void toggle() const noexcept { *dest ^= ~mask; }
    constexpr operator uint8_t() const noexcept { return (*dest & mask) >> offset; }
  };
  inline constexpr proxy operator[](size_t i) noexcept {
    return {
        .dest = values + (i / 8),
        .offset = uint8_t(i % 8),
    };
  }

  constexpr inline size_t size() const noexcept { return S; }
};

template<typename ...Values, std::enable_if_t<std::conjunction_v<std::is_convertible<Values, uint8_t>...>, int> = 0>
constexpr inline bit_array<sizeof...(Values)*8> make_bit_array(Values... vs) noexcept {
  return bit_array<sizeof...(Values)*8>{
    .values = {uint8_t(vs)...},
  };
}

constexpr bit_array<0> a0{};
static_assert(sizeof(a0) == 0);
constexpr bit_array<1> a1{};
static_assert(sizeof(a1) == 1);
constexpr bit_array<8> a8{};
static_assert(sizeof(a8) == 1);
constexpr bit_array<9> a9{};
static_assert(sizeof(a9) == 2);
static_assert(sizeof(make_bit_array(0, 0)) == 2);
static_assert(make_bit_array(0,0xFF)[0] == 0);
static_assert(make_bit_array(0,0xFF)[1] == 0);
static_assert(make_bit_array(0,0xFF)[2] == 0);
static_assert(make_bit_array(0,0xFF)[3] == 0);
static_assert(make_bit_array(0,0xFF)[4] == 0);
static_assert(make_bit_array(0,0xFF)[5] == 0);
static_assert(make_bit_array(0,0xFF)[6] == 0);
static_assert(make_bit_array(0,0xFF)[7] == 0);
static_assert(make_bit_array(0,0xFF)[8] == 1);
static_assert(make_bit_array(0,0xFF)[9] == 1);
static_assert(make_bit_array(0,0xFF)[10] == 1);
static_assert(make_bit_array(0,0xFF)[11] == 1);
static_assert(make_bit_array(0,0xFF)[12] == 1);
static_assert(make_bit_array(0,0xFF)[13] == 1);
static_assert(make_bit_array(0,0xFF)[14] == 1);
static_assert(make_bit_array(0,0xFF)[15] == 1);

int main(int argc, const char **argv) {
  using namespace std;
  auto file = get_input("day8.txt", argc, argv);

  string instruction_str;
  instruction_str.reserve(300);
  getline(file, instruction_str); // directions
  bit_array<300> instructions;
  size_t instruction_size = instruction_str.size();
  for (size_t i = 0; i < instruction_size; ++i) {
    //logln("assigning ", (instruction_str[i] == 'R'), " at ", i, " current value is ", (int)instructions[i]);
    instructions[i] = (instruction_str[i] == 'R');
  }

  //logln(instruction_str, '(', instruction_size, ')');
  for (size_t i = 0; i < instruction_size; ++i) {
    log((instructions[i] == 1) ? 'R' : 'L');
  }
  logln();

  using directions = node[2];
  array<directions, 26*26*26> rules = {};

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

  size_t steps = 0;
  const int target = nodes.size();
  for (;;) {
    int reached = 0;
    size_t idx = steps % instruction_size;
    size_t instr = instructions[idx];
    logln("At step ", steps, " go ", [instr](){return instr ? 'R' : 'L';});
    for (auto& n : nodes) {
      if (end_with_z(n)) {
        ++reached;
      }
      log("From ", [n]() { return to_string(n); }, " go to ");
      n = rules[n][instr];
      if (n == 0) {
        std::cerr << "fetched invalid rule at step " << steps << std::endl;
        exit(1);
      }
      logln([n]() { return to_string(n); });
    }
    if (reached == target) break;
    ++steps;
    if (steps == 0) {
      cerr << "overflowed" << endl;
      exit(1);
    }
    if (steps % 100000000 == 0) {
      cout << steps << endl;
    }
  }
  std::cout << "steps: " << steps << endl;
}
