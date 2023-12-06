#include "utils.hpp"
#include <algorithm>

int main(int argc, const char **argv) {
  using namespace std;

  auto file = get_input("day5.txt", argc, argv);

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
      logln("src: ", src_range, " dest: ", dest_range, " remaining seeds: ", seeds);

      for (auto it = seeds.begin(); it != seeds.end();) {
        logln(*it, " & ", src_range);
        // it is potentially in range (beg <= it.beg <= end)
        if (src_range.begin <= it->begin && it->begin <= src_range.end) {
          if (it->end <= src_range.end) {
            // it is fully contained, we can insert the whole range mapped
            auto offset = it->begin - src_range.begin;
            auto new_range = range{dest_range.begin + offset, dest_range.begin + it->size() + offset};
            logln("inserting the whole range, became: ", new_range);
            range_merge_insert(new_seeds, new_range);
          } else {
            // We need to split the range in two, the first part is mapped, the other isn't.
            range left = { it->begin, src_range.end };
            range right = { src_range.end, it -> end };
            auto offset = left.begin - src_range.begin;
            auto new_range = range{dest_range.begin + offset, dest_range.begin + left.size() + offset};
            logln("inserting the split range ", left, " (cast to ", new_range, ") and ", right);
            range_merge_insert(new_seeds, new_range);
            range_merge_insert(new_seeds, right);
          }
          unstable_erase(seeds, it);
        } else if (src_range.begin > it->begin && src_range.end < it->end) {
          // The src range is completely contained in it, we cast it to destination and insert the remainder
          range left = range{it->begin, src_range.begin};
          range right = range{it->end, src_range.end};
          range_merge_insert(new_seeds, left);
          range_merge_insert(new_seeds, dest_range);
          range_merge_insert(new_seeds, right);
          logln("inserting the split range (3 way) ", left, ", ", src_range, "(cast to ", dest_range, ") and ", right);

          unstable_erase(seeds, it);

        } else if (src_range.begin <= it->end && it->end <= src_range.end) {
          // We need to map [src_range.begin..it->begin] and send the rest as is
          range left = {it->begin, src_range.begin};
          range right = {src_range.begin, it->end};
          range new_range = range{dest_range.begin, dest_range.begin + right.size()};
          logln("inserting the split range ", left, " and ", right, " (cast to ", new_range, ")");
          range_merge_insert(new_seeds, left);
          // No offset in this case, since the range to cast started at src_range
          range_merge_insert(new_seeds, new_range) ;
          unstable_erase(seeds, it);
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
