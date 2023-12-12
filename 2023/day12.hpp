#ifndef HEADER_GUARD_DAY12_HPP
#define HEADER_GUARD_DAY12_HPP

#include "utils.hpp"
#include <iostream>
#include <vector>
namespace dpsg {

enum class State { Broken, Operational, Unknown };

inline std::ostream &operator<<(std::ostream &out, State s) {
  switch (s) {
  case State::Broken:
    return out << '#';
  case State::Operational:
    return out << '.';
  case State::Unknown:
    return out << '?';
  }
}

enum class Expectation { Broken, Operational, Any };

inline std::ostream& operator<<(std::ostream& out, Expectation e) {
  switch (e) {
  case Expectation::Broken:
    return out << "Broken";
  case Expectation::Operational:
    return out << "Operational";
  case Expectation::Any:
    return out << "Any";
  }
}

#ifdef DPSG_PUSH_BACK_NODES
  #define DPSG_INSERT_OR_MERGE(v, n) v.push_back(n)
#else
void insert_or_merge(auto& v, auto n) {
  for (auto& e : v) {
    if (e.required_size == n.required_size && e.groups == n.groups && e.expected_next == n.expected_next) {
      e.similar_nodes += n.similar_nodes;
      return;
    }
  }
  v.push_back(n);
};
#define DPSG_INSERT_OR_MERGE(v, n) insert_or_merge(v, n)
#endif

template <template <class, class...> class C,
          template <class, class...> class D, class... Args1, class... Args2>
size_t count_possibilities(const C<State, Args1...> &states,
                        const D<int, Args2...> &expectations) {
  using GroupContainer = D<int, Args2...>;
  if (expectations.empty()) {
    return 1;
  }

  auto total_n = expectations[0];
  for (auto i = 1u; i < expectations.size(); ++i) {
    total_n += expectations[i] + 1; // +1 for the space between groups
  }

  // Virtual node, represent the point of the possibility tree where we're at
  // We don't actually need the whole tree, just the number of valid leaves
  struct node {
    int required_size;
    GroupContainer groups;
    Expectation expected_next;
    size_t similar_nodes;
  };

  std::vector<node> current_depth_level;
  current_depth_level.push_back({
      .required_size = total_n,
      .groups = expectations,
      .expected_next = Expectation::Any,
      .similar_nodes = 1,
  });

  auto create_broken_node = [](auto current_node) {
    // We decrement the current group size. If it's 0, we remove it from
    // the list of groups, and we expect the next value to be operational
    current_node.required_size -= 1;
    if (--current_node.groups[0] == 0) {
      current_node.required_size -= 1; // may go negative, but fine bc we break on greater than remaining size
      current_node.groups.erase(current_node.groups.begin());
      current_node.expected_next = Expectation::Operational;
    } else {
      // If we're not at 0, we're inside the group, so the next value must be
      // broken
      current_node.expected_next = Expectation::Broken;
    }
    return current_node;
  };

  auto create_operational_node = [](auto current_node) {
    // We don't decrement the current group size. Next value can be anything
    current_node.expected_next = Expectation::Any;
    return current_node;
  };

  // We go over each value once, creating a layer of the tree for each one
  int remaining_values = states.size();
  for (auto current_state : states) {
    remaining_values -= 1;
    std::vector<node> next_depth_level;

    for (auto current_node : current_depth_level) {
      switch (current_state) {
      case State::Broken:
        if (current_node.expected_next == Expectation::Operational){
          // This possibility subtree is invalid
          continue;
        }

        if (current_node.required_size <= 0) {
          // We can't have more broken nodes, this subtree is invalid
          continue;
        }
        DPSG_INSERT_OR_MERGE(next_depth_level, create_broken_node(current_node));

        break;
      case State::Operational:
        if (current_node.expected_next == Expectation::Broken) {
          // This possibility subtree is invalid
          continue;
        }
        if(current_node.required_size > remaining_values) {
          // If the required size to fit this branch is greater than the number of
          // remaining values, we won't be able to find a valid solution
          continue;
        }

        DPSG_INSERT_OR_MERGE(next_depth_level, create_operational_node(current_node));

        break;

      case State::Unknown:
        // If we have a specific expectation, we must assume unknown is that.
        if (current_node.expected_next == Expectation::Broken) {
          DPSG_INSERT_OR_MERGE(next_depth_level, create_broken_node(current_node));
        } else if (current_node.expected_next == Expectation::Operational) {
          if(current_node.required_size > remaining_values) {
            // If the required size to fit this branch is greater than the number of
            // remaining values, we won't be able to find a valid solution
            continue;
          }
          DPSG_INSERT_OR_MERGE(next_depth_level, create_operational_node(current_node));
        } else {
          if(current_node.required_size <= remaining_values) {
            DPSG_INSERT_OR_MERGE(next_depth_level, create_operational_node(current_node));
          }
          if (current_node.required_size > 0) {
            if (current_node.required_size <= 0) {
              // We can't have more broken nodes, this subtree is invalid
              continue;
            }
            DPSG_INSERT_OR_MERGE(next_depth_level, create_broken_node(current_node));
          }
        }

        break;
      }
    }
    std::swap(current_depth_level, next_depth_level);
  }

  // We sum the number of leaves in the tree
  size_t total = 0;
  for (auto& n : current_depth_level) {
    total += n.similar_nodes;
  }
  return total;
}

inline std::pair<std::vector<State>, std::vector<int>> process_input_line(const std::string &line) {
  std::vector<State> states;
  for (auto c : line) {
    switch (c) {
    case '#':
      states.push_back(State::Broken);
      break;
    case '.':
      states.push_back(State::Operational);
      break;
    case '?':
      states.push_back(State::Unknown);
      break;
    default:
      goto condition_list;
    }
  }

condition_list:
  std::vector<int> conditions;
  for (auto n = next_number(line); n; n = next_number(line, n->position.end)) {
    conditions.push_back(n->value);
  };

  return {states, conditions};
}

} // namespace dpsg

#endif // HEADER_GUARD_DAY12_HPP
