#include "utils.hpp"
#include <algorithm>

DPSG_AOC_MAIN(file) {
  using namespace std;
  using namespace dpsg;

  string line;
  getline(file, line);
  vector<range> seeds;
  auto s = next_number(line);
  auto e = next_number(line, s->position.end);
  do {
    auto r = range{s->value, s->value + e->value};
    range_merge_insert(seeds, r);
    s = next_number(line, e->position.end);
    e = next_number(line, s->position.end);
  } while (s && e);

  logln("inital seeds: ", seeds);

  for (;;) {
    do {
      if (!getline(file, line)) {
        logln("ERROR parsing line (empty): jumping outside of loop");
        goto done;
      }
    } while (line.empty());
    logln(line);

    vector<range> new_seeds;
    while (getline(file, line) && !line.empty()) {
      logln("\nParsing: ", line);
      auto dest = next_number(line);
      auto src = next_number(line, dest->position.end);
      auto dist = next_number(line, src->position.end);
      logln("dest: ", dest->value, " src: ", src->value,
            " dist: ", dist->value);

      if (!dest || !src || !dist) {
        logln("error parsing line: ", line);
        goto done;
      }

      range src_range = {src->value, src->value + dist->value};
      range dest_range = {dest->value, dest->value + dist->value};
      long long offset = (long long)dest_range.begin - (long long)src_range.begin;
      logln("src: ", src_range, " dest: ", dest_range, " offset: ", offset, " remaining seeds: ", seeds);

      for (auto it = seeds.begin(); it != seeds.end();) {
        logln(*it, " & ", src_range);
        auto extrusion = extrude(*it, src_range);
        logln("extrusion: [left: ", extrusion.left, " | extrusion : ", extrusion.extruded, " | right: ", extrusion.right, " ]");
        if (extrusion.extruded.size() > 0) {
          range_merge_insert(new_seeds, extrusion.left);
          range_merge_insert(new_seeds, extrusion.right);
          logln("merging ", extrusion.extruded, " with offset ", offset, " -> ", extrusion.extruded + offset);
          range_merge_insert(new_seeds, extrusion.extruded + offset);
          it = seeds.erase(it);
        } else {
          ++it;
        }

      }
    }
    logln();
    log("inserting ");
    for (auto seed : seeds) {
      log(seed, ' ');
      range_merge_insert(new_seeds, seed);
    }
    swap(seeds, new_seeds);
    logln(" -> new seeds: ", seeds);

    logln("Step over. Current seeds: ", seeds, "\n-------------------------------");
  }
done:

  cout << "seeds: [";
  for (size_t i = 0; i < seeds.size(); ++i) {
    cout << "\t\n" << seeds[i];
  }
  cout << "\n]" << endl;

  if (seeds.size() > 0) {
    cout << "min: " << seeds.front() << endl;
  } else {
    cerr << "no seeds in array" << endl;
  }
}
