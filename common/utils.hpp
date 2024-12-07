#ifndef HEADER_GUARD_UTILS_HPP
#define HEADER_GUARD_UTILS_HPP

#include "range.hpp"
#include "vt100.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

namespace dpsg {

template <class T, class U>
concept same_as = std::is_same_v<T, U> && std::is_same_v<U, T>;

template <class It>
concept input_iterator = requires(It it) {
  { *it } -> same_as<typename std::iterator_traits<It>::reference>;
  { ++it } -> same_as<It &>;
  { it++ } -> same_as<It>;
};

template <class T>
concept Iterable = requires(T t) {
  { std::begin(t) } -> input_iterator;
  { std::end(t) } -> input_iterator;
};

template <class T> struct ranged {
  T value;
  range position;
};
template <class T>
std::ostream &operator<<(std::ostream &os, const ranged<T> &r);

range find_range(std::string_view str, const char *pattern, size_t pos);

std::optional<ranged<std::string_view>> next_word(std::string_view str,
                                                  size_t pos = 0);
std::optional<ranged<size_t>> next_number(std::string_view str, size_t pos = 0);
std::optional<ranged<std::string_view>>
next_alnum_sequence(std::string_view str, size_t pos = 0);
std::optional<ranged<std::string_view>> next_symbol(std::string_view str,
                                                    size_t pos = 0);

bool isValid(range r);

enum class log_level { none, fatal, error, warning, info, debug, all };
log_level get_log_level();
void set_log_level(int level);
bool is_log_level(log_level level);
template <class T, class U>
std::ostream &log_(std::ostream &os, const std::pair<T, U> &opt);

template <class T> std::ostream &log_(std::ostream &os, const T &r) {
  return os << r;
}

inline std::ostream &log_(std::ostream &os, const std::string_view &str) {
  return os << str;
}
inline std::ostream &log_(std::ostream &os, const std::string &str) {
  return os << str;
}

template <class F,
          std::enable_if_t<std::is_invocable_v<std::decay_t<F>>, int> = 0>
std::ostream &log_(std::ostream &os, F &&f) {
  return os << f();
}

template <class F,
          std::enable_if_t<std::is_invocable_v<std::decay_t<F>, std::ostream &>,
                           int> = 0>
std::ostream &log_(std::ostream &os, F &&f) {
  f(os);
  return os;
}

template <class T, class... Args>
std::ostream &log_(std::ostream &os, const std::vector<T, Args...> &v) {
  os << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i > 0)
      os << ", ";
    else
      os << " ";
    log_(os, v[i]);
  }
  return os << " ]";
}

template <class T, class Traits>
std::ostream &log_(std::ostream &os,
                   const std::basic_string_view<T, Traits> &opt) {
  return os << opt;
}

std::ostream &log_(std::ostream &os, const Iterable auto &opt) {
  os << "{";
  for (auto &&e : opt) {
    log_(os, e);
    os << ", ";
  }
  return os << "}";
}

template <class T, class U>
std::ostream &log_(std::ostream &os, const std::pair<T, U> &opt) {
  os << "<";
  log_(os, opt.first);
  os << ", ";
  log_(os, opt.second);
  return os << ">";
}

template <class... Args>
std::ostream &log_(std::ostream &os, const std::tuple<Args...> &t) {
  os << "{";
  std::apply([&os](auto &&...args) { ((log_(os, args), os << ", "), ...); }, t);
  return os << "}";
}

template <class... Args> void log(Args &&...args) {
  if (is_log_level(log_level::debug))
    return;
  (log_(std::clog, args), ...);
}

template <class... Args> void logln(Args &&...args) {
  if (is_log_level(log_level::debug))
    return;
  (log_(std::clog, args), ...);
  std::clog << std::endl;
}

std::fstream get_input(std::string_view filename, int argc, const char **arg);

template <template <class T, class... R> class C, class T, class U,
          std::enable_if_t<std::is_convertible_v<std::decay_t<U>, T>, int> = 0,
          class... R>
