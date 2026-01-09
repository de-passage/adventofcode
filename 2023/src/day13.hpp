#ifndef HEADER_GUARD_DAY13_HPP
#define HEADER_GUARD_DAY13_HPP

#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace dpsg {
template <typename Generator, typename Consumer,
          std::enable_if_t<std::is_same_v<std::invoke_result_t<Generator>,
                                          std::optional<std::string>>,
                           int> = 0,
          std::enable_if_t<std::is_invocable_v<Consumer, const std::string &>,
                           int> = 0>
void parse_grid(Generator &&f, Consumer &&c) {
  std::optional<std::string> s;
  while ((s = f()) != std::nullopt) {
    c(*s);
  }
}

auto get_line(auto &filename) {
  struct {
    auto operator()() {
      std::string line;
      if (getline(file, line)) {
        return std::optional<std::string>{line};
      }
      return std::optional<std::string>{};
    }
    decltype(filename) &file;
  } getline{filename};
  return getline;
}

template <typename T, class F> struct with_number_vectors_t {
  std::vector<T> lines;
  std::vector<T> cols;

  F f;

  with_number_vectors_t(F &&f) : f(std::move(f)) {}

  void operator()(const std::string &line) {
    if (line.empty()) {
      // do work here
      // and ignore the building of the vectors
      logln("lines: ", lines);
      logln("cols: ", cols);

      f(lines, cols);

      // reset the vectors
      lines.clear();
      cols.clear();
      return;
    };

    uint32_t line_value = 0;
    for (auto i = 0u; i < line.size(); ++i) {
      if (cols.size() <= i) {
        cols.push_back(0);
      }

      line_value <<= 1;
      if (line[i] == '#') {
        line_value |= 1;
        cols[i] |= (1 << (lines.size()));
      }
    }
    lines.push_back(line_value);
  }
};
template <
    class T, class F,
    std::enable_if_t<std::conjunction_v<std::is_invocable<
                         F, const std::vector<T> &, const std::vector<T> &>>,
                     int> = 0>
with_number_vectors_t<T, F> with_number_vectors(F &&f) {
  return with_number_vectors_t<T, F>(std::move(f));
}

} // namespace dpsg

#endif // HEADER_GUARD_DAY13_HPP
