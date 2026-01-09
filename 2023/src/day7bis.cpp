#include "utils.hpp"
#include "poker.hpp"

using namespace std;
using namespace poker_2;

struct play {
  hand h;
  size_t bid;
};

bool operator<(const play &a, const play &b) {
  return a.h < b.h;
}

ostream& operator<<(ostream& os, const play& p) {
  os << '{' << p.h << " | ";
  for (int i =0; i < 5; ++i) {
    switch(p.h.order[i]) {
      case 0: os << 'J'; break;
      case 10: os << 'T'; break;
      case 11: os << 'Q'; break;
      case 12: os << 'K'; break;
      case 13: os << 'A'; break;
      default: os << (p.h.order[i]);
    }
  }
  return os << " | " << p.bid << '}';
}

int main(int argc, const char **argv) {
  using namespace dpsg;

  auto file = get_input("day7.txt", argc, argv);

  vector<play> plays;
  string line;
  while(getline(file, line)) {
    auto h = line.substr(0, 5);
    auto bid = next_number(line, 5);
    if (bid) {
      auto p = play{.h= parse_hand(h), .bid = bid->value};
      logln("line: ", line, " -> ", p);
      sorted_insert(plays, p);
    } else {
      cerr << "Failed to parse line: " << line << endl;
    }
  }

  size_t total = 0;
  for (size_t i = 0; i < plays.size(); ++i) {
    auto this_bid = (i+1) * plays[i].bid;
    log(plays[i], " * ", i+1, " = ", this_bid);
    total += this_bid;
    logln(" -> ", total);
  }
  cout << total << endl;
}
