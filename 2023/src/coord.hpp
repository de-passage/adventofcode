#ifndef HEADER_GUARD_COORD_HPP
#define HEADER_GUARD_COORD_HPP

#include <functional>
#include <iostream>

namespace dpsg {

template <typename T> struct basic_coord {
  T x;
  T y;
};
using coord = basic_coord<int>;

} // namespace dpsg

namespace std {
template <class T> struct hash<dpsg::basic_coord<T>> {
  size_t operator()(dpsg::basic_coord<T> const &c) const noexcept {
    return (static_cast<size_t>(c.x) << (sizeof(T) / 2)) + static_cast<size_t>(c.y);
  }
};
} // namespace std

namespace dpsg {

template <class T>
std::ostream &operator<<(std::ostream &os, basic_coord<T> c) {
  os << "(" << c.x << ", " << c.y << ")";
  return os;
}

template <class T>
basic_coord<T> operator+(basic_coord<T> a, basic_coord<T> b) {
  return {a.x + b.x, a.y + b.y};
}

template <class T>
basic_coord<T> operator+=(basic_coord<T> &a, basic_coord<T> b) {
  return a = a + b;
}

template <class T> bool operator==(basic_coord<T> a, basic_coord<T> b) {
  return a.x == b.x && a.y == b.y;
}

template <class T> bool operator!=(basic_coord<T> a, basic_coord<T> b) {
  return !(a == b);
}

template <class T> bool operator<(basic_coord<T> a, basic_coord<T> b) {
  return a.x < b.x || (a.x == b.x && a.y < b.y);
}

template <class T> bool operator>(basic_coord<T> a, basic_coord<T> b) {
  return a.x > b.x || (a.x == b.x && a.y > b.y);
}

template <class T> bool operator<=(basic_coord<T> a, basic_coord<T> b) {
  return a < b || a == b;
}

template <class T> bool operator>=(basic_coord<T> a, basic_coord<T> b) {
  return a > b || a == b;
}

template <class T> basic_coord<T> operator-(basic_coord<T> c) {
  return {-c.x, -c.y};
}

} // namespace dpsg

#endif // HEADER_GUARD_COORD_HPP
