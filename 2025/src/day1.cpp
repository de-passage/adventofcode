#include "utils.hpp"
#include "print.hpp"
#include <cassert>

DPSG_AOC_MAIN(file) {
    int current_value = 50;
    int zeroes = 0;

    for (const auto& line : dpsg::lines(file)) {
        int sign = line[0] == 'L' ? -1 : 1;
        auto value = static_cast<int>(unwrap(dpsg::next_number(line)).value);
        int offset = sign * value;
        dpsg::logln("Line: '", line, "', sign: ",  sign,", value: ",  value, ", offset: ", offset);
        current_value += offset;
        dpsg::log("Intermediate value: ", current_value);
        current_value %= 100;
        if (current_value < 0) current_value += 100;
        dpsg::logln("\t-> Current value: ", current_value);
        if (current_value == 0) {
            dpsg::logln("Found a zero");
            zeroes++;
        }
    }
    dpsg::println("Number of zeroes: {}", zeroes);
}