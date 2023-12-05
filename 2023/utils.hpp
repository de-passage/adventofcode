#ifndef HEADER_GUARD_UTILS_HPP
#define HEADER_GUARD_UTILS_HPP

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

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
#endif

#endif // HEADER_GUARD_UTILS_HPP