void sorted_insert(C<T, R...> &vec, U &&value) {
  using std::begin;
  using std::end;
  auto it = std::lower_bound(begin(vec), end(vec), value);
  vec.insert(it, value);
}

// replacement for std::erase that does not preserves the relative order of
// the remaining elements. It does this by swapping the element to be erased
// with the last element.
// The iterator value is changed in the process, as the value at the current
// iterator is swapped with the last element. This means you should only process
// the vector sequentially from the beginning to the end, without incrementing
// the iterator when calling erase, or you'll skip past the element that was
// swapped in!!!
//
// Example:
// >	std::vector<int> vec = {1, 2, 3, 4, 5, 6};
// >	auto it = vec.begin();
// >	while (it != vec.end()) {
// >	  if (*it % 2 == 0) {
// >	    unstable_erase(vec, it); // 2 is swapped with 6, then popped, so we
// need to reprocess 6 by not incrementing >	  } else { >	    it++; >
// } >	}
//
template <template <class T, class... R> class C, class T, class It, class... R>
auto unstable_erase(C<T, R...> &vec, It it)
    -> decltype(std::declval<std::enable_if_t<
                    std::disjunction_v<
                        std::is_same<It, typename C<T, R...>::const_iterator>,
                        std::is_same<It, typename C<T, R...>::iterator>>,
                    void>>()) {
  using std::iter_swap;
  using std::end;
  auto last =
      end(vec) -
      1; // there must be at least one element since we have an iterator on one
  iter_swap(it, last);
  vec.pop_back();
}

template <template <class T, class... R> class C, class T, class... R>
auto unstable_erase(C<T, R...> &vec, T value) {
  using std::begin;
  using std::end;
  using std::iter_swap;
  auto it = std::find(begin(vec), end(vec), value);

  if (it != end(vec)) {
    auto last =
        end(vec) - 1; // there must be at least one element since we found one
    iter_swap(it, last);
    vec.pop_back();
  }
}

namespace detail {
struct eof_line_iter {};
struct eol_iter {};
template <class T, class U> struct line_iter {
  std::basic_fstream<T, U> *file;
  std::string current;

  line_iter(std::basic_fstream<T, U> *file) : file(file) {
    next_();
  }

  line_iter &operator++() { next_(); return *this; }

  const std::string& operator*() const {
    return current;
  }

  friend bool operator!=(const line_iter &lhs,
                         [[maybe_unused]] const eof_line_iter &rhs) {
    return !lhs.file->eof();
  }

  void next_() {
    std::getline(*file, current);
  }
};

template<class T> struct integers_iter {
  std::string_view line;
  size_t pos = 0;
  T current = 0;

  integers_iter(std::string_view line, size_t start) : line(line), pos{start} {
    next_();
  }

  integers_iter& operator++() {
    next_();
    return *this;
  }

  T operator*() const {
    return current;
  }

  void next_() {
    auto r = next_number(line, pos);
    if (r) {
      pos = r->position.end;
      current = r->value;
    } else {
      pos = std::string::npos;
    }
  }

  friend bool operator!=(const integers_iter& lhs, [[maybe_unused]] const eol_iter& rhs) {
    return lhs.pos != std::string::npos;
  }
};

} // namespace detail

template <class C, class T> struct lines {
  std::basic_fstream<C, T> &file;

  lines(std::basic_fstream<C, T> &file) : file(file) {}

  auto begin() { return detail::line_iter<C, T>{&file}; }

  auto end() { return detail::eof_line_iter{}; }
};
template <class C, class T, template<class,class>class S> lines(S<C, T> &)
    -> lines<C, T>;

struct numbers {
  std::string_view line;
  size_t start;

  numbers(std::string_view line, size_t start = 0) : line(line), start{start} {}

  auto begin() { return detail::integers_iter<size_t>{line, start}; }

  auto end() { return detail::eol_iter{}; }
};

} // namespace dpsg

