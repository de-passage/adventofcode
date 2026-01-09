#include "utils.hpp"
#include <algorithm>

int main(int argc, const char ** argv) {
    using namespace std;

  auto file = get_input("day5.txt", argc, argv);

  string line;
  getline(file, line);
  vector<size_t> seeds;
  for(auto s = next_number(line); s; s = next_number(line, s->position.end)) {
    sorted_insert(seeds, s->value);
  }

  logln("inital seeds: ", seeds);

  for (;;) {
    do {
      if (!getline(file, line)) {
        logln("ERROR parsing line (empty): jumping outside of loop");
        goto done;
      }
      logln("<empty>", line);
    } while (line.empty());

    vector<size_t> new_seeds;
    while (getline(file, line) && !line.empty()) {
      logln("Parsing: ", line);
      auto dest = next_number(line);
      auto src = next_number(line, dest->position.end);
      auto dist = next_number(line, src->position.end);
      logln("dest: ", dest->value, " src: ", src->value, " dist: ", dist->value);

      if (!dest || !src || !dist) {
       logln("error parsing line: ", line);
        goto done;
      }

      range src_range = {src->value, src->value + dist->value};
      range dest_range = {dest->value, dest->value + dist->value};
      logln("src: ", src_range, " dest: ", dest_range);


      for (auto it = seeds.begin(); it != seeds.end();) {
        const auto& seed = *it;
        if (src_range.contains(seed)) {
          auto offset = seed - src_range.begin;
          logln("removing ", seed, " from seeds (became " , dest_range.begin + offset , ")");

          sorted_insert(new_seeds, dest_range.begin + offset);
          unstable_erase(seeds, it);
        } else {
          logln("keeping ", seed, " in seeds");
          it++;
        }
      }
    }
    log("inserting ");
    for (auto seed : seeds) {
      log(seed, ' ');
      sorted_insert(new_seeds, seed);
    }
    swap(seeds, new_seeds);
    logln(" -> new seeds: ", seeds);

    logln("Step over. Current seeds: ", seeds, "\n\n");
  }
done:

  cout << "seeds: [";
  for (size_t i = 0; i < seeds.size(); ++i) {
    if (i > 0)
      cout << ", ";
    else
      cout << " ";
    cout << seeds[i];
  }
  cout << " ]" << endl;

  if (seeds.size() > 0) {
    cout << "min: " << seeds.front() << endl;
  } else {
    cerr << "no seeds in array" << endl;
  }
}


