#include "coord.hpp"
#include "utils.hpp"

using namespace std;
using namespace dpsg;

bool valid(coord c, vector<string> maze) {
  return c.x >= 0 && c.x < maze.size() && c.y >= 0 && c.y < maze[c.x].size();
}

auto at(coord c, vector<string> maze) {
  return maze[c.x][c.y];
}

struct  connection {
  uint8_t east: 1 = 0;
  uint8_t west: 1 = 0;
  uint8_t north: 1 = 0;
  uint8_t south: 1 = 0;
};

std::ostream& operator<<(std::ostream& os, connection c) {
  bool subseq = false;
  os << "{ ";
  if (c.east) os << "east ";
  subseq = c.east;
  if (subseq && c.west) os << ", ";
  if (c.west) os << "west ";
  subseq |= c.west;
  if (subseq && c.north) os << ", ";
  if (c.north) os << "north ";
  subseq |= c.north;
  if (subseq && c.south) os << ", ";
  if (c.south) os << "south ";
  os << "}";
  return os;
}

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
  if (direction == coord{0,1}) {
    return from.east && to.west;
  } else if (direction == coord{0,-1}) {
    return from.west && to.east;
  } else if (direction == coord{1,0}) {
    return from.south && to.north;
  } else if (direction == coord{-1,0}) {
    return from.north && to.south;
  } else {
    return false;
  }
}

void print_maze(vector<string> maze, coord s) {
  for (size_t i = 0; i < maze.size(); i++) {
    for (size_t j = 0; j < maze[i].size(); j++) {
      if (s.x == i && s.y == j) {
        std::cout << "*";
      } else {
        std::cout << maze[i][j];
      }
    }
    std::cout << std::endl;
  }
}

int main(int argc, const char** argv) {
  auto file = get_input("day10.txt", argc, argv);

  vector<string> maze;
  string line;
  coord s{-1,-1};

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

  int steps = 0;
  coord last = {};
  do {
    auto c = direction_from_char(maze[s.x][s.y]);
    logln("Currently at ", s, " ", c);
    //print_maze(maze, s);
    steps++;
    const auto l = s;
    for (auto d : {coord{0,-1}, coord{-1,0}, coord{0,1}, coord{1,0}}) {
      auto n = s + d;
      logln("Checking ", s, " + ", d, " = ", n, " (last: ", last, ")");
      if (n == last) {
        logln("Ignoring ", d, " because it's the last direction");
        continue;
      }
      if (!valid(n, maze)) {
        continue;
      }
      auto nc = at(n, maze);
      if (connected(c, direction_from_char(nc), d)) {
        logln("Moving from ",s, " (", c, ") to ", n, " (", direction_from_char(nc), ")");
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

  std::cout << steps/2 << std::endl;

}
