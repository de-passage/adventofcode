#ifndef HEADER_GUARD_UTILS_HPP
#define HEADER_GUARD_UTILS_HPP

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <optional>

struct range {
  size_t begin;
  size_t end;
};

template <class T> struct ranged {
  T value;
  range position;
};

range find_range(std::string_view str, const char *pattern, size_t pos);
std::optional<ranged<std::string_view>> next_word(std::string_view str, size_t pos = 0);
std::optional<ranged<int>> next_number(std::string_view str, size_t pos = 0);
bool isValid(range r);

int get_log_level();
void set_log_level(int level);

template <class... Args> void log(Args &&...args) {
  if (get_log_level() < 1)
    return;
  (std::cout << ... << args);
}

template <class... Args> void logln(Args &&...args) {
  if (get_log_level() < 1)
    return;
  (std::cout << ... << args) << std::endl;
}

std::fstream get_input(std::string filename, int argc, char **arg);

#ifdef COMPILE_UTILS

const auto NUMBERS = "0123456789";
const auto LETTERS = "abcdefghijklmnopqrstuvwxyz";

int LOG_LEVEL = 0;
int get_log_level() { return LOG_LEVEL; }
void set_log_level(int level) { LOG_LEVEL = level; }

std::fstream get_input(std::string filename, int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0)
      set_log_level(1);
    else
      filename = argv[i];
  }

  std::fstream file(filename, std::ios::in);
  if (!file.is_open()) {
    std::cerr << "Could not open file " << filename << std::endl;
    std::exit(1);
  }
  return file;
}

int atoi(std::string_view str, size_t pos, size_t end) {
  int result = 0;
  for (size_t i = pos; i < end; ++i) {
    result *= 10;
    result += str[i] - '0';
  }
  return result;
}

range find_range(std::string_view str, const char *pattern, size_t pos) {
  auto begin = str.find_first_of(pattern, pos);
  auto end = std::string_view::npos;
  if (begin != std::string_view::npos) {
    end = str.find_first_not_of(pattern, begin);
    if (end == std::string_view::npos) {
      end = str.size();
    }
  }
  return {begin, end};
}

bool isValid(range r) { return r.begin != std::string_view::npos; }

std::optional<ranged<int>> next_number(std::string_view str, size_t pos) {
  auto r = find_range(str, NUMBERS, pos);
  if (isValid(r)) {
    return {{atoi(str, r.begin, r.end), r}};
  }
  return {};
}

std::optional<ranged<std::string_view>> next_word(std::string_view str, size_t pos) {
  auto r = find_range(str, LETTERS, pos);
  if (isValid(r)) {
    return {{str.substr(r.begin, r.end - r.begin), r}};
  }
  return {};
}
#endif

#endif // HEADER_GUARD_UTILS_HPP
