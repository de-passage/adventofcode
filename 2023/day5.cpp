#include "utils.hpp"
#include <algorithm>

int main(int argc, const char ** argv) {
    using namespace std;

  auto file = get_input("day5.txt", argc, argv);

  string line;
  getline(file, line);
  vector<int> seeds;
  for(auto s = next_number(line); s; s = next_number(line, s->position.end)) {
    seeds.push_back(s->value);
  }

  for (;;) {
    do {
      if (!getline(file, line)) { goto done; }
    } while (line.empty());

    if (!getline(file, line)) { goto done; } // title line

    while (getline(file, line) && !line.empty()) {
      auto dest = next_number(line);
      auto src = next_number(line, dest->position.end);
      auto dist = next_number(line, src->position.end);

      if (!dest || !src || !dist) {
       logln("error parsing line: ", line);
        goto done;
      }

      range src_range = {static_cast<size_t>(src->value), static_cast<size_t>(src->value + dist->value)};
      range dest_range = {static_cast<size_t>(dest->value), static_cast<size_t>(dest->value + dist->value)};

      auto it = find(seeds.begin(), seeds.end(), src->value);
      if (it != seeds.end()) {
        stable_erase(seeds, src->value);
      }

    }


  }
done:

  for (auto seed : seeds) {
    cout << seed << ", ";
  }

  auto e = min_element(seeds.begin(), seeds.end());
  if (e != seeds.end()) {
    cout << "min: " << *e << endl;
  } else
    cout << "no values in array" << endl;
}