#ifdef COMPILE_UTILS
// NOLINTBEGIN(misc-definitions-in-headers)
namespace dpsg {

const auto NUMBERS = "0123456789";
const auto LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const auto SYMBOL_FIRST_CHAR =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const auto SYMBOL_NEXT_CHAR =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";

volatile log_level LOG_LEVEL = log_level::none;
log_level get_log_level() { return LOG_LEVEL; }
void set_log_level(log_level level) { LOG_LEVEL = level; }
bool is_log_level(log_level level) { return LOG_LEVEL < level; }

inline std::fstream get_input(std::string_view filename, int argc,
                              const char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0)
      set_log_level(log_level::debug);
    else
      filename = argv[i];
  }

  std::fstream file(filename.data(), std::ios::in);
  if (!file.is_open()) {
    std::cerr << "Could not open file " << filename << std::endl;
    std::exit(1);
  }
  return file;
}

size_t atoi(std::string_view str, size_t pos, size_t end) {
  size_t result = 0;
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

std::optional<ranged<size_t>> next_number(std::string_view str, size_t pos) {
  auto r = find_range(str, NUMBERS, pos);
  if (isValid(r)) {
    return {{atoi(str, r.begin, r.end), r}};
  }
  return {};
}

std::optional<ranged<std::string_view>> next_word(std::string_view str,
                                                  size_t pos) {
  auto r = find_range(str, LETTERS, pos);
  if (isValid(r)) {
    return {{str.substr(r.begin, r.end - r.begin), r}};
  }
  return {};
}

std::optional<ranged<std::string_view>> next_symbol(std::string_view str,
                                                    size_t pos) {
  auto r = str.find_first_of(SYMBOL_FIRST_CHAR, pos);
  if (r != std::string_view::npos) {
    auto end = str.find_first_not_of(SYMBOL_NEXT_CHAR, r);
    if (end == std::string_view::npos) {
      end = str.size();
    }
    return {{str.substr(r, end - r), {r, end}}};
  }
  return {};
}

std::optional<ranged<std::string_view>>
next_alnum_sequence(std::string_view str, size_t pos) {
  auto r = std::find_if(str.begin() + pos, str.end(),
                        [](char c) { return std::isalnum(c); });
  if (r != str.end()) {
    auto pos = std::distance(str.begin(), r);
    auto end = std::find_if_not(str.begin() + pos, str.end(),
                                [](char c) { return std::isalnum(c); });
    auto d = std::distance(r, end);
    return {{str.substr(pos, d), {size_t(pos), size_t(pos + d)}}};
  }
  return {};
}

template <class T>
std::ostream &operator<<(std::ostream &os, const ranged<T> &r) {
  return os << "{ " << r.value << " at " << r.position << " }";
}
} // namespace dpsg

// NOLINTEND(misc-definitions-in-headers)
#endif

namespace dpsg {

// Builds a new integer by pushing the digits of the left integer to the left of
// the digits of the right integer. e.g. combine_ints(123, 456) == 123456
inline size_t combine_ints(size_t left, size_t right) {
  size_t t = right;
  while (t > 0) {
    left *= 10;
    t /= 10;
  }
  return left + right;
}

namespace detail {
inline std::string prepare_file_name(std::string_view file) {
  auto pos = file.find_last_of('/');
  if (pos != std::string_view::npos) {
    file.remove_prefix(pos + 1);
  }
  if (file.substr(0, 3) != "day") {
    return std::string{file} + ".txt";
  }
  pos = file.find_first_not_of("0123456789", 3);
  return std::string{file.substr(0, pos)} + ".txt";
}
} // namespace detail
} // namespace dpsg

#define DPSG_AOC_MAIN(filevar)                                                 \
  namespace dpsg::detail {                                                     \
  void aoc_main(std::fstream &filevar);                                        \
  }                                                                            \
  int main(int argc, const char **argv) {                                      \
    auto filevar = dpsg::get_input(                                            \
        dpsg::detail::prepare_file_name(__BASE_FILE__), argc, argv);           \
    dpsg::detail::aoc_main(filevar);                                           \
  }                                                                            \
  void dpsg::detail::aoc_main(std::fstream &filevar)

#endif // HEADER_GUARD_UTILS_HPP
