#include "utils.hpp"
#include <cmath>


int main(int argc, const char** argv) {
  using namespace std;
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

  int product = 1;

  for (size_t i = 0; i < times.size(); i++) {
    auto time = times[i];
    auto best_distance = distances[i];

    auto tsqu = time * time;
    auto sq = sqrt(tsqu - (4*(best_distance)));
    auto left = floor((time + sq) / 2);
    auto right = floor((time - sq) / 2);

    auto nb = left > right ? (left - right) : right - left;
    product *= nb;
  }

  cout << "product: " << product << endl;
}
