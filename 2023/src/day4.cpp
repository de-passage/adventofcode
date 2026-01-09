#include "utils.hpp"
#include <vector>

using namespace std;

DPSG_AOC_MAIN(file) {
  using namespace dpsg;

  std::string line;
  size_t sum = 0;
  while (getline(file, line)) {
    logln(line);

    vector<int> n1;
    vector<int> n2;

    auto beg = line.find_first_of(":");
    if (beg == std::string::npos) {
      logln("invalid line");
      continue;
    }
    beg++;
    auto end = line.find_first_of("|", beg);

    for (auto p = next_number(line, beg); p.has_value() && p->position.end < end; p = next_number(line, p->position.end)) {
      sorted_insert(n1, p->value);
    }

    beg = end + 1;
    for (auto p = next_number(line, beg); p.has_value(); p = next_number(line, p->position.end)) {
      sorted_insert(n2, p->value);
    }

    size_t idx1 = 0;
    size_t idx2 = 0;
    int count = 0;

    log("in common: ");
    while (idx1 < n1.size() && idx2 < n2.size()) {
      if (n1[idx1] < n2[idx2]) {
        idx1++;
      } else if (n1[idx1] > n2[idx2]) {
        idx2++;
      } else {
        log(n1[idx1], " ");
        count++;
        idx1++;
        idx2++;
      }
    }

    logln("total: ", count);
    if (count > 0) {
      sum += (1 << (count - 1));
      logln("1 << (", count - 1, ") = ", sum);
    }
  }

  cout << sum << endl;
}
