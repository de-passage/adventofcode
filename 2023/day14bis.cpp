#include "coord.hpp"
#include "utils.hpp"
#include <unordered_map>

// There's a lot of garbage in this file, most of it was to get the algorithms right.
// I discovered that matrix index rotation is my nemesis and had a really hard time
// getting the simulation right. The internal representation doesn't help, but it makes
// the final computation easier. Marginally.

constexpr auto coord_color = dpsg::vt100::green | dpsg::vt100::bold;
constexpr auto in_coord_color = [](const auto &v) -> decltype(auto) {
  return dpsg::vt100::generic_decorate{coord_color, v};
};
constexpr auto blocker_color = dpsg::vt100::red | dpsg::vt100::bold;
constexpr auto rock_color = dpsg::vt100::blue | dpsg::vt100::bold;
constexpr auto counter_color = dpsg::vt100::yellow | dpsg::vt100::bold;
constexpr auto empty_color = dpsg::vt100::faint;
constexpr auto comment = dpsg::vt100::faint | dpsg::vt100::white;
constexpr auto title_color = dpsg::vt100::bold | dpsg::vt100::underline |
                             dpsg::vt100::black | bg(dpsg::vt100::color::white);

namespace dpsg {
std::ostream &operator<<(std::ostream &out, basic_coord<size_t> c) {
  using namespace dpsg::vt100;
  out << coord_color << "(" << c.x << ',' << c.y << ")" << reset;
  return out;
}
} // namespace dpsg
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
  enum class Dir {
    North,
    West,
    East,
    South,
  };

  template <class T>
  friend std::basic_ostream<T> &operator<<(std::basic_ostream<T> &out, Dir d) {
    using namespace dpsg::vt100;
    switch (d) {
    case Dir::North:
      out << "North";
      break;
    case Dir::West:
      out << "West";
      break;
    case Dir::East:
      out << "East";
      break;
    case Dir::South:
      out << "South";
      break;
    }
    return out;
  }

  inline void print_representation(std::ostream &out) {
    using namespace dpsg;
    auto orientation = directions[current_direction];

    out << "Tilted " << orientation << std::endl;
    if (orientation == Dir::North) {
      for (auto row = 0u; row < line_count; ++row) {
        for (auto col = 0u; col < line_count; ++col) {
          out << at(line_count - col - 1, row);
        }
        out << std::endl;
      }
      out << std::endl;
    } else if (orientation == Dir::West) {
      for (auto row = 0u; row < line_count; ++row) {
        for (auto col = 0u; col < line_count; ++col) {
          out << at(row, col);
        }
        out << std::endl;
      }
      out << std::endl;
    } else if (orientation == Dir::South) {
      for (auto row = 0u; row < line_count; ++row) {
        for (auto col = 0u; col < line_count; ++col) {
          out << at(col, line_count - row - 1);
        }
        out << std::endl;
      }
      out << std::endl;
    } else if (orientation == Dir::East) {
      for (auto row = 0u; row < line_count; ++row) {
        for (auto col = 0u; col < line_count; ++col) {
          out << at(line_count - row - 1, line_count - col - 1);
        }
        out << std::endl;
      }
      out << std::endl;
    }
  };

  using subcontainer_type = std::vector<mass_counter>;
  using container_type = std::vector<subcontainer_type>;
  container_type data;
  size_t line_count;
  constexpr static inline Dir directions[] = {Dir::North, Dir::East, Dir::South,
                                              Dir::West};
  int current_direction = 0;

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
  static std::basic_ostream<T> &print_array(std::basic_ostream<T> &out,
                                            const container_type &data,
                                            size_t line_count) {
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

      out << " -> ";
      dpsg::log_(out, col);
      out << std::endl;
    }
    return out;
  }

  size_t current_load() const noexcept {
    if (directions[current_direction] == Dir::North) {
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
    } else if (directions[current_direction] == Dir::East) {
      auto acc = 0;
      for (auto i = 1uz; i <= data.size(); ++i) { // data is eastward, so north is the last line, the weights go up
        auto &v = data[i - 1];
        for (auto &c : v) {
          acc += c.count * i;
        }
      }
      return acc;
    }
    std::cerr << "Current load not implemented for " << directions[current_direction]
              << std::endl;
    std::exit(1);
  }

  // rotate counter-clockwise
  void tilt() {
    using namespace dpsg;
    using namespace dpsg::vt100;

    // Start at 1 because line 0 is filled with blockers
    std::vector<subcontainer_type::const_iterator>
        indices; // Index of the currently considered element each of the lines

    container_type new_container;
    new_container.resize(line_count);
    for (auto i = 0u; i < line_count; ++i) {
      new_container[i].emplace_back();
      indices.emplace_back(data[i].cbegin());
    }

    log(title_color, "Rotating counter clockwise, direction ", blue,
        directions[current_direction]);
    current_direction = current_direction-- == 0 ? 3 : current_direction;
    logln(title_color, " -> ", blue, directions[current_direction]);
    for (auto col = 1u; col <= line_count; ++col) {
      logln("----------------------------------------");
      logln(*this);
      logln([&](auto &out) { print_array(out, new_container, line_count); });
      for (int row = indices.size() - 1; row >= 0; --row) {

        auto &it = indices[row];

        if (it == data[row].cend()) { // There are no more blockers on this line
          logln(" => No more blockers on this line");
          continue;
        }
        auto current = *it;

        auto new_row = col - 1;
        auto new_col = line_count - row - 1;
        logln("Considering col ", in_coord_color(col), " transposed to row ", in_coord_color(new_row),
              " and row ", in_coord_color(row), " transposed to col ", in_coord_color(new_col), " with current blocker: ", *it,
              reset);

        if (col < current.start) { // We haven't reached the next blocker yet
          logln(comment, "We haven't reached the next blocker yet", reset);
          continue;
        }

        while (current.start + current.count < col) {
          logln(comment,
                " -> -> Incrementing the current blocker iterator since "
                "current.count "
                "< col (",
                (current.start + current.count), " < ", col, ")", reset);
          ++it;
          if (it == data[row].cend()) {
            logln("No more blockers on this line");
            goto next_line;
          }
          current = *it;
        }

        logln("Current blocker is ", current, ", col ", (green | bold), col, reset);
        if (current.start == static_cast<size_t>(col)) { // We are on the new blocker, we can add
                                        // it to the new container
          logln(comment, "-> We are on a ", rock_color, "new blocker", comment,
                " we can add it to the new container", reset);
          new_container[new_row].push_back({
              .start = line_count - row,
              .count = 0,
          });
        } else if (current.start < static_cast<size_t>(col) &&
                   static_cast<size_t>(col) <= current.start + current.count) {
          new_container[new_row].back().count++;
          logln(comment, "-> ", blocker_color, "Incrementing", comment,
                ", the current blocker count to ", new_container[new_row].back(), reset);
        } else {
          logln(comment, "Nothing to do here", reset);
        }
next_line:;
      }
      logln([&](auto &out) { print_array(out, new_container, line_count); });
      logln("----------------------------------------");
    }

    logln(bold | underline | black | bg(color::white), "Swapping containers",
          reset);
    logln(*this);
    logln([&](auto &out) { print_array(out, new_container, line_count); });
    std::swap(new_container, data);
  }

  // rotate clockwise
  void rtilt() {
    using namespace dpsg;
    using namespace dpsg::vt100;

    log(title_color, "Rotating clockwise, direction ", blue,
        directions[current_direction]);
    current_direction = (current_direction + 1) % 4;
    logln(title_color, " -> ", blue, directions[current_direction]);

    std::vector<subcontainer_type::const_iterator>
        indices; // Index of the last blocker seen on the original data
    container_type new_container(line_count);
    for (auto i = 0u; i < line_count; ++i) {
      indices.emplace_back(data[i].cbegin());
      new_container[i].emplace_back();
    }

    // we want to build the new container where each line corresponds to a
    // column in the original data To do this, we traverse the colums in reverse
    // order computing whether there would be data there or not
    for (auto dest_row = 0uz; dest_row < line_count; dest_row++) {
      auto &dest = new_container[line_count - dest_row - 1];
      logln("----------------------------------------");
      logln(*this);
      logln([&](auto &out) { print_array(out, new_container, line_count); });

      // Columns are not explicitely represented in the data, we need to compute
      // them. We can do this by going over each line. At every point we keep
      // track of the current relevant blocker.
      for (auto orig_row = 0uz; orig_row < line_count; orig_row++) {
        auto &it = indices[orig_row];
        logln("Considering original row ", in_coord_color(orig_row),
              " and dest row: ", in_coord_color(dest_row),
              " with current blocker: ", *it, reset);

        if (it ==
            data[orig_row].cend()) { // There are no more blockers on this line
          logln(decorate{comment, " => No more blockers on this line"});
          continue;
        }

        // We find the current relevant blocker, that is the first blocker that
        // contains or is after the current column in this context, dest_row ==
        // current column - 1 (since col 0 is implicitely a blocker)
        auto current_col = dest_row + 1;
        logln("Current column is ", in_coord_color(current_col));

        auto current = *it; // The current blocker
        while (current_col > current.start + current.count) {
          logln(
              comment,
              "--> Incrementing the current blocker iterator since current_col "
              "> current.start + current.count (",
              green, current_col, " > ", (current.start + current.count), white,
              ") ");
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
          dest.push_back({
              .start = orig_row + 1,
              .count = 0,
          });
          logln(comment, "-> We are on a ", blocker_color, "new blocker",
                reset | comment, " we can add it to the new container -> ",
                yellow, dest.back(), reset);
        } else if (current.start < current_col &&
                   current_col <= current.start + current.count) {
          logln(comment, "-> ", rock_color, "Incrementing", reset | comment,
                ", the current blocker count", reset);
          dest.back().count++;
        } else {
          logln(comment, "Nothing to do here", reset);
        }
      next_line:;
      } // inner loop
      logln([&](auto &out) { print_array(out, new_container, line_count); });
      logln("----------------------------------------");
    } // outer loop
    logln(bold | underline | black | bg(color::white), "Swapping containers",
          reset);
    logln(*this);
    logln([&](auto &out) { print_array(out, new_container, line_count); });
    std::swap(new_container, data);
  }

  void cycle() {
    for (int i = 0; i < 4; ++i) {
      tilt();
    }
  }
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
  //data.print_representation(std::cout);
  data.tilt();
  //data.print_representation(std::cout);
  data.tilt();
  //data.print_representation(std::cout);
  data.tilt();
  //std::cout << "After 1 cycle: \n";
  //data.print_representation(std::cout);

  constexpr auto ITERATIONS = 1000000000;
  std::vector<int> loads;
  loads.emplace_back(data.current_load());
  std::unordered_map<int, std::vector<size_t>> number_indices; // not using multimap because I don't want to look up whether relative order is preserved
  number_indices.emplace(loads.back(), 0);
  auto last = loads.back();
  for (int iteration = 2; iteration <= ITERATIONS; ++iteration) {
    data.cycle();
    auto load = data.current_load();
    loads.push_back(load);


    auto& vec = number_indices[load];
    if (load == last) {
      continue;
    }
    last = load;
    for(auto it = vec.rbegin(); it != vec.rend(); ++it) {
      // look for a cycle. A cycle is defined as a sequence of numbers identical to the
      // end sequence of the array that stretchs for the length of the distance between the
      // last element and the matching element.
      // For example, if the array is [1,2,3,4,5,6,7,8,10,9,10,7,8,10,9,10], the cycle is
      // [7,8,10,9,10], found by looking at each 10 starting from the end, and testing numbers in
      // the array in reverse as long as they match. If we reach the matching index, we have a cycle

      auto met_idx = *it;
      auto last_idx = iteration - 1;

      for (auto i = 0u; i < (last_idx - met_idx); ++i) {
        auto left = loads[last_idx - i];
        auto right = loads[met_idx - i];
        if (left != right) {
          goto next;
        }
      }
      // we have a cycle

      auto cycle_length = last_idx - met_idx;
      std::cout << "Found cycle of length " << cycle_length << " starting at " << met_idx << std::endl;
      for (auto i = 0u; i < cycle_length; ++i) {
        std::cout << loads[last_idx - i] << ' ';
      }
      // We can now compute the value of the element at iteration 1 billion
      auto steps_in_cycle = (ITERATIONS - met_idx) % cycle_length;
      std::cout << " => " << loads[met_idx + steps_in_cycle - 1] << std::endl;

      goto done;

    }

next: vec.push_back(iteration - 1);
  }
done:

  return 0;
}
