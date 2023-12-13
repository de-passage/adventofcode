#include "coord.hpp"
#include "utils.hpp"
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <numeric>

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

void doit(const std::vector<string>& maze, coord s) {
  coord last = {};
  vector<coord> vertices;
  size_t steps = 0;
  unordered_set<coord> loop;
  do {
    auto p = at(s, maze);
    loop.insert(s);
    if (p != '-' && p != '|') {
      vertices.push_back(s);
    }
    steps++;
    auto c = direction_from_char(p);
    logln("Currently at ", s, " ", c, " (", p, ")");
    logln([&](ostream& out) { print_maze(out, maze,  s); });

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
      exit(1);
    }
  } while (maze[s.x][s.y] != 'S');

  // Remove start is it's not a vertex
  if ((vertices[0].x == vertices[1].x && vertices[0].x == vertices[vertices.size() - 1].x)
      || (vertices[0].y == vertices[1].y && vertices[0].y == vertices[vertices.size() - 1].y)) {
    vertices.erase(vertices.begin());
  }

  auto sum = 0;
  for (auto i = 1u; i < vertices.size(); i++) {
    auto& v1 = vertices[i - 1];
    auto& v2 = vertices[i];
    sum += abs(v1.x*v2.y - v1.y*v2.x);
  }
  sum += abs(vertices[vertices.size() - 1].x*vertices[0].y - vertices[vertices.size() - 1].y*vertices[0].x);

  auto sum_f = 0;
  for (auto i = 1u; i < vertices.size(); i++) {
    auto& v1 = vertices[i - 1];
    auto& v2 = vertices[i];
    sum_f += v1.x*v2.y - v1.y*v2.x;
  }
  sum_f += vertices[vertices.size() - 1].x*vertices[0].y - vertices[vertices.size() - 1].y*vertices[0].x;

  auto sum_b = 0;
  for (auto i = vertices.size() - 1; i > 0; --i) {
    auto& v1 = vertices[i];
    auto& v2 = vertices[i - 1];
    sum_b += v1.x*v2.y - v1.y*v2.x;
  }
  sum_b += vertices[0].x*vertices[vertices.size() - 1].y - vertices[0].y*vertices[vertices.size() - 1].x;

  for (auto i = 0u; i < maze.size(); i++) {
    for (auto j = 0u; j < maze[i].size(); j++) {
      if (loop.find({static_cast<int>(i), static_cast<int>(j)}) == loop.end()) {
        std::cout << '.';
      } else {
        std::cout << maze[i][j];
      }
    }
    std::cout << std::endl;
  }
  double area = double(sum) / 2.;
  double area_f = double(sum_f) / 2.;
  double area_b = double(sum_b) / 2.;
  std::cout << "Found " << vertices.size() << " vertices" << std::endl; logln(vertices);
  std::cout << "Area (abs): " << area << " Sq root: " << std::sqrt(area) << std::endl;
  std::cout << "Area (forward): " << area_f << " Sq root: " << std::sqrt(area_f) << std::endl;
  std::cout << "Area (backward): " << area_b << " Sq root: " << std::sqrt(area_b) << std::endl;
  std::cout << "Steps: " << steps << std::endl;
  std::cout << "Area minus steps: " << (abs(area_f) - steps) << std::endl;
  auto grid = (abs(area_f) - (double(steps)/2.));
  std::cout << "Attempt: " << grid << " Sq root: " << sqrt(grid) << std::endl;
  std::cout << "Grid size: " << maze.size() << "x" << maze[0].size() << " == " << maze.size() * maze[0].size()<< std::endl;
}

int main(int argc, const char **argv) {
  auto file = get_input("day10.txt", argc, argv);

  vector<string> maze;
  string line;
  coord s{-1, -1};

  size_t x = 0;
  while (getline(file, line)) {
    if (line.empty()) {
      if (s.x == -1 && s.y == -1) {
        continue;
      } else {
        doit(maze, s);
      }
      maze.clear();
      s = {-1, -1};
      x = 0;
      continue;
    }
    maze.push_back(line);
    for (size_t i = 0; i < line.size(); i++) {
      if (line[i] == 'S') {
        s = {static_cast<int>(x), static_cast<int>(i)};
      }
    }
    ++x;
  }

  if (s.x == -1 && s.y == -1) {
    // do nothing
  } else if (!maze.empty()) {
    doit(maze, s);
  }

  return 0;
}
