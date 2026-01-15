#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"

using namespace std;

DPSG_AOC_MAIN(file) {
  string line;
  vector<vector<int>> sizes;
  while(getline(file, line)) {
    sizes.emplace_back();
    for (auto c : line) sizes.back().push_back(c - '0');
  }

  int best =0;

  int w = sizes.size();
  for (int i = 0; i < w; ++i) {
    int h = sizes[i].size();
    for (int j = 0; j < h; ++j) {
      int current=sizes[i][j];

      int scenic_score=1;
      int distance=0;

      for (int l = i + 1; l < h; ++l) {
        int compare=sizes[l][j];
        distance++;
        if (compare >= current) {
          break;
        }
      }
      scenic_score*=distance;
      distance=0;
      for (int l = i - 1; l >= 0; --l) {
        int compare=sizes[l][j];
        distance++;
        if (compare >= current) {
          break;
        }
      }
      scenic_score*=distance;
      distance=0;
      for (int l = j + 1; l < w; ++l) {
        int compare=sizes[i][l];
        distance++;
        if (compare >= current) {
          break;
        }
      }
      scenic_score*=distance;
      distance=0;
      for (int l = j - 1; l >= 0; --l) {
        int compare=sizes[i][l];
        distance++;
        if (compare >= current) {
          break;
        }
      }
      scenic_score*=distance;
      if (scenic_score > best) {
        best =scenic_score;
      }
    }
  }
  cout << best << endl;
}
