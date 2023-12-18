#include "coord.hpp"
#include "utils.hpp"

#include <queue>

using coordinates = dpsg::basic_coord<size_t>;

enum class cell_type : char {
  empty = 0,            // .
  split_horizontal = 2, // -
  split_vertical = 3,   // |
  reflect_up = 4,       // slash
  reflect_down = 5,     // backslash
};

enum class ray_origin : char {
  none = 0,
  up = 1,
  down = 2,
  left = 4,
  right = 8
};

ray_origin operator|(ray_origin a, ray_origin b) {
  return static_cast<ray_origin>(static_cast<char>(a) | static_cast<char>(b));
}

ray_origin operator&(ray_origin a, ray_origin b) {
  return static_cast<ray_origin>(static_cast<char>(a) & static_cast<char>(b));
}

bool contains(ray_origin a, ray_origin b) { return (a & b) == b; }

struct cell {
  cell_type cell;
  ray_origin energized;
};

char to_char(cell_type c) {
  switch (c) {
  case cell_type::empty:
    return '.';
  case cell_type::split_horizontal:
    return '-';
  case cell_type::split_vertical:
    return '|';
  case cell_type::reflect_up:
    return '/';
  case cell_type::reflect_down:
    return '\\';
  default:
    return static_cast<char>(c) + '0';
  }
}

const char *to_string(cell_type c) {
  switch (c) {
  case cell_type::empty:
    return "empty";
  case cell_type::split_horizontal:
    return "split horizontal";
  case cell_type::split_vertical:
    return "split vertical";
  case cell_type::reflect_up:
    return "reflect up";
  case cell_type::reflect_down:
    return "reflect down";
  default:
    return "invalid";
  }
}

std::ostream &operator<<(std::ostream &out, cell_type c) {
  using namespace dpsg::vt100;
  switch (c) {
  case cell_type::empty:
    return out << (faint) << '.' << reset;
  default:
    return out << (bold | green) << to_char(c) << reset;
  }
}

char to_char(ray_origin o) {
  switch (o) {
  case ray_origin::none:
    return '.';
  case ray_origin::up:
    return 'v';
  case ray_origin::down:
    return '^';
  case ray_origin::left:
    return '>';
  case ray_origin::right:
    return '<';
  default:
    return static_cast<char>(o) + '0';
  }
}

std::ostream &operator<<(std::ostream &out, ray_origin o) {
  using namespace dpsg::vt100;
  switch (o) {
  case ray_origin::none:
    return out << (faint) << '.' << reset;
  default:
    return out << (bold | cyan) << to_char(o) << reset;
  }
}

struct ray {
  coordinates pos;
  ray_origin origin;
};

// up is forward
ray next_ray(ray r, ray_origin next) {
  auto [current, origin] = r;
  switch (origin) {
  case ray_origin::up:
    switch (next) {
    case ray_origin::up:
      return {
          .pos = {current.x + 1, current.y},
          .origin = ray_origin::up,
      };
    case ray_origin::left:
      return {
          .pos = {current.x, current.y + 1},
          .origin = ray_origin::left,
      };
    case ray_origin::right:
      return {
          .pos = {current.x, current.y - 1},
          .origin = ray_origin::right,
      };
    default:
      std::cerr << "invalid next direction: " << origin << " + " << (int)next
                << std::endl;
      exit(1);
    }
  case ray_origin::down:
    switch (next) {
    case ray_origin::up:
      return {
          .pos = {current.x - 1, current.y},
          .origin = ray_origin::down,
      };
    case ray_origin::left:
      return {
          .pos = {current.x, current.y - 1},
          .origin = ray_origin::right,
      };
    case ray_origin::right:
      return {
          .pos = {current.x, current.y + 1},
          .origin = ray_origin::left,
      };
    default:
      std::cerr << "invalid next direction: " << origin << " + " << (int)next
                << std::endl;
      exit(1);
    }
  case ray_origin::left:
    switch (next) {
    case ray_origin::up:
      return {
          .pos = {current.x, current.y + 1},
          .origin = ray_origin::left,
      };
    case ray_origin::left:
      return {
          .pos = {current.x - 1, current.y},
          .origin = ray_origin::down,
      };
    case ray_origin::right:
      return {
          .pos = {current.x + 1, current.y},
          .origin = ray_origin::up,
      };
    default:
      std::cerr << "invalid next direction: " << origin << " + " << (int)next
                << std::endl;
      exit(1);
    }
  case ray_origin::right:
    switch (next) {
    case ray_origin::up:
      return {
          .pos = {current.x, current.y - 1},
          .origin = ray_origin::right,
      };
    case ray_origin::left:
      return {
          .pos = {current.x + 1, current.y},
          .origin = ray_origin::up,
      };
    case ray_origin::right:
      return {
          .pos = {current.x - 1, current.y},
          .origin = ray_origin::down,
      };
    default:
      std::cerr << "invalid next ray: " << origin << " + " << (int)next
                << std::endl;
      exit(1);
    }
  default:
    std::cerr << "invalid ray origin: " << (int)origin << " + " << (int)next
              << std::endl;
    exit(1);
  }
}

