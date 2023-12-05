#include "utils.hpp"
#include <numeric>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, const char ** argv) {
  auto file = get_input("day4.txt", argc, argv);

  std::string line;

  vector<int> copies;
  size_t copy_idx = 0;

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

    if (copy_idx >= copies.size()) {
      copies.push_back(1);
    }
    size_t this_copy_count = copies[copy_idx];
    logln(" this copy count: ", this_copy_count, " | count: ", count);
    for (size_t it = copy_idx + 1; it < copy_idx + count + 1; ++it) {
      if (it >= copies.size()) {
        logln("pushed ", this_copy_count + 1);
        copies.push_back(this_copy_count + 1);
      } else {
        log("added ", this_copy_count);
        copies[it] += this_copy_count;
        logln(" => ", copies[it]);
      }
    }
    copy_idx++;
    logln();
  }

  for (auto c : copies) {
    log(c, " ");
  }

  logln(" copy_idx: ", copy_idx);
  cout << std::accumulate(copies.begin(), copies.begin() + copy_idx, 0) << endl;
}
