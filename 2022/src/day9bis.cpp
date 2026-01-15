#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <cmath>
#include <limits>
#include "utils.hpp"

using namespace std;

struct position {
  position() = default;
  position(position const &) = default;
  position(int x, int y) : x{x}, y{y} {}
  int x = 0;
  int y = 0;

  friend position operator+(const position &p1, const position &p2) {
    return {p1.x + p2.x, p1.y + p2.y};
  }

  friend position operator-(const position &p1, const position &p2) {
    return {p1.x - p2.x, p1.y - p2.y};
  }

  friend position &operator+=(position &p1, const position &p2) {
    return p1 = {p1 + p2};
  }

  friend bool operator==(const position &p1, const position &p2) {
    return p1.x == p2.x && p1.y == p2.y;
  }
};

ostream &operator<<(ostream &out, const position &pos) {
  return out << '{' << pos.x << ',' << pos.y << '}';
}

int distance_squared(const position &p1, const position &p2) {
  position r = p1 - p2;
  return r.x * r.x + r.y * r.y;
}

struct position_hash {
  std::size_t operator()(const position &pair) const {
    return std::hash<int>()(pair.x) ^ std::hash<int>()(pair.y);
  }
};

double distance(const position p1, const position p2) {
  return sqrt(static_cast<double>(distance_squared(p1,p2)));
}

position snap(const position& head, position tail) {
  int d = distance_squared(head, tail);
  if (d > 2) {
    int best = numeric_limits<int>::max();
    position best_pos = tail;
    for (position p :
    {position{1, 0}, {1, 1}, {0, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1,-1}, {-1,1}}) {
      position n = tail + p;
      int td = distance_squared(n, head);
      if (best > td) {
        best = td;
        best_pos = n;
      }
    }
    tail = best_pos;
  }
  return tail;
}

DPSG_AOC_MAIN(file) {
  string line;
  unordered_set<position, position_hash> positions;
  position knots[10] = { {0, 0}, {0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0}};
  while (getline(file, line)) {
    char action = line[0];
    int count = std::atoi(line.substr(line.find_first_of("0123456789")).c_str());
    const position move = [action]() -> position {
      switch (action) {
      case 'D':
        return {-1, 0};
      case 'U':
        return {1, 0};
      case 'L':
        return {0, -1};
      case 'R':
        return {0, 1};
      };
      return {0, 0};
    }();

    while (count-- > 0) {
      knots[0] += move;
      for (int i = 0; i < 9; ++i) {
        knots[i+1] = snap (knots[i], knots[i+1]);
      }
      positions.emplace(knots[9]);
    }
  }

  cout << positions.size() << endl;
}
