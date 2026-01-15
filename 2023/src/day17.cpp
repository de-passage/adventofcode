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

template<>
struct std::hash<directed_coordinates_and_steps> {
  std::size_t operator()(const directed_coordinates_and_steps& n) const {
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
  default: // shouldn't happen
    return os;
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
  direction dir;

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

DPSG_AOC_MAIN(file) {
  using namespace std;
  using namespace dpsg;

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
      .dir = direction::right,
      .last = {coordinates{0, 0}, direction::right},
  };

  priority_queue<node, std::vector<node>, node_compare> to_visit;
  to_visit.push(start);

  unordered_map<directed_coordinates_and_steps, node> visited;
  unordered_map<directed_coordinates, vector<directed_coordinates>>
      visited_paths;

  auto create_new_node_if_valid = [&](const node &current_node,
                                      coordinates next, direction dir) {
    using namespace dpsg::vt100;
    log(faint, "Considering adding ", (yellow|bold), next, (reset|faint), " with direction ", colored{dir});
    if (next.x < 0 || next.y < 0 || next.x >= static_cast<int>(grid.size()) ||
        next.y >= static_cast<int>(grid[0].size())) {
      logln(" -> ", faint|red, "Out of bounds", reset);
      return;
    }
    if (current_node.dir == dir && current_node.steps >= 2) {
      logln(" -> ", faint|red, " too many steps", reset);
      return;
    }

    auto new_node = node{
        .pos = next,
        .distance = current_node.distance + grid[next.x][next.y],
        .steps = current_node.dir == dir ? current_node.steps + 1 : 0,
        .dir = dir,
        .last = {current_node.pos, current_node.dir},
    };
    logln(" -> ", faint|green, "valid", reset|faint, " with distance ", green|bold, new_node.distance, reset|faint, " and steps ", cyan|bold, new_node.steps, reset|faint, " and direction ", magenta|bold, new_node.dir);
    to_visit.emplace(move(new_node));
  };

  auto target = coordinates{static_cast<int>(grid.size() - 1),
                            static_cast<int>(grid[0].size() - 1)};

  while (!to_visit.empty()) {
    auto current = to_visit.top();
    to_visit.pop();

    // Print the logs for the current node
    logln("Considering ", current.pos, vt100::reset, " with distance ",
          vt100::green | vt100::bold, current.distance, vt100::reset,
          " and direction ", vt100::magenta | vt100::bold, current.dir,
          vt100::reset, " and steps ", vt100::cyan | vt100::bold, current.steps,
          ".", vt100::reset);

    log([&](auto &out) {
      auto p = visited_paths.find(current.last);

      const auto &current_path =
          p != visited_paths.end() ? p->second : vector<directed_coordinates>{};

      out << (vt100::reset | vt100::blue);
      for (auto i = 0u; i < grid.size(); ++i) {
        for (auto j = 0u; j < grid[i].size(); ++j) {
          bool found = false;
          if (coordinates{static_cast<int>(i), static_cast<int>(j)} ==
              current.pos) {
            out << (vt100::reset | vt100::red | vt100::bold)
                << colored{current.dir};
            found = true;
          } else {
            for (auto &n : current_path) {
              if (n.pos ==
                  coordinates{static_cast<int>(i), static_cast<int>(j)}) {
                out << (vt100::magenta|vt100::bold) << colored{n.dir};
                found = true;
              }
            }
          }
          if (!found)
            out << grid[i][j];
          else {
            out << (vt100::reset | vt100::blue);
          }
        }
        out << '\n' << (vt100::reset | vt100::blue);
      }
    });

    log(vt100::reset|vt100::faint, " -> ");

    auto v = visited.find({{current.pos, current.dir}, current.steps});
    if (v != visited.end()) {
      log(vt100::faint, "Already visited\n", vt100::faint, " --> ");
      if (v->second.distance > current.distance) {
        v->second = current;
        logln("with higher distance, ", vt100::underline, "updating",
              vt100::reset);
        auto last_path = visited_paths[{current.last.pos, current.last.dir}];
        auto& current_path = visited_paths[{current.pos, current.dir}];
        last_path.push_back({current.pos, current.dir});
        current_path = move(last_path);
      } else {
        logln("with lower distance, ", vt100::underline, "ignoring",
              vt100::reset);

        continue;
      }
    } else {
      logln(vt100::underline, "New node", vt100::reset);
      auto p = visited_paths.find(current.last);
      auto current_path =
          p != visited_paths.end() ? p->second : vector<directed_coordinates>{};
      current_path.push_back({current.pos, current.dir});
      visited_paths[{current.pos, current.dir}] = move(current_path);
    }

    visited.emplace(directed_coordinates_and_steps{{current.pos, current.dir}, current.steps},
                    current);

    if (current.pos == target) {
      cout << (vt100::bg(vt100::color::white) | vt100::black) << "Found "
           << (vt100::red | vt100::bold) << current.distance << vt100::reset
           << endl;
      break;
    }

    auto [x, y] = current.pos;

    auto up = coordinates{x - 1, y};
    auto down = coordinates{x + 1, y};
    auto left = coordinates{x, y - 1};
    auto right = coordinates{x, y + 1};

    if (current.dir != direction::down) {
      create_new_node_if_valid(current, up, direction::up);
    }
    if (current.dir != direction::up) {
      create_new_node_if_valid(current, down, direction::down);
    }
    if (current.dir != direction::right) {
      create_new_node_if_valid(current, left, direction::left);
    }
    if (current.dir != direction::left) {
      create_new_node_if_valid(current, right, direction::right);
    }
  }
}
