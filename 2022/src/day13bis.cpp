#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <stack>
#include <variant>
#include "utils.hpp"

using namespace std;

using node = variant<struct tree, int>;

struct tree {
  tree() = default;
  tree *parent;
  vector<node> children;

  tree *add_child();
  void add_int(int i) { children.push_back(i); }
};

ostream &operator<<(ostream &out, const tree &t) {
  out << '[';
  for (int i = 0; i < t.children.size(); ++i) {
    if (t.children[i].index() == 0) {
      out << get<0>(t.children[i]);
    } else {
      out << get<1>(t.children[i]);
    }
    if (i != t.children.size() - 1)
      cout << ',';
  }
  return out << ']';
}

enum class comparison { lesser = -1, equal = 0, greater = 1 };
ostream& operator<<(ostream& out, comparison c) {
  switch(c) {
    case comparison::lesser:
      return out << "<";
    case comparison::greater:
      return out << ">";
    default:
      return out << "=";
  }
}

comparison compare(const tree &left, const tree &right);
comparison compare(int left, int right) {
  return left == right
             ? comparison::equal
             : (left < right) ? comparison::lesser : comparison::greater;
}

comparison compare(const tree &left, int right) {
  tree t;
  t.add_int(right);
  return compare(left, t);
}
comparison compare(int left, const tree &right) {
  tree t;
  t.add_int(left);
  return compare(t, right);
}

comparison compare(const tree &left, const tree &right) {
  int l = min(left.children.size(), right.children.size());
  for (int i = 0; i < l; ++i) {
    auto &leftc = left.children[i];
    auto &rightc = right.children[i];
    if (leftc.index() == 0 && rightc.index() == 0) {
      auto c = compare(get<0>(leftc), get<0>(rightc));
      if (c != comparison::equal) {
        return c;
      }
    } else if (leftc.index() == 1 && rightc.index() == 0) {
      auto c = compare(get<1>(leftc), get<0>(rightc));
      if (c != comparison::equal) {
        return c;
      }
    } else if (leftc.index() == 0 && rightc.index() == 1) {
      auto c = compare(get<0>(leftc), get<1>(rightc));
      if (c != comparison::equal) {
        return c;
      }
    } else {
      auto c = compare(get<1>(leftc), get<1>(rightc));
      if (c != comparison::equal) {
        return c;
      }
    }
  }
  return compare(left.children.size(), right.children.size());
}

bool operator<(const tree& left, const tree& right) {
  return compare(left, right) == comparison::lesser;
}

bool operator==(const tree& left, const tree& right) {
  return compare(left, right) == comparison::equal;
}

tree *tree::add_child() {
  children.emplace_back(in_place_type<tree>);
  tree *child = &get<tree>(children.back());
  child->parent = this;
  return child;
}

void parse_tree(tree &tree, const string_view &line) {
  int acc = 0;
  struct tree *current = &tree;
  bool nb = false;

  for (char c : line.substr(1)) {
    switch (c) {
    case '[':
      current = current->add_child();
      break;
    case ']':
      if (nb)
        current->add_int(acc);
      nb = false;
      acc = 0;
      current = current->parent;
      break;
    case ',':
      if (nb)
        current->add_int(acc);
      nb = false;
      acc = 0;
      break;
    default:
      assert(c >= '0' && c <= '9');
      acc *= 10;
      acc += (c - '0');
      nb = true;
      break;
    }
  }
}

DPSG_AOC_MAIN(file) {
  string line1;
  string line2;
  string discard;

  int sum = 0;
  int current_pair = 0;
  vector<tree> trees;

  while (getline(file, line1) && getline(file, line2)) {
    tree root1;
    tree root2;
    parse_tree(root1, line1);
    parse_tree(root2, line2);
    trees.emplace_back(move(root1));
    trees.emplace_back(move(root2));

    getline(file, discard);
  }
  tree root1;
  tree root2;
  parse_tree(root1, "[[2]]");
  parse_tree(root2, "[[6]]");
  trees.emplace_back(root1);
  trees.emplace_back(root2);
  sort(trees.begin(), trees.end());

  auto r1 = find(trees.begin(), trees.end(), root1);
  auto r2 = find(r1+1, trees.end(), root2);

  int d1 = distance(trees.begin(), r1) + 1;
  int d2 = distance(trees.begin(), r2) + 1;

  cout << d1 << " * " << d2 << " == " << d1 * d2 << endl;
}