int main(int argc, const char **argv) {
  using namespace std;
  using namespace dpsg;
  auto file = get_input("day16.txt", argc, argv);

  string line;
  using grid = vector<vector<cell>>;
  grid map;
  while (getline(file, line)) {
    auto &v = map.emplace_back();
    for (auto ch : line) {
      switch (ch) {
      case '.':
        v.push_back({cell_type::empty, ray_origin::none});
        break;
      case '-':
        v.push_back({cell_type::split_horizontal, ray_origin::none});
        break;
      case '|':
        v.push_back({cell_type::split_vertical, ray_origin::none});
        break;
      case '/':
        v.push_back({cell_type::reflect_up, ray_origin::none});
        break;
      case '\\':
        v.push_back({cell_type::reflect_down, ray_origin::none});
        break;
      default:
        std::cerr << "invalid character: " << ch << std::endl;
        exit(1);
      }
    }
  }

  std::queue<ray> rays;
  rays.push(ray{.pos = {0, 0}, .origin = ray_origin::left});

  while (!rays.empty()) {
    auto current = rays.front();
    rays.pop();

    logln("Processing ", (vt100::bold | vt100::yellow), current.pos,
          vt100::reset, " from ", current.origin);

    auto &cell = map[current.pos.x][current.pos.y];

    logln("Cell is ", vt100::bold | vt100::blue, to_string(cell.cell),
          vt100::reset);

    // If we already processed this cell, skip it
    if (contains(cell.energized, current.origin)) {
      logln("Already processed ", vt100::bold | vt100::yellow, current.pos, vt100::reset, " from ", current.origin, ", skipping");
      continue;
    }

    // Mark as energized so we don't process it again
    cell.energized = cell.energized | current.origin;

    const coordinates size{map[0].size(), map.size()};
    auto emplace_if_valid = [&rays, &size](auto &&ray) {
      if (ray.pos.x < size.x && ray.pos.y < size.y && ray.pos.x >= 0 &&
          ray.pos.y >= 0) {
        logln("Emplacing ", (vt100::bold | vt100::yellow), ray.pos,
              (vt100::reset), " from ", ray.origin);
        rays.emplace(ray);
      }
    };

    // Depending on the cell type and the direction, we add the next step(s) to
    // the queue
    switch (cell.cell) {
    case cell_type::empty:
      emplace_if_valid(next_ray(
          current, ray_origin::up)); // keep going in the same direction
      break;
    case cell_type::split_horizontal: // '-' is a split iff the ray is coming
                                      // from up or down
      if (current.origin == ray_origin::up ||
          current.origin == ray_origin::down) {
        emplace_if_valid(next_ray(current, ray_origin::left));
        emplace_if_valid(next_ray(current, ray_origin::right));
      } else {
        emplace_if_valid(next_ray(
            current, ray_origin::up)); // keep going in the same direction
      }
      break;
    case cell_type::split_vertical:
      if (current.origin == ray_origin::left ||
          current.origin == ray_origin::right) {
        emplace_if_valid(next_ray(current, ray_origin::left));
        emplace_if_valid(next_ray(current, ray_origin::right));
      } else {
        emplace_if_valid(next_ray(
            current, ray_origin::up)); // keep going in the same direction
      }
      break;
    case cell_type::reflect_up:
      if (current.origin == ray_origin::up) {
        emplace_if_valid(next_ray(current, ray_origin::right));
      } else if (current.origin == ray_origin::left) {
        emplace_if_valid(next_ray(current, ray_origin::left));
      } else if (current.origin == ray_origin::right) {
        emplace_if_valid(next_ray(current, ray_origin::left));
      } else {
        emplace_if_valid(next_ray(current, ray_origin::right));
      }
      break;
    case cell_type::reflect_down:
      if (current.origin == ray_origin::up) {
        emplace_if_valid(next_ray(current, ray_origin::left));
      } else if (current.origin == ray_origin::left) {
        emplace_if_valid(next_ray(current, ray_origin::right));
      } else if (current.origin == ray_origin::right) {
        emplace_if_valid(next_ray(current, ray_origin::right));
      } else {
        emplace_if_valid(next_ray(current, ray_origin::left));
      }
      break;
    }

    logln([&](auto &cout) {
      for (auto i = 0u; i < map.size(); ++i) {
        auto &row = map[i];
        for (auto j = 0u; j < row.size(); ++j) {
          auto &cell = row[j];
          if (current.pos.x == i && current.pos.y == j) {
            cout << vt100::bg(vt100::color::white);
            cout << (vt100::black | vt100::bold) << to_char(current.origin)
                 << vt100::reset;
          } else if (cell.cell == cell_type::empty) {
            cout << vt100::faint << cell.energized;
          } else {
            cout << cell.cell;
          }
        }
        cout << "\t\t";
        for (auto &cell : row) {
          cout << cell.energized;
        }
        cout << endl;
      }

      cout << endl;
    });
  }

  auto count = 0u;
  for (auto& row : map) {
    for (auto& cell : row) {
      if (cell.energized != ray_origin::none) {
        ++count;
      }
    }
  }

  std::cout << "Total cells energized: " << (vt100::bold | vt100::green) << count << vt100::reset << std::endl;

  return 0;
}
