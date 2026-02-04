#include "utils.hpp"
#include "print.hpp"
#include <cassert>

DPSG_AOC_MAIN(file) {
    int current_value = 50;
    int zeroes = 0;
    std::string line;

    while (getline(file, line)) {
        int sign = line[0] == 'L' ? -1 : 1;
        auto value = static_cast<int>(unwrap(dpsg::next_number(line)).value);
        int offset = sign * value;
        int overflow_count = abs(offset / 100);
        offset %= 100;

        int add_to_overflow = current_value == 0 ? 0 : 1;
        current_value += offset;
        if (current_value >= 100) {
            current_value -= 100;
            overflow_count++;
        } else if (current_value < 0) {
            current_value += 100;
            overflow_count += add_to_overflow;
        }
        else if (current_value == 0) {
            zeroes++;
        }

        zeroes += overflow_count;

        dpsg::logln(
              "\033[33;1mline:\033[m ", line,
            ", \033[33;1moffset:\033[m ", offset,
            ", \033[33;1mcurrent_value:\033[m ", current_value,
            ", \033[33;1mzeroes:\033[m ", zeroes,
            ", \033[33;1moverflow_count:\033[m ", overflow_count);
    }
    dpsg::println("Number of zeroes: {}", zeroes);
}