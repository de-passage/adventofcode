#include <cmath>
#include "utils.hpp"

constexpr i64 count_digits(i64 n) {
  i64 r = 0;
  while (n) { r++; n /= 10; }
  return r;
}
static_assert(count_digits(1000) == 4);

inline bool odd(auto n) { return n % 2 != 0; }

DPSG_AOC_MAIN(file) {
  std::string line;

  u64 sum = 0;
  while (std::getline(file, line)) {
    u64 cursor = 0;
    while (true) {
      auto start_str = dpsg::next_number(line, cursor);
      if (!start_str) { break; }
      cursor = start_str->position.end;
      i64 start = start_str->value;
      auto end_str = dpsg::next_number(line, cursor);
      if (!end_str) { break; }
      cursor = end_str->position.end;
      i64 end = end_str->value;
  
      if (start == 0) { start = 10; } // no need to consider numbers 0-9

      i64 sz = count_digits(start);
      i64 ez = count_digits(end);

      dpsg::logln("Start: ", start, "\tEnd: ", end);
      dpsg::logln("\e[2mNumber of digits for start: ", sz, "\tfor end: ", ez);

      if (odd(sz)) { start = pow(10, sz); sz++; }
      if (odd(ez)) { end = pow(10, ez - 1) - 1; ez--; }

      dpsg::logln("Adjusted Start: ", start, "\tEnd: ", end);
      dpsg::logln("Adjusted Number of digits for start: ", sz, "\tfor end: ", ez);

      i64 r_start = start / pow(10, sz / 2);
      i64 r_end = end / pow(10, ez / 2);

      dpsg::logln("Start: ", r_start, "\tEnd: ", r_end, "\e[m");

      bool printed = false;
      for (i64 s = r_start; s <= r_end; s++) {
        i64 rz = count_digits(s);
        i64 r = s * pow(10, rz) + s;

        if (r >= start && r <= end) {
          if (printed) { dpsg::log(","); }
          else { printed = true; }
          dpsg::log("\e[1;33m", r, "\e[m");
          sum += r;
        }
      }
      dpsg::logln();
    }
  }
  std::cout << "Sum: " << sum << std::endl;
}