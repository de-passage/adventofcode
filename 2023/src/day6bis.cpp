#include "utils.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

template<class ...F>
struct overload_set : F... {
  using F::operator()...;
};
template<class ...F>
overload_set(F&&...) -> overload_set<std::decay_t<F>...>;

int main(int argc, const char** argv) {
  using namespace std;
  using namespace dpsg;
  auto file = get_input("day6.txt", argc, argv);

  vector<int> times;
  vector<int> distances;

  string line;
  getline(file, line);
  for (auto t = next_number(line); t; t = next_number(line, t->position.end)) {
    times.push_back(t->value);
  }
  getline(file, line);
  for (auto d = next_number(line); d; d = next_number(line, d->position.end)) {
    distances.push_back(d->value);
  }

  auto time = accumulate(times.begin(), times.end(), 0uz, combine_ints);
  auto best_distance = accumulate(distances.begin(), distances.end(), 0uz, combine_ints);

  auto tsqu = time * time;
  auto sq = sqrt(tsqu - (4*(best_distance)));
  auto left = floor((time + sq) / 2);
  auto right = floor((time - sq) / 2);

  auto nb = left > right ? (left - right) : right - left;

  cout << "beaten: " << size_t(nb) << endl;
}
