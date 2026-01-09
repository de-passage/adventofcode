#include "utils.hpp"
#include <cassert>
#include "print.hpp"
#include <queue>

struct operation {
  long value;
  size_t idx_nb;
};

int digit_count(long t) {
  int count = 1;
  while (t > 0) {
    count *= 10;
    t /= 10;
  }
  return count;
}

std::optional<long> remove_last_digits(long target, long v) {
  int dc = digit_count(v);
  if (target % dc == v) {
    return {target / dc};
  }
  return {};
}


bool try_possibilities (long target, std::vector<size_t>& numbers) {
  assert(numbers.size() > 0);
  operation current = {target, numbers.size() - 1};

  std::queue<operation> operations;
  operations.push(current);
  int sum = 0;

  while (!operations.empty()) {
    current = operations.front();
    operations.pop();

    long t = current.value;
    long n  = numbers[current.idx_nb];

    if (current.idx_nb == 0) {
      if (t == n) {
        return true;
      }
    } else {
      operations.push({t - n, current.idx_nb - 1});
      if (n != 0 && t % n == 0) {
        operations.push({t / n, current.idx_nb - 1});
      }
      if (auto trimmed = remove_last_digits(t, n); trimmed) {
        operations.push({trimmed.value(), current.idx_nb - 1});
      }
    }
  }
  return false;
}

DPSG_AOC_MAIN(file) {
  size_t sum = 0;

  for (auto line : dpsg::lines(file)) {
    auto targetopt = next_number(line);
    assert(targetopt.has_value());
    auto [target, range] = targetopt.value();
    auto [_, end] = range;

    std::vector<size_t> numbers;
    for (auto n : dpsg::numbers(line, end)) {
      numbers.push_back(n);
    }

    dpsg::logln("Target: ", target);
    dpsg::log("Numbers: ");
    for (auto n : numbers) {
      dpsg::log(n, ", ");
    }
    dpsg::logln();
    bool r = try_possibilities(target, numbers);
    dpsg::logln("Result: ", r);

    auto last = sum;
    if (r) { sum += target;}
    if (sum < last) {
      dpsg::println("Overflow: ", sum, " < ", last);
      break;
    }
  }
  dpsg::println("Sum: {}", sum);
}
