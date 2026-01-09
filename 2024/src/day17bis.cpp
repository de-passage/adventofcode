#include "utils.hpp"
#include <cassert>
#include <cstdlib>
#include "print.hpp"
#include <unordered_set>
#include <span>
#include <limits>

#define unwrap(X)                                                              \
  ([](auto &&x) {                                                              \
    assert(x.has_value());                                                     \
    return x.value();                                                          \
  })(X)

auto combo(auto d, auto a, auto b, auto c) {
  assert(d >= 0 && d <= 6);
  if (d == 4) {
    return a;
  } else if (d == 5) {
    return b;
  } else if (d == 6) {
    return c;
  } else {
    return d;
  }
}

std::vector<size_t> simulate(const std::vector<size_t> instructions, size_t a,
                             size_t b, size_t c) {
  const auto deref = [&](auto &x) { return combo(x, a, b, c); };

  std::vector<size_t> output;
  size_t ip = 0;
  while (ip < instructions.size() - 1) {
    auto code = instructions[ip];
    auto arg = instructions[ip + 1];

    switch (code) {
    case 0:
      a /= (1 << deref(arg));
      ip += 2;
      break;
    case 1:
      b ^= arg;
      ip += 2;
      break;
    case 2:
      b = deref(arg) & 0b111;
      ip += 2;
      break;
    case 3:
      ip = a != 0 ? arg : ip + 2;
      break;
    case 4:
      b = b ^ c;
      ip += 2;
      break;
    case 5:
      output.push_back(deref(arg) & 0b111);
      ip += 2;
      break;
    case 6:
      b = a / (1 << deref(arg));
      ip += 2;
      break;
    case 7:
      c = a / (1 << deref(arg));
      ip += 2;
      break;
    default:
      dpsg::println(stderr, "Invalid opcode: {}", code);
      std::exit(1);
    }
  }
  return output;
}

DPSG_AOC_MAIN(file) {
  std::string line;

  getline(file, line);
  auto a = unwrap(next_number(line)).value;

  getline(file, line);
  auto b = unwrap(next_number(line)).value;

  getline(file, line);
  auto c = unwrap(next_number(line)).value;

  getline(file, line);
  getline(file, line);

  std::string::size_type pos = 0;

  std::vector<size_t> instructions;

  for (auto n : dpsg::numbers(line)) {
    instructions.push_back(n);
  }

  int current = 0;
  std::unordered_set<size_t> possibilities;
  for (size_t a = 0; a < 1 << 3; ++a) {
    auto output = simulate(instructions, a, b, c);

    if (output.size() == 1 && output[0] == instructions.back()) {
      dpsg::print("Found a match for a = {}:\n", a);
      possibilities.insert(a);
    }
  }

  for (int i = instructions.size() - 2; i >= 0; --i) {
    auto target = std::span(instructions).subspan(i);
    dpsg::print("Trying to generate: ");
    for (int i = 0; i < target.size(); ++i) {
      if (i != 0)
        dpsg::print(",");
      dpsg::print("{}", target[i]);
    }
    dpsg::println();
    dpsg::println("Possibilities: {}", possibilities.size());

    std::unordered_set<size_t> found;
    for (auto p : possibilities) {
      for (size_t n = 0; n < 8; ++n) {
        size_t a = (p << 3) | n;

        auto output = simulate(instructions, a, b, c);

        if (std::ranges::equal(output, target)) {
          dpsg::print("Found a match for a = {0} ({0:b})\n", a);
          found.insert(a);
        }
      }
    }
    std::fflush(stdout);
    possibilities = std::move(found);
    assert(possibilities.size() > 0);
  }
  size_t min = std::numeric_limits<size_t>::max();
  for (auto p : possibilities) {
    min = std::min(min, p);
  }
  dpsg::print("Minimum value for a: {}\n", min);

}
