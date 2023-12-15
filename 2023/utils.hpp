#ifndef HEADER_GUARD_UTILS_HPP
#define HEADER_GUARD_UTILS_HPP

#include "range.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace dpsg {

template<class T, class U>
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

int get_log_level();
void set_log_level(int level);
template<class T, class U>
std::ostream& log_(std::ostream& os, const std::pair<T,U>&  opt);

template <class T> std::ostream &log_(std::ostream &os, const T &r) {
  return os << r;
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

template<class T, class Traits>
std::ostream& log_(std::ostream& os, const std::basic_string_view<T, Traits>&  opt) {
  return os << opt;
}

std::ostream& log_(std::ostream& os, const Iterable auto&  opt) {
  os << "{";
  for (auto&& e : opt) {
    log_(os, e);
    os << ", ";
  }
  return os << "}";
}

template<class T, class U>
std::ostream& log_(std::ostream& os, const std::pair<T,U>&  opt) {
  os << "<"; log_(os, opt.first); os << ", "; log_(os, opt.second);
  return os << ">";
}

namespace vt100 {
  enum class color: uint8_t {
    black = 0,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
  };


  template<size_t S>
    struct termcode_sequence {
      uint8_t codes[S];
    };

  using termcode = termcode_sequence<1>;


  static constexpr termcode reset{0};

  inline constexpr termcode bg(color c) noexcept {
    return termcode{static_cast<uint8_t>(static_cast<uint8_t>(c) + 40)};
  }
  inline constexpr termcode fg(color c) noexcept {
    return termcode{static_cast<uint8_t>(static_cast<uint8_t>(c) + 30)};
  }

  static constexpr inline auto green = fg(color::green);
  static constexpr inline auto red = fg(color::red);
  static constexpr inline auto black = fg(color::black);
  static constexpr inline auto blue = fg(color::blue);
  static constexpr inline auto cyan = fg(color::cyan);
  static constexpr inline auto magenta = fg(color::magenta);
  static constexpr inline auto yellow = fg(color::yellow);
  static constexpr inline auto white = fg(color::white);
  static constexpr inline termcode bold{1};
  static constexpr inline termcode faint{2};
  static constexpr inline termcode italic{3};
  static constexpr inline termcode underline{4};
  static constexpr inline termcode blink{5};
  static constexpr inline termcode reverse{7};
  static constexpr inline termcode conceal{8};
  static constexpr inline termcode crossed{9};

  namespace detail {
    template<size_t ...I1, size_t ...I2>
      constexpr auto concat_impl(termcode_sequence<sizeof...(I1)> t1, termcode_sequence<sizeof...(I2)> t2, std::index_sequence<I1...>, std::index_sequence<I2...>) noexcept {
        return termcode_sequence<sizeof...(I1) + sizeof...(I2)>{
          .codes = {t1.codes[I1]..., t2.codes[I2]...}
        };
      }
  }

  template<size_t S1, size_t S2>
    constexpr auto operator|(termcode_sequence<S1> s1, termcode_sequence<S2> s2) noexcept {
      return detail::concat_impl(s1, s2, std::make_index_sequence<S1>{}, std::make_index_sequence<S2>{});
    }

  template<size_t S, std::enable_if_t<(S > 1), int> = 0>
    std::ostream& operator<<(std::ostream& os, const termcode_sequence<S>& s) {
      os << "\033[" << static_cast<int>(s.codes[0]);
      for (auto i = 1uz; i < S; ++i) {
        os << ';' << static_cast<int>(s.codes[i]);
      }
      return os << "m";
    }

    inline std::ostream& operator<<(std::ostream& os, termcode s) {
      return os << "\033[" << static_cast<int>(s.codes[0]) << "m";
    }

  template<size_t S, typename T>
    struct generic_decorate {
      termcode_sequence<S> codes;
      T value;
    };
  template<size_t S, typename T>
    generic_decorate(termcode_sequence<S>, T) -> generic_decorate<S, T>;

  template<size_t S>
    struct decorate : generic_decorate<S, std::string_view> {};
  template<size_t S>
    decorate(termcode_sequence<S>, std::string_view) -> decorate<S>;

  template<size_t S, typename T>
    std::ostream& operator<<(std::ostream& os, const generic_decorate<S, T>& d) {
      return os << d.codes << d.value << reset;
    }
}


template <class... Args> void log(Args &&...args) {
  if (get_log_level() < 1)
    return;
  (log_(std::clog, args), ...);
}

template <class... Args> void logln(Args &&...args) {
  if (get_log_level() < 1)
    return;
  (log_(std::clog, args), ...);
  std::clog << std::endl;
}

std::fstream get_input(std::string filename, int argc, const char **arg);

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
} // namespace dpsg

#ifdef COMPILE_UTILS
namespace dpsg {

const auto NUMBERS = "0123456789";
const auto LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const auto SYMBOL_FIRST_CHAR =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const auto SYMBOL_NEXT_CHAR =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";

int LOG_LEVEL = 0;
int get_log_level() { return LOG_LEVEL; }
void set_log_level(int level) { LOG_LEVEL = level; }

std::fstream get_input(std::string filename, int argc, const char **argv) {
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
#endif

inline size_t combine_ints(size_t left, size_t right) {
  size_t t = right;
  while (t > 0) {
    left *= 10;
    t /= 10;
  }
  return left + right;
}
}

#endif // HEADER_GUARD_UTILS_HPP
