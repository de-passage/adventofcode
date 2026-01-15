
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>
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

struct position_with_distance {
  position pos;
  int distance;
  position_with_distance() = default;
  position_with_distance(position p, int d) : pos(p), distance(d) {}

  friend bool operator==(const position_with_distance &p1,
                         const position_with_distance &p2) {
    return p1.pos == p2.pos;
  }

  friend position_with_distance operator+(const position_with_distance &p1,
                                          const position &p2) {
    return {{p1.pos.x + p2.x, p1.pos.y + p2.y}, p1.distance + 1};
  }
};

struct position_hash {
  std::size_t operator()(const position_with_distance &pair) const {
    return std::hash<int>()(pair.pos.x) ^ std::hash<int>()(pair.pos.y);
  }
};

template<class F, class G>
int bfs(position_with_distance begin, position_with_distance end, int h, int y, F&& can_go_from_to, G&& validate) {
  queue<position_with_distance> to_visit;
  to_visit.push(begin);
  unordered_set<position_with_distance, position_hash> visited;
  visited.emplace(begin);

  while (!to_visit.empty()) {
    position_with_distance current = to_visit.front();
    to_visit.pop();
    if (current == end) {
      return current.distance;
    }
    for (position p : {position{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
      auto new_p = current + p;
      // cerr << "considering " << new_p.pos;
      bool valid = validate(new_p.pos);
      bool can_go = valid && can_go_from_to(current.pos, new_p.pos);
      bool already_visited = visited.count(new_p);
      if (valid && can_go && !already_visited) {
        visited.emplace(new_p);
        // cerr << " yes" << endl;
        to_visit.push(new_p);
      } else {
        // cerr << boolalpha << " no (valid: " << valid;
        // if (valid)
          // cerr << " can_go: " << can_go;
        // cerr << " already_visited: " << already_visited << ')' << endl;
      }
    }
  }
  return numeric_limits<int>::max();
}

DPSG_AOC_MAIN(file) {
  string line;
  vector<vector<int>> elevations;
  position_with_distance begin, end;
  vector<position_with_distance> starts;

  int x = 0;
  while (getline(file, line)) {
    elevations.emplace_back();
    for (int y = 0; y < line.size(); ++y) {
      char c = line[y];
      if (c == 'S' || c == 'a') {
        elevations.back().push_back(0);
        starts.emplace_back(position{x, y}, 0);
      } else if (c == 'E') {
        elevations.back().push_back(25);
        end = {{x, y}, 0};
      } else {
        elevations.back().push_back(c - 'a');
      }
    }
    x++;
  }

  int h = elevations.size();
  int w = elevations.front().size();

  const auto can_go_from_to = [&elevations](const position &from,
                                            const position &to) -> bool {
    return (elevations[to.x][to.y] - elevations[from.x][from.y]) <= 1;
  };
  const auto validate = [h, w, &can_go_from_to](const position &p) -> bool {
    return p.x >= 0 && p.x < h && p.y >= 0 && p.y < w;
  };

  vector<int> vals;

  for (auto begin : starts)
  vals.push_back(bfs(begin, end, h,w,can_go_from_to, validate));
  cout << *min_element(vals.begin(), vals.end()) << endl;
}
