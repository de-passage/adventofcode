#include "coord.hpp"
#include "utils.hpp"

#include <queue>
#include <unordered_map>

using coordinates = dpsg::coord;

enum class direction { up, down, left, right };

struct directed_coordinates {
  coordinates pos;
  direction dir;
};

bool operator==(const directed_coordinates &lhs,
                const directed_coordinates &rhs) {
  return lhs.pos == rhs.pos && lhs.dir == rhs.dir;
}

template <> struct std::hash<directed_coordinates> {
  std::size_t operator()(const directed_coordinates &n) const {
    return std::hash<coordinates>()(n.pos) ^
           (std::hash<direction>()(n.dir) << 1);
  }
};

struct directed_coordinates_and_steps {
  directed_coordinates pos;
  int steps;
};

bool operator==(const directed_coordinates_and_steps &lhs,
                const directed_coordinates_and_steps &rhs) {
  return lhs.pos.pos == rhs.pos.pos && lhs.pos.dir == rhs.pos.dir &&
         lhs.steps == rhs.steps;
}

template <> struct std::hash<directed_coordinates_and_steps> {
  std::size_t operator()(const directed_coordinates_and_steps &n) const {
    return std::hash<coordinates>()(n.pos.pos) ^
           (std::hash<direction>()(n.pos.dir) << 1) ^
           (std::hash<int>()(n.steps) << 2);
  }
};

template <typename T> struct colored { const T &obj; };
template <typename T> colored(T &) -> colored<T>;
template <typename T> colored(const T &) -> colored<T>;

std::ostream &operator<<(std::ostream &os, colored<direction> d) {
  using namespace dpsg::vt100;
  switch (d.obj) {
  case direction::up:
    return os << "↑" << reset;
  case direction::down:
    return os << "↓" << reset;
  case direction::left:
    return os << "←" << reset;
  case direction::right:
    return os << "→" << reset;
  }
}

std::ostream &operator<<(std::ostream &os, colored<directed_coordinates> d) {
  using namespace dpsg::vt100;
  switch (d.obj.dir) {
  case direction::up:
    return os << (yellow | bold) << d.obj.pos << (magenta | bold) << "(↑)"
              << reset;
  case direction::down:
    return os << (yellow | bold) << d.obj.pos << (magenta | bold) << "(↓)"
              << reset;
  case direction::left:
    return os << (yellow | bold) << d.obj.pos << (magenta | bold) << "(←)"
              << reset;
  case direction::right:
    return os << (yellow | bold) << d.obj.pos << (magenta | bold) << "(→)"
              << reset;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, direction dir) {
  switch (dir) {
  case direction::up:
    return os << "up";
  case direction::down:
    return os << "down";
  case direction::left:
    return os << "left";
  case direction::right:
    return os << "right";
  }
  return os;
}

struct node {
  coordinates pos;
  int distance;
  int steps;
  direction direction;

  directed_coordinates last;
};

bool operator==(const node &lhs, const node &rhs) { return lhs.pos == rhs.pos; }

template <> struct std::hash<node> {
  std::size_t operator()(const node &n) const {
    return std::hash<coordinates>()(n.pos);
  }
};

struct node_compare {
  bool operator()(const node &lhs, const node &rhs) const {
    return lhs.distance > rhs.distance;
  }
};

int main(int argc, const char **argv) {
  using namespace std;
  using namespace dpsg;

  auto file = dpsg::get_input("day17.txt", argc, argv);

  vector<vector<int>> grid;

  string line;
  while (getline(file, line)) {
    vector<int> row;
    for (auto c : line) {
      row.push_back(c - '0');
    }
    grid.push_back(row);
  }

  node start = {
      .pos = coordinates{0, 0},
      .distance = 0,
      .steps = 0,
      .direction = direction::right,
      .last = {coordinates{0, 0}, direction::right},
  };

  priority_queue<node, std::vector<node>, node_compare> to_visit;
  to_visit.push(start);

  unordered_map<directed_coordinates_and_steps, node> visited;

  auto create_new_node_if_valid = [&](const node &current_node,
                                      coordinates next, direction dir) {
    using namespace dpsg::vt100;
    if (next.x < 0 || next.y < 0 || next.x >= static_cast<int>(grid.size()) ||
        next.y >= static_cast<int>(grid[0].size())) {
      return;
    }
    if (current_node.direction == dir && current_node.steps >= 9) {
      return;
    }
    if (current_node.direction != dir && current_node.steps < 3) {
      return;
    }

    auto new_node = node{
        .pos = next,
        .distance = current_node.distance + grid[next.x][next.y],
        .steps = current_node.direction == dir ? current_node.steps + 1 : 0,
        .direction = dir,
        .last = {current_node.pos, current_node.direction},
    };
    to_visit.emplace(move(new_node));
  };

  auto target = coordinates{static_cast<int>(grid.size() - 1),
                            static_cast<int>(grid[0].size() - 1)};

  while (!to_visit.empty()) {
    auto current = to_visit.top();
    to_visit.pop();

    auto v = visited.find({{current.pos, current.direction}, current.steps});
    if (v != visited.end()) {
      if (v->second.distance > current.distance) {
        v->second = current;
      } else {
        continue;
      }
    }
    visited.emplace(
        directed_coordinates_and_steps{{current.pos, current.direction},
                                       current.steps},
        current);

    if (current.pos == target) {
      if (current.steps < 3) {
        continue;
      }
      cout << (vt100::bg(vt100::color::white) | vt100::black) << "Found "
           << (vt100::red | vt100::bold) << current.distance << vt100::reset
           << endl;
      break;
    }

    auto [x, y] = current.pos;

    if (current.direction != direction::down) {
      auto up = coordinates{x - 1, y};
      create_new_node_if_valid(current, up, direction::up);
    }
    if (current.direction != direction::up) {
      auto down = coordinates{x + 1, y};
      create_new_node_if_valid(current, down, direction::down);
    }
    if (current.direction != direction::right) {
      auto left = coordinates{x, y - 1};
      create_new_node_if_valid(current, left, direction::left);
    }
    if (current.direction != direction::left) {
      auto right = coordinates{x, y + 1};
      create_new_node_if_valid(current, right, direction::right);
    }
  }
}
