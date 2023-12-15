#include "coord.hpp"
#include "utils.hpp"

constexpr auto coord_color = dpsg::vt100::green | dpsg::vt100::bold;
constexpr auto in_coord_color = [](const auto& v) -> decltype(auto) {
  return dpsg::vt100::generic_decorate{coord_color, v};
};
constexpr auto blocker_color = dpsg::vt100::red | dpsg::vt100::bold;
constexpr auto rock_color = dpsg::vt100::blue | dpsg::vt100::bold;
constexpr auto counter_color = dpsg::vt100::yellow | dpsg::vt100::bold;
constexpr auto empty_color = dpsg::vt100::faint;
constexpr auto comment = dpsg::vt100::faint | dpsg::vt100::white;

namespace dpsg {
  std::ostream &operator<<(std::ostream &out, basic_coord<size_t> c) {
    using namespace dpsg::vt100;
    out << coord_color << "(" << c.x << ',' << c.y << ")" << reset;
    return out;
  }
}
using coordz = dpsg::basic_coord<size_t>;

enum class What {
  Empty,
  Rock,
  Blocker,
};

std::ostream &operator<<(std::ostream &out, What w) {
  using namespace dpsg::vt100;
  switch (w) {
  case What::Empty:
    out << empty_color << '.' << reset;
    break;
  case What::Rock:
    out << rock_color << 'O' << reset;
    break;
  case What::Blocker:
    out << blocker_color << '#' << reset;
    break;
  }
  return out;
}

struct mass_counter {
  size_t start = 0;
  int count = 0;

  friend bool operator==(const mass_counter &lhs, const mass_counter &rhs) {
    return lhs.start == rhs.start && lhs.count == rhs.count;
  }
};

std::ostream &operator<<(std::ostream &out, const mass_counter &c) {
  using namespace dpsg::vt100;
  using namespace dpsg::vt100;
  out << counter_color << '{' << c.start << ',' << c.count << '}' << reset;
  return out;
}

struct dish_representation {
  using subcontainer_type = std::vector<mass_counter>;
  using container_type = std::vector<subcontainer_type>;
  container_type data;
  size_t line_count;

  friend bool operator==(const dish_representation &lhs,
                         const dish_representation &rhs) {
    return lhs.data == rhs.data;
  }

  What at(size_t x, size_t y) const {
    auto &v = data[x];
    y += 1;
    auto it = std::find_if(v.begin(), v.end(), [y](auto &c) {
      return c.start <= y && y <= c.start + c.count;
    });
    if (it != v.end()) {
      if (it->start == y) {
        return What::Blocker;
      } else if (y - 1 < it->start + it->count) {
        return What::Rock;
      } else {
        return What::Empty;
      }
    } else {
      return What::Empty;
    }
  }

  template <class T>
  friend std::basic_ostream<T> &operator<<(std::basic_ostream<T> &out,
                                           const dish_representation &d) {
    return print_array(out, d.data, d.line_count);
  }

  template <class T>
  static std::basic_ostream<T>& print_array(std::basic_ostream<T>& out, const container_type& data, size_t line_count) {
    for (auto &col : data) {
      size_t t = 0;
      for (auto &row_data : col) {
        if (t != 0) {
          for (; t < row_data.start; ++t) {
            out << What::Empty;
          }
          out << What::Blocker;
        }
        ++t;
        for (auto i = 0; i < row_data.count; ++i, ++t) {
          out << What::Rock;
        }
      }
      while (t++ <= line_count) {
        out << What::Empty;
      }

      out << " -> "; dpsg::log_(out, col);
      out << std::endl;
    }
    return out;
  }

  size_t current_load() const noexcept {
    size_t mass = 0;
    for (auto &v : data) {
      for (auto &c : v) {
        auto start_value = line_count - c.start;
        auto s = start_value * (start_value + 1) / 2;
        auto end_value = start_value - c.count;
        auto e = end_value * (end_value + 1) / 2;
        mass += s - e;
      }
    }
    return mass;
  }

