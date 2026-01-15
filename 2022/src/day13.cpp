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

enum class comparison { lesser, equal, greater };
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

comparison compare(int left, int right) {
  return left == right
             ? comparison::equal
             : (left < right) ? comparison::lesser : comparison::greater;
}
comparison lesser(const tree &left, const tree &right, int idt);
comparison lesser(int left, int right, int idt) {
  cerr << string(idt, ' ') << "End of chain " << left << " / " << right
       << " returning " << compare(left, right) << endl;
    return compare(left, right);
}

comparison lesser(const tree &left, int right, int idt) {
  cerr << string(idt, ' ') << "comparing " << left << " with " << right << endl;
  tree t;
  t.add_int(right);
  return lesser(left, t, idt + 1);
}
comparison lesser(int left, const tree &right, int idt) {
  cerr << string(idt, ' ') << "comparing " << left << " with " << right << endl;
  tree t;
  t.add_int(left);
  return lesser(t, right, idt + 1);
}

comparison lesser(const tree &left, const tree &right, int idt) {
  int l = min(left.children.size(), right.children.size());
  cerr << string(idt, ' ') << "Comparing up to " << l << " values in " << left
       << "<=" << right << endl;
  for (int i = 0; i < l; ++i) {
    // cerr << "Ith nb : " << i << endl;
    auto &leftc = left.children[i];
    auto &rightc = right.children[i];
    if (leftc.index() == 0 && rightc.index() == 0) {
      auto c = lesser(get<0>(leftc), get<0>(rightc), idt + 1);
      if (c != comparison::equal) {
        cerr << string(idt, ' ') << "returning " << c << "\n";
        return c;
      }
    } else if (leftc.index() == 1 && rightc.index() == 0) {
      auto c = lesser(get<1>(leftc), get<0>(rightc), idt + 1);
      if (c != comparison::equal) {
        cerr << string(idt, ' ') << "returning " << c << "\n";
        return c;
      }
    } else if (leftc.index() == 0 && rightc.index() == 1) {
      auto c = lesser(get<0>(leftc), get<1>(rightc), idt + 1);
      if (c != comparison::equal) {
        cerr << string(idt, ' ') << "returning " << c << "\n";
        return c;
      }
    } else {
      auto c = lesser(get<1>(leftc), get<1>(rightc), idt + 1);
      if (c != comparison::equal) {
        cerr << string(idt, ' ') << "returning " << c << "\n";
        return c;
      }
    }
  }
  cerr << string(idt, ' ') << "Returning " << left.children.size()
       << " / " << right.children.size()
       << " == " << compare(left.children.size(), right.children.size()) << endl;
  return compare(left.children.size(), right.children.size());
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
  cerr << boolalpha;
  while (getline(file, line1) && getline(file, line2)) {
    current_pair++;
    tree root1;
    tree root2;
    parse_tree(root1, line1);
    parse_tree(root2, line2);
    cerr << "line1: " << line1 << '\n' << "line2: " << line2 << endl;

    int l, r;
    if (lesser(root1, root2, 0) == comparison::lesser) {
      cerr << "pair " << current_pair << " is ordered " << endl;
      sum += current_pair;
    }

    getline(file, discard);
  }
  cout << sum << endl;
}
