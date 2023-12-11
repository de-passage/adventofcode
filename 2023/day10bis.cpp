#include "coord.hpp"
#include "utils.hpp"
#include <unordered_map>
#include <unordered_set>

using namespace std;
using namespace dpsg;

bool valid(coord c, vector<string> maze) {
  return c.x >= 0 && c.x < static_cast<int>(maze.size()) && c.y >= 0 &&
         c.y < static_cast<int>(maze[c.x].size());
}

auto at(coord c, vector<string> maze) { return maze[c.x][c.y]; }

struct connection {
  uint8_t east : 1 = 0;
  uint8_t west : 1 = 0;
  uint8_t north : 1 = 0;
  uint8_t south : 1 = 0;
};

std::ostream &operator<<(std::ostream &os, connection c) {
  bool subseq = false;
  os << "{ ";
  if (c.east)
    os << "east ";
  subseq = c.east;
  if (subseq && c.west)
    os << ", ";
  if (c.west)
    os << "west ";
  subseq |= c.west;
  if (subseq && c.north)
    os << ", ";
  if (c.north)
    os << "north ";
  subseq |= c.north;
  if (subseq && c.south)
    os << ", ";
  if (c.south)
    os << "south ";
  os << "}";
  return os;
}

struct cell {
  coord pos;
  connection c;
};

static_assert(sizeof(connection) == 1, "connection is not 1 byte");

connection direction_from_char(char c) {
  switch (c) {
  case '|':
    return {
        .north = 1,
        .south = 1,
    };
  case '-':
    return {
        .east = 1,
        .west = 1,
    };
  case 'F':
    return {
        .east = 1,
        .south = 1,
    };
  case 'L':
    return {
        .east = 1,
        .north = 1,
    };
  case '7':
    return {
        .west = 1,
        .south = 1,
    };
  case 'J':
    return {
        .west = 1,
        .north = 1,
    };
  case 'S':
    return {
        .east = 1,
        .west = 1,
        .north = 1,
        .south = 1,
    };
  default:
    return {};
  }
}

bool connected(connection from, connection to, coord direction) {
  if (direction == coord{0, 1}) {
    return from.east && to.west;
  } else if (direction == coord{0, -1}) {
    return from.west && to.east;
  } else if (direction == coord{1, 0}) {
    return from.south && to.north;
  } else if (direction == coord{-1, 0}) {
    return from.north && to.south;
  } else {
    return false;
  }
}

void print_maze(ostream& out, vector<string> maze, coord s) {
  for (size_t i = 0; i < maze.size(); i++) {
    for (size_t j = 0; j < maze[i].size(); j++) {
      if (s.x == static_cast<int>(i) && s.y == static_cast<int>(j)) {
        out << "*";
      } else {
        out << maze[i][j];
      }
    }
    out << std::endl;
  }
}

