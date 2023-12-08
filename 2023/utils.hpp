#ifndef HEADER_GUARD_UTILS_HPP
#define HEADER_GUARD_UTILS_HPP

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include "range.hpp"

template <class T> struct ranged {
  T value;
  range position;
};
template <class T>
std::ostream &operator<<(std::ostream &os, const ranged<T> &r);

range find_range(std::string_view str, const char *pattern, size_t pos);

std::optional<ranged<std::string_view>> next_word(std::string_view str, size_t pos = 0);
std::optional<ranged<size_t>> next_number(std::string_view str, size_t pos = 0);
std::optional<ranged<std::string_view>> next_alnum_sequence(std::string_view str, size_t pos = 0);
std::optional<ranged<std::string_view>> next_symbol(std::string_view str, size_t pos = 0);

bool isValid(range r);

int get_log_level();
void set_log_level(int level);

template <class T> std::ostream &log_(std::ostream &os, const T &r) {
  return os << r;
}

template <class T, class... Args>
std::ostream &log_(std::ostream &os, const std::vector<T, Args...> &v) {
  os << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i > 0)
      os << ", ";
    else
      os << " ";
    os << v[i];
  }
  return os << " ]";
}

template <class... Args> void log(Args &&...args) {
  if (get_log_level() < 1)
    return;
  (log_(std::cout, args), ...);
}

template <class... Args> void logln(Args &&...args) {
  if (get_log_level() < 1)
    return;
  (log_(std::cout, args), ...);
  std::cout << std::endl;
}

std::fstream get_input(std::string filename, int argc, const char **arg);

template <template <class T, class... R> class C, class T, class U,
          std::enable_if_t<std::is_convertible_v<std::decay_t<U>, T>, int> = 0, class... R>
void sorted_insert(C<T, R...> &vec, U&& value) {
  using std::begin;
  using std::end;
  auto it = std::lower_bound(begin(vec), end(vec), value);
  vec.insert(it, value);
}

// replacement for std::erase that does not preserves the relative order of
// the remaining elements. It does this by swapping the element to be erased
// with the last element.
// The iterator value is changed in the process, as the value at the current iterator
// is swapped with the last element.
// This means you should only process the vector sequentially from the beginning
// to the end, without incrementing the iterator when calling erase, or you'll skip
// past the element that was swapped in!!!
//
// Example:
// >	std::vector<int> vec = {1, 2, 3, 4, 5, 6};
// >	auto it = vec.begin();
// >	while (it != vec.end()) {
// >	  if (*it % 2 == 0) {
// >	    unstable_erase(vec, it); // 2 is swapped with 6, then popped, so we need to reprocess 6 by not incrementing
// >	  } else {
// >	    it++;
// >	  }
// >	}
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

#ifdef COMPILE_UTILS

const auto NUMBERS = "0123456789";
const auto LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const auto SYMBOL_FIRST_CHAR = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const auto SYMBOL_NEXT_CHAR = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";

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

std::optional<ranged<std::string_view>> next_symbol(std::string_view str, size_t pos) {
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

std::optional<ranged<std::string_view>> next_alnum_sequence(std::string_view str, size_t pos) {
  auto r = std::find_if(str.begin() + pos, str.end(), [](char c) {
    return std::isalnum(c);
  });
  if (r != str.end()) {
    auto pos = std::distance(str.begin(), r);
    auto end = std::find_if_not(str.begin() + pos, str.end(), [](char c) {
      return std::isalnum(c);
    });
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
    left*=10;
    t /= 10;
  }
  return left + right;
}

#endif // HEADER_GUARD_UTILS_HPP
