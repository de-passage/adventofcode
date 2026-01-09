#include "utils.hpp"
#include "print.hpp"

enum state { IDENT, NUMBER_FST, NUMBER_SND, MUL, DO, DONT };

std::string_view to_string(state st) {
  switch(st) {
  case IDENT: return "IDENT";
  case NUMBER_FST: return "NUMBER_FST";
  case NUMBER_SND: return "NUMBER_SND";
  case MUL: return "MUL";
  case DO: return "DO";
  case DONT: return "DONT";
  }
  return "<invalid state>";
}

DPSG_AOC_MAIN(file) {
  std::string input = "";
  for (auto line : dpsg::lines(file)) {
    input += line;
  }

  state expect = IDENT;
  char current_char = 0;
  int current_number = 0;
  bool enabled = true;

  int last_number = 0;
  int sum = 0;

  for (auto c : input) {
    dpsg::logln("char: ", c, " - state: ", to_string(expect));
    switch (expect) {
    case IDENT:
      if (c == 'm') {
        current_char = 'm';
        expect = MUL;
      } else if (c == 'd') {
        current_char = 'd';
        expect = DO;
      }
      break;
    case MUL:
      if ((current_char == 'm' && c == 'u') ||
          (current_char == 'u' && c == 'l')) {
        current_char = c;
      } else if (current_char == 'l' && c == '(') {
        expect = NUMBER_FST;
        current_number = 0;
        current_char = 0;
      } else {
        expect = IDENT;
        current_char = 0;
      }
      break;
    case DO:
      if (current_char == 'd' && c == 'o') {
        current_char = 'o';
        expect = DO;
      } else if (current_char == 'o') {
        if (c == '(') {
          current_char = '(';
        } else if (c == 'n') {
          current_char = 'n';
          expect = DONT;
        } else {
          current_char = 0;
          expect = IDENT;
        }
      } else if (current_char == '(' && c == ')') {
        enabled = true;
        expect = IDENT;
      } else {
        expect = IDENT;
        current_char = 0;
      }
      break;
    case DONT:
      if ((current_char == 'n' && c == '\'') ||
          (current_char == '\'' && c == 't') ||
          (current_char == 't' && c == '(')) {
        current_char = c;
      } else if (current_char == '(' && c == ')') {
        enabled = false;
        current_char = 0;
        expect = IDENT;
      } else {
        expect = IDENT;
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
      } else if (c == ')' && enabled) {
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