int main(int argc, const char **argv) {
  auto file = get_input("day10.txt", argc, argv);

  vector<string> maze;
  string line;
  coord s{-1, -1};

  size_t x = 0;
  while (getline(file, line)) {
    maze.push_back(line);
    for (size_t i = 0; i < line.size(); i++) {
      if (line[i] == 'S') {
        s = {static_cast<int>(x), static_cast<int>(i)};
      }
    }
    ++x;
  }

  coord last = {};
  unordered_set<coord> to_visit;
  unordered_set<coord> visited;
  unordered_set<coord> loop;
  do {
    auto p = at(s, maze);
    loop.insert(s);
    auto c = direction_from_char(p);
    logln("Currently at ", s, " ", c, " (", p, ")");
    logln([&](ostream& out) { print_maze(out, maze,  s); });

    // compute the cell inside the maze based on the direction we're going
    // we're going clockwise, and only full cells count, so we can simply
    // add the cells that are on the right (direction-wise) of the current
    // straight line
    if (p == '-') {
      log("Going horizontally ");
      if (last == s + coord{0, 1}) {
        auto i = s + coord{-1, 0};
        to_visit.insert(i);
        logln("from right to left, inserting the cell above (", i, ")");
      } else if (last == s + coord{0, -1}) {
        auto i = s + coord{1, 0};
        logln("from left to right, inserting the cell below (", i, ")");
        to_visit.insert(i);
      }
    } else if (p == '|') {
      log("Going vertically ");
      if (last == s + coord{1, 0}) {
        auto i = s + coord{0, 1};
        logln("from bottom to top, inserting the cell on the right (", i, ")");
        to_visit.insert(i);
      } else if (last == s + coord{-1, 0}) {
        auto i = s + coord{0, -1};
        logln("from top to bottom, inserting the cell on the left (", i, ")");
        to_visit.insert(i);
      }
    }/*  else if (p == 'J') { // For the remaining cases, we have 2 possible cases, yielding 1 or 2 cells to visit depending on the shape of the corner
      if (last == s + coord{-1, 0}) {
        log("Going from top to up")
      }
    } else if (p == '7') {
      log("Going from top to left ");
    } else if (p == 'L') {
      log("Going from bottom to right ");
    } else if (p == 'F') {
      log("Going from bottom to left ");
    } */
    visited.insert(s);


    // print_maze(maze, s);
    const auto l = s;
    // up left down right -> clockwise
    for (auto d : {coord{-1, 0}, coord{0, 1}, coord{1, 0}, coord{0, -1}}) {
      auto n = s + d;
      //logln("Checking ", s, " + ", d, " = ", n, " (last: ", last, ")");
      if (n == last) {
        //logln("Ignoring ", d, " because it's the last direction");
        continue;
      }
      if (!valid(n, maze)) {
        continue;
      }
      auto nc = at(n, maze);
      if (connected(c, direction_from_char(nc), d)) {
        // logln("Moving from ", s, " (", c, ") to ", n, " (",
        //       direction_from_char(nc), ")");
        last = s;
        s += d;
        break;
      }
    }
    if (l == s) {
      std::cerr << "Stuck at " << l << " " << c << std::endl;
      return 1;
    }
  } while (maze[s.x][s.y] != 'S');

  logln("Cells to visit (", to_visit.size(), "):", [&](auto &os) {
      for (auto it = to_visit.begin(); it != to_visit.end(); ++it) {
      if (it != to_visit.begin()) { os << ", "; }
        os << *it;
      }
  });

  unordered_set<coord> inside_loop;
  // We have the list of cells to visit and the list of cells forming the loop,
  // we can now flood fill the maze to find the cells that are inside the loop
  // but not part of the path
  while(!to_visit.empty()) {
    auto c = *to_visit.begin();
    to_visit.erase(c);
    if (visited.find(c) != visited.end()) { // we may have path cells in the to_visit list
      continue;
    }
    visited.insert(c);
    inside_loop.insert(c);

    for (auto d : {coord{-1, 0}, coord{0, -1}, coord{1, 0}, coord{0, 1}}) {
      auto n = c + d;
      if (!valid(n, maze)) {
        continue;
      }
      if (visited.find(n) != visited.end()) { // already visited (or in the path)
        continue;
      }
      to_visit.insert(n);
    }
  }

  auto c = 0;
  for (size_t y = 0; y < maze.size(); y++) {
    for (size_t x = 0; x < maze[y].size(); x++) {
      if (inside_loop.find(coord{static_cast<int>(y), static_cast<int>(x)}) != inside_loop.end()) {
        cout << "I";
      } else if (loop.find(coord{static_cast<int>(y), static_cast<int>(x)}) != loop.end()) {
        cout << maze[y][x];
      } else {
        cout << ".";
        c++;
      }
    }
    cout << endl;
  }
  std::cout << "Found " << inside_loop.size() << " cells inside the loop" << endl;
  std::cout << "Found " << loop.size() << " cells on the path" << endl;
  std::cout << "Found " << c << " cells outside the loop" << endl;
  std::cout << "Total grid size: " << maze.size() << "x" << maze[0].size() << " = " << maze.size() * maze[0].size() << endl;


  return 0;
}
