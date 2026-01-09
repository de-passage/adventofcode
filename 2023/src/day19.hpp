#ifndef HEADER_GUARD_DAY19_HPP
#define HEADER_GUARD_DAY19_HPP

#include <string>
#include <iostream>

namespace dpsg {
  template <class T>
  struct basic_rule {
    std::string target;
    basic_rule(std::string target) noexcept : target{std::move(target)} {}
    virtual bool apply(T p) const = 0;
    virtual ~basic_rule() = default;
    virtual std::ostream& print(std::ostream& out) const {
      return out << "rule{" << target << "}";
    }
  };
}

#endif // HEADER_GUARD_DAY19_HPP
