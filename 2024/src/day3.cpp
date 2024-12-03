#include "utils.hpp"
#include <print>

enum state { IDENT, NUMBER_FST, NUMBER_SND, };

DPSG_AOC_MAIN(file) {
  std::string input = "";
  for (auto line : dpsg::lines(file)) {
    input += line;
  }

  state expect = IDENT;
  char current_char = 0;
  int current_number = 0;

  int last_number = 0;
  int sum = 0;

  for (auto c : input) {
    switch (expect) {
    case IDENT:
      if ((current_char == 0 && c == 'm') || (current_char == 'm' && 'u') ||
          (current_char == 'u' && c == 'l')) {
        expect = IDENT;
        current_char = c;
      } else if (current_char == 'l' && c == '(') {
        expect = NUMBER_FST;
        current_number = 0;
        current_char = 0;
      } else {
        current_char = 0;
      }
      break;
    case NUMBER_FST:
      if (c >= '0' && c <= '9') {
        current_number = current_number * 10 + (c - '0');
      } else if (c == ',') {
        last_number = current_number;
        current_number = 0;
        expect = NUMBER_SND;
      } else {
        expect = IDENT;
      }
      break;
    case NUMBER_SND:
      if (c >= '0' && c <= '9') {
        current_number = current_number * 10 + (c - '0');
      } else if (c == ')') {
        sum += (last_number * current_number);
        last_number = 0;
        current_number = 0;
        expect = IDENT;
      } else {
        expect = IDENT;
      }
    }
  }
  std::println("{}", sum);
}
