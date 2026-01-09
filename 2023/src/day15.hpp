#ifndef HEADER_GUARD_DAY15_HPP
#define HEADER_GUARD_DAY15_HPP

#include <algorithm>
#include <numeric>
#include <string_view>

namespace dpsg {

struct tokenizer {
  std::string_view data;
  char delimiter;

  struct iterator {
    tokenizer *parent;
    std::string_view::iterator begin;
    std::string_view::iterator end;

    iterator &operator++() {
      if (end == parent->data.end()) {
        begin = end;
        return *this;
      }
      begin = end + 1;
      end = std::find(begin, parent->data.end(), parent->delimiter);
      return *this;
    }

    std::string_view operator*() const {
      return std::string_view(&*begin, std::distance(begin, end));
    }

    bool operator!=(const iterator &other) const { return !(*this == other); }

    bool operator==(const iterator &other) const {
      return begin == other.begin && end == other.end;
    }
  };

  iterator begin() {
    return iterator{this, data.begin(),
                    std::find(data.begin(), data.end(), delimiter)};
  }

  iterator end() { return iterator{this, data.end(), data.end()}; }
};

using i64 = int64_t;
using u64 = uint64_t;

constexpr u64 hash(char c, u64 seed = 0) { return ((seed + c) * 17) % 256; }

constexpr u64 hash(std::string_view str) {
  return std::accumulate(str.begin(), str.end(), 0,
                         static_cast<u64 (*)(char c, u64)>(::dpsg::hash));
}

} // namespace

#endif // HEADER_GUARD_DAY15_HPP
