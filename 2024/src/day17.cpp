#include "utils.hpp"
#include <cassert>
#include "print.hpp"

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

  const auto deref = [&](auto &x) { return combo(x, a, b, c); };

  for (auto n : dpsg::numbers(line)) {
    instructions.push_back(n);
  }

  std::vector<size_t> output;
  size_t ip = 0;
  while (ip < instructions.size() - 1) {
    auto code = instructions[ip];
    auto arg = instructions[ip + 1];
    dpsg::println("Register A: {}", a);
    dpsg::println("Register B: {}", b);
    dpsg::println("Register C: {}", c);
    dpsg::println("opcode: {}, arg: {}", code, arg);
    for (int x = 0; x < instructions.size(); x++) {
      if (x == ip) {
        dpsg::print("\033[1;32m{}\033[0m", instructions[x]);
      } else if (x == ip + 1) {
        dpsg::print("\033[1;36m{}\033[0m", instructions[x]);
      } else {
        dpsg::print("{}", instructions[x]);
      }
      if (x != instructions.size() - 1) {
        dpsg::print(",");
      }
    }
    dpsg::println("\n\033[1;32m{0:>{1}}\033[m", "^", (ip + 1) * 2 - 1);

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
    dpsg::println();
  }

  dpsg::println("Register A: {}", a);
  dpsg::println("Register B: {}", b);
  dpsg::println("Register C: {}", c);
  dpsg::print("Output: ");
  for (int x = 0; x < output.size(); x++) {
    if (x == 0) {
      dpsg::print("{}", output[x]);
    } else {
      dpsg::print(",{}", output[x]);
    }
  }
  dpsg::println();
}