  // rotate counter-clockwise
  void tilt() {
    // Start at 1 because line 0 is filled with blockers
    std::vector<subcontainer_type::const_iterator>
        indices; // Index of the currently considered element each of the lines

    container_type new_container;
    new_container.resize(line_count);
    for (auto i = 0u; i < line_count; ++i) {
      new_container[i].emplace_back();
      indices.emplace_back(data[i].cbegin());
    }

    // The data is represented as a list. Each element corresponds to a line of
    // the visual representation. Each line is represented by a list of stopping
    // points, each one of those is its position on the line (the outside of the
    // visual is implicitely a blocker, but is not actually part of it, so 0
    // must be dropped when tilting) and the number of elements held by the
    // blocker.
    //
    // We need to tilt the representation in order, which means transposing the
    // data counter-clockwise. Re-counting the number of elements held by each
    // blocker will preserve the simulation we want. The columns are not
    // explicit in the data so we have to compute them as we go.
    //
    // Example rotations
    //
    // #O.. -> {0, 0}, {1, 1}
    // .#.# -> {0, 0}, {2, 0}, {4, 0}
    // O.#. -> {0, 1}, {3, 0}
    // OOO. -> {0, 3}
    //
    // .#.. -> {0, O}, {4, 0}
    // ..#O -> {0, 0}, {3, 0}
    // O#O. -> {0, 0}, {2, 0}
    // #OO. -> {0, 1}, {3, 1}
    //
    // O...
    // .#OO
    // #.#O
    // O..#
    //
    // OO.#
    // O.#.
    // .#..
    // O.#O
    //
    // We can go over each column (1 to line_count), and for each line, check if
    // the current index falls inside a group of blocked elements. This would be
    // computed by checking if current_index > start && current_index <= start +
    // count If current_value == start, then this is a new blocker, so we need
    // to add it to the new container. To simplify knowing what group should be
    // considered in the current line, we can keep a list of iterators to the
    // current relevant group for each line. When the current column index
    // exceed the current group count (start + count), we can increment the
    // iterator

    // basic transposition would be
    // for (auto row = 0u; row < indices.size(); ++row) {
    //  for (auto col = 0u; col < line_count; ++col) {
    //    new_data[line_count - row - 1] = data[row][col];
    //  }
    // }
    //
    // In our case we don't have the columns, we only have indirect information
    // about what would be in them.

    using namespace dpsg;
    using namespace dpsg::vt100;
    for (auto row = 0u; row < indices.size(); ++row) {
      logln("----------------------------------------");
      logln(*this);
      for (auto col = 1u; col <= line_count; ++col) {

        log("Considering ", coordz{col, row});
        auto &it = indices[row];

        if (it == data[row].cend()) { // There are no more blockers on this line
          logln(" => No more blockers on this line");
          continue;
        }
        auto current = *it;

        logln(" => current blocker is ", current, " in ", data[row]);

        if (col < current.start) { // We haven't reached the next blocker yet
          logln("We haven't reached the next blocker yet");
          continue;
        }

        while (current.start + current.count < col) {
          logln(comment, " -> -> Incrementing the current blocker iterator since current.count "
                "< col (",
                (current.start + current.count), " < ", col, ")", reset);
          ++it;
          if (it == data[row].cend()) {
            logln("No more blockers on this line");
            goto next_line;
          }
          current = *it;
        }
        logln("Current blocker is ", current, ", col ", (green|bold), col,
              reset, " transposed to ", (green|bold), col, reset);
        if (current.start == col) { // We are on the new blocker, we can add it
                                    // to the new container
          logln(comment, "-> We are on a ", rock_color, "new blocker", comment, " we can add it to the new container");
          new_container[col - 1].push_back({
              .start = line_count - row,
              .count = 0,
          });
        } else if (current.start < col &&
                   current.start + current.count >= col) {
          logln(comment, "-> ", blocker_color, "Incrementing", comment ,", the current blocker count", reset);
          new_container[col - 1].back().count++;
        } else {
          logln(comment, "Nothing to do here", reset);
        }

        logln("=> New container is now\n", [&](auto &out) {
          for (auto &c : new_container) {
            log_(out, c);
            out << '\n';
          }
        });
      }
    next_line:;
    }

    std::swap(new_container, data);
  }

