#include "utils.hpp"
#include <numeric>

#include "day15.hpp"

using namespace std;
using namespace dpsg;

struct hash_map {
  struct entry {
    std::string_view key;
    u64 value;
  };
  std::vector<entry> data[256];

  void insert(std::string_view key, u64 val) {
    auto h = dpsg::hash(key);

    auto it = std::find_if(data[h].begin(), data[h].end(),
                           [&](auto &e) { return e.key == key; });
    if (it != data[h].end()) {
      logln("updating ", key, " in bucket ", h, " -> ", data[h]);
      it->value = val;
    } else {
      logln("inserting ", key , " into bucket ", h , " -> ", data[h]);
      data[h].push_back({key, val});
    }
  }

  void erase(std::string_view key) {
    auto h = dpsg::hash(key);
    auto &bucket = data[h];
    log("erasing ", key, " from bucket ", h, " ", bucket ," -> ");
    auto it = std::find_if(bucket.begin(), bucket.end(),
                           [&](auto &e) { return e.key == key; });
    if (it != bucket.end()) {
      logln("ok");
      bucket.erase(it);
    } else {
      logln("not found");
    }
  }

  u64 compute_value() {
    u64 acc = 0;
    for (auto i = 0uz; i < 256; ++i) {
      auto &bucket = data[i];
      for (auto j = 0uz; j < bucket.size(); ++j) {
        logln("Adding ", bucket[j].key, " (", i, ',', j, ") to count.");
        acc += (i + 1) * (j + 1) * bucket[j].value;
      }
    }
    return acc;
  }
};

std::ostream& operator<<(std::ostream& out, const hash_map::entry& e) {
  return out << '[' << e.key << ':' << e.value << ']';
}

std::ostream& operator<<(std::ostream& out, const hash_map& m) {
  out << "{\n";
  for (auto i = 0uz; i < size(m.data); ++i) {
    auto& v = m.data[i];
    if (!v.empty()) {
      out << "B(" << i << ") ";
      dpsg::log_(out, v);
      out << '\n';
    }
  }
  return out << "}\n";
}
DPSG_AOC_MAIN(file) {

  vector<int> data;
  string line;
  hash_map map;
  while (getline(file, line)) {
    auto tokens = tokenizer{line, ','};
    for (auto t : tokens) {
      auto pos = t.find_first_of("=-");
      if (pos == std::string_view::npos) {
        std::cerr << "Invalid input: " << t << std::endl;
        continue;
      }
      if (t[pos] == '=') {
        auto val = next_number(t, pos + 1);
        if (!val) {
          std::cerr << "Invalid input: " << t << std::endl;
          continue;
        }
        logln("inserting: ", t.substr(0, pos), " -> ", val->value);
        map.insert(t.substr(0, pos), val->value);
      } else {
        logln("erasing: ", t.substr(0, pos));
        map.erase(t.substr(0, pos));
      }

      logln(map);
    }
  }
  std::cout << map.compute_value() << std::endl;
}
