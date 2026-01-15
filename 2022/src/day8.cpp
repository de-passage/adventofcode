#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"

using namespace std;

DPSG_AOC_MAIN(file) {
  string line;
  //first line

  // list of visible summits in descending orders, from the bottom
  /* vector<vector<int>> col_descending_max;
  vector<int> col_max;

  int number_of_trees=line.size();
  for (int i = 0; i<line.size();++i) {
    int value=line[i];
    col_max.push_back(value);
    col_descending_max.emplace_back();
    col_descending_max[i].push_back(value);
  }
   */
  vector<vector<int>> sizes;
  while(getline(file, line)) {
    sizes.emplace_back();
    for (auto c : line) sizes.back().push_back(c - '0');
  }

  int count =0;

  for (int i = 0; i < sizes.size(); ++i) {
    for (int j = 0; j < sizes[i].size(); ++j) {
      int current=sizes[i][j];

      /* for (int k = 0; k < sizes.size(); ++k) {
        for (int l = 0; l < sizes[k].size(); ++l) {
          if (k != i || j != l)
//            cerr << ' ' << sizes[k][l] << ' ';
          else
//            cerr << '*' << sizes[k][l] << '*';
        }
//        cerr << endl;
      } */

//      cerr << "Inspecting " << i << "," << j << " (" << current << ")" << endl;
      bool visible=true;

      for (int l = 0; l < i; ++l) {
        int compare=sizes[l][j];
        if (compare >= current) {
//          cerr << "blocked by " << l << "," << j << " (" << compare << ")" << " from the top " << endl;
          visible=false;
          break;
        }
      }
      if (visible==true) {
        count++;
//        cerr << "Visible from the top !" << endl;
//      cerr << "----------------------" << endl;
        continue;
      }

      visible=true;
      for (int l = sizes.size() - 1; l > i; --l) {
        int compare=sizes[l][j];
        if (compare >= current) {
//          cerr << "blocked by " << l << "," << j << " (" << compare << ")" << " from the bottom " << endl;
          visible=false;
          break;
        }
      }
      if (visible==true) {
        count++;
//        cerr << "Visible from the bottom !" << endl;
//      cerr << "----------------------" << endl;
        continue;
      }

      visible=true;
      for (int l = 0; l < j; ++l) {
        int compare=sizes[i][l];
        if (compare >= current) {
//          cerr << "blocked by " << i << "," << l << " (" << compare << ")" << " from the left " << endl;
          visible=false;
          break;
        }
      }
      if (visible==true) {
        count++;
//        cerr << "Visible from the left !" << endl;
//      cerr << "----------------------" << endl;
        continue;
      }

      visible=true;
      for (int l = sizes[i].size() - 1; l > j; --l) {
        int compare=sizes[i][l];
        if (compare >= current) {
//          cerr << "blocked by " << i << "," << l << " (" << compare << ")" << " from the right " << endl;
          visible=false;
          break;
        }
      }
      if (visible==true) {
        count++;
//        cerr << "Visible from the right !" << endl;
//      cerr << "----------------------" << endl;
        continue;
      }
//      cerr << "Not visible !" << endl;
//      cerr << "----------------------" << endl;
    }
//    cerr << "====================" << endl;
  }
  cout << count << endl;
}
