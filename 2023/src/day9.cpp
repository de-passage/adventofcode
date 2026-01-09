#include "utils.hpp"
#include "algorithms.hpp"
#include <sstream>

using namespace std;

DPSG_AOC_MAIN(file) {
  string line;
  int acc = 0;
  while (getline(file, line)) {
    std::vector<int> numbers;
    int n = 0;
    std::istringstream iss(line);
    while (iss >> n) {
      numbers.push_back(n);
      for (auto i = numbers.size() - 1; i >= 1; --i) {
        numbers[i-1] = numbers[i] - numbers[i-1];
      }
    }
    logln("Result: ", numbers);

    int s = dpsg::sum(numbers);
    acc += s;
    logln("Sum: ", s, "\tAcc: ", acc);
  }
  std::cout << "Acc: " << acc << "\n" << endl;

  return 0;
}
