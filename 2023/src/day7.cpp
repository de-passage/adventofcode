#include "utils.hpp"
#include "poker.hpp"

using namespace std;
using namespace poker_1;

struct play {
  hand h;
  size_t bid;
};

bool operator<(const play &a, const play &b) {
  return a.h < b.h;
}

ostream& operator<<(ostream& os, const play& p) {
  return os << '{' << p.h << " | " << p.bid << '}';
}

int main(int argc, const char **argv) {
  auto file = get_input("day7.txt", argc, argv);

  vector<play> plays;
  string line;
  while(getline(file, line)) {
    auto h = line.substr(0, 5);
    auto bid = next_number(line, 5);
    if (bid) {
      sorted_insert(plays, play{.h= parse_hand(h), .bid = bid->value});
      logln("plays: ", plays);
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
