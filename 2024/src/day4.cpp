#include "utils.hpp"
#include <cassert>
#include "print.hpp"

int count(std::vector<std::string> &lines) {
  int sum = 0;
  for (auto line : lines) {
    //dpsg::println("{}", line);
    if (line.size() >= 4) {
      for (int i = 0; i < line.size() - 3; ++i) {
        auto sub = line.substr(i, 4);
        if (sub == "XMAS" || sub == "SAMX") {
          //dpsg::println("found {} at {}", sub, i);
          sum++;
        }
      }
    }
  }
  return sum;
}

DPSG_AOC_MAIN(file) {
  int sum = 0;

  std::vector<std::string> norm, transp, diag, diagr;

  for (auto line : dpsg::lines(file)) {
    if (transp.empty()) {
      transp.resize(line.size());
      diag.resize(line.size() * 2);
      diagr.resize(line.size() * 2);
    }
    norm.push_back(line);
    for (int i = 0; i < line.size(); ++i) {
      transp[i] += line[i];
    }
  }

  auto nc = count(norm);
  dpsg::println("norm {}", nc);

  auto tc = count(transp);
  dpsg::println("transp {}", tc);

  for (int i = 0; i < norm.size(); ++i) {
    for (int j = 0; j < norm[i].size(); ++j) {
      diag[i + norm.size() - (j + 1)] += norm[i][j];
      diagr[i + j] += norm[i][j];
    }
  }

  auto dc = count(diag);
  dpsg::println("diag {}", dc);


  auto rc = count(diagr);
  dpsg::println("diagr {}", rc);

  dpsg::println("{}", nc + rc + dc + tc);
}