  // rotate clockwise
  void rtilt() {
    std::vector<subcontainer_type::const_iterator>
        indices; // Index of the last blocker seen on the original data
    container_type new_container(line_count);
    for (auto i = 0u; i < line_count; ++i) {
      indices.emplace_back(
          data[i].cbegin());
          new_container[i].emplace_back();
    }

    using namespace dpsg;
    using namespace dpsg::vt100;

    // we want to build the new container where each line corresponds to a
    // column in the original data To do this, we traverse the colums in reverse
    // order computing whether there would be data there or not
    for (auto dest_row = 0uz; dest_row < line_count; dest_row++) {
      auto &dest = new_container[line_count - dest_row - 1];
      logln("----------------------------------------");
      logln(*this);
      logln([&](auto &out) {
          print_array(out, new_container, line_count);
          });

      // Columns are not explicitely represented in the data, we need to compute
      // them. We can do this by going over each line. At every point we keep
      // track of the current relevant blocker.
      for (auto orig_row = 0uz; orig_row < line_count; orig_row++) {
        auto &it = indices[orig_row];
        logln("Considering original row ", in_coord_color(orig_row), " and dest row: " , in_coord_color(dest_row), " with current blocker: ", *it, reset);

        if (it == data[orig_row].cend()) { // There are no more blockers on this line
          logln(decorate{comment," => No more blockers on this line"});
          continue;
        }

        // We find the current relevant blocker, that is the first blocker that
        // contains or is after the current column in this context, dest_row ==
        // current column - 1 (since col 0 is implicitely a blocker)
        auto current_col = dest_row + 1;
        logln("Current column is ", in_coord_color(current_col));

        auto current = *it; // The current blocker
        while (current_col > current.start + current.count) {
          logln(comment, "--> Incrementing the current blocker iterator since current_col "
                "> current.start + current.count (",
                green, current_col, " > ", (current.start + current.count), white, ") ");
          ++it;
          if (it == data[orig_row].cend()) {
            logln(underline, "No more blockers on this line", reset);
            goto next_line;
          }
          current = *it;
          logln("--> New current blocker is ", yellow, current, reset);
        }

        if (current.start == current_col) { // We are on the new blocker, we can
                                            // add it to the new container
          logln(comment, "-> We are on a ", blocker_color, "new blocker", reset|comment, " we can add it to the new container", reset);
          dest.push_back({
              .start = orig_row,
              .count = 0,
          });
        } else if (current.start < current_col &&
                   current_col <= current.start + current.count) {
          logln(comment, "-> ", rock_color, "Incrementing", reset|comment, ", the current blocker count", reset);
          dest.back().count++;
        } else {
          logln(comment, "Nothing to do here", reset);
        }
next_line:;
      } // inner loop
      logln([&](auto &out) {
          print_array(out, new_container, line_count);
          });
      logln("----------------------------------------");
    } // outer loop
    logln(bold|underline|black|bg(color::white), "Swapping containers", reset);
    std::swap(new_container, data);
  }

  void cycle() {
    for (int i = 0; i < 4; ++i) {
      rtilt();
    }
  }
};

enum class Dir {
  North,
  West,
  East,
  South,
};

int main(int argc, const char **argv) {
  using namespace std;
  using namespace dpsg;

  auto file = get_input("day14.txt", argc, argv);

  string line;
  dish_representation data;
  while (getline(file, line)) {
    if (data.data.size() == 0) {
      data.data.resize(line.size());
      for (auto i = 0u; i < line.size(); ++i) {
        data.data[i].emplace_back();
      }
    }

    data.line_count++;

    for (auto i = 0u; i < line.size(); ++i) {
      auto c = line[i];
      if (c == '.') {
        // nothing to do
      } else if (c == '#') {
        data.data[line.size() - i - 1].push_back(
            {.start = data.line_count, .count = 0});
      } else if (c == 'O') {
        data.data[line.size() - i - 1].back().count++;
      }
    }
  }

  auto p = [](const auto &data, Dir orientation) {
    if (orientation == Dir::North) {
      logln("Printing ", vt100::cyan, "north");
      std::cout << data << std::endl;
      for (auto row = 0u; row < data.line_count; ++row) {
        for (auto col = 0u; col < data.line_count; ++col) {
          std::cout << data.at(data.line_count - col - 1, row);
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    } else if (orientation == Dir::West) {
      logln("Printing ", vt100::cyan, "west");
      cout << data << std::endl;
      for (auto row = 0u; row < data.line_count; ++row) {
        for (auto col = 0u; col < data.line_count; ++col) {
          std::cout << data.at(col, row);
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    } else if (orientation == Dir::South) {
      logln("Printing ", vt100::cyan, "south");
      std::cout << data << std::endl;
      for (auto row = 0u; row < data.line_count; ++row) {
        for (auto col = 0u; col < data.line_count; ++col) {
          std::cout << data.at(col, data.line_count - row - 1);
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    } else if (orientation == Dir::East) {
      logln("Printing ", vt100::cyan, "east");
      std::cout << data << std::endl;
      for (auto row = 0u; row < data.line_count; ++row) {
        for (auto col = 0u; col < data.line_count; ++col) {
          std::cout << data.at(data.line_count - row - 1,
                               data.line_count - col - 1);
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }
  };
  p(data, Dir::North);
  data.rtilt();
  p(data, Dir::West);
  // data.rtilt();
  // p(data, Dir::South);
  // data.rtilt();
  // p(data, Dir::East);
  //
  return 0;
}
