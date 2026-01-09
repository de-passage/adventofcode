#include "utils.hpp"
#include "algorithms.hpp"
#include <sstream>

using namespace std;

int main(int argc, const char **argv) {
  auto file = get_input("day9.txt", argc, argv);
  string line;
  int acc = 0;
  while (getline(file, line)) {
    std::vector<int> numbers;
    int n = 0;
    std::istringstream iss(line);
    while (iss >> n) {
      numbers.push_back(n);
    }
    logln("Numbers: ", numbers);
    for (auto i = numbers.size() - 1; i > 0; --i) {
      for (auto j = i - 1; j < numbers.size() - 1; ++j) {
        logln(numbers);
        logln("Computing ", i, " - ", j, " => ", numbers[i], " - ", numbers[j], " = ", numbers[j + 1] - numbers[j]);
        numbers[j + 1] = numbers[j + 1] - numbers[j];
      }
    }
    logln("Result: ", numbers);

    int s = std::accumulate(numbers.rbegin(), numbers.rend(), 0, [](int acc, int n) { return n - acc; });
    acc += s;
    logln("Sum: ", s, "\tAcc: ", acc);
    logln();
  }
  std::cout << "Acc: " << acc << "\n" << endl;

  return 0;
}
