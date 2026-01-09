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

template <class T> struct ::std::hash<dpsg::basic_coord<T>> {
  size_t operator()(dpsg::basic_coord<T> c) const {
    return (c.x << (sizeof(T) / 2)) + c.y;
  }
};

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
