#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <stack>

using namespace std;

void printstack(vector<stack<char>>& ss) {
  vector<stack<char>> ss2;
  for (auto& s1 : ss) {
    ss2.emplace_back();
    auto& s2 = ss2.back();
    while(!s1.empty()) {
      s2.push(s1.top());
      s1.pop();
    }
    while(!s2.empty()) {
      cerr << s2.top();
      s1.push(s2.top());
      s2.pop();
    }
    cerr << endl;
  }
}


int main(int argc, const char** argv) {
  if (argc <= 1) {
    cerr << "Gimme a file pls" << endl;
    return 1;
  }
  std::string filename=argv[1];
  ifstream file(filename);
  if (!file) {
    cerr << "Not a good file this " << filename << endl;
    return 1;
  }
  string line;
  int count = 0;


  vector<string> drawing;
  while(getline(file, line)) {
    for (char c : line) {
      if (c == '[') {
        // this is a box
        drawing.emplace_back(move(line));
        goto next;
      }
      if (c >= '0' && c <= '9') {
        // this is the line with the numbers
        goto done;
      }
    }
  next:;
  }
  done:


  vector<stack<char>> stacks;
  for(int i = 0; i < line.size(); ++i) {
    if(line[i] != ' ') {
      stacks.emplace_back();
      for (int j = drawing.size() - 1; j >= 0; --j) {
        if (drawing[j].size() <= i || drawing[j][i] == ' ') {
          break;
        }
        stacks.back().push(drawing[j][i]);
      }
    }
  }

//  for(auto& s : stacks) {
//    while(!s.empty()) {
//      cerr << s.top();
//      s.pop();
//    }
//    cerr << endl;
//  }

  getline(file, line);
  // printstack(stacks);
  while(getline(file, line)) {
    int nb_idx = line.find_first_of("0123456789");
    int end_nb_idx = line.find_first_of(' ', nb_idx);
    int nb = 0;
    while (nb_idx < end_nb_idx) {
      nb = nb*10 + line[nb_idx++] - '0';
    }
    int from_idx = line.find_first_of("0123456789", end_nb_idx);
    int to_idx = line.find_first_of("0123456789", from_idx + 1);
    // cerr << line << endl;
    int from = line[from_idx] - '0' - 1;
    int to = line[to_idx] - '0' - 1;
    // cerr << nb << " " << from << " " << to << endl;

    vector<char> crates;
    for (int i = 0; i < nb; ++i) {
      crates.push_back(stacks[from].top());
      stacks[from].pop();
    }
    for (int i = crates.size() - 1; i >= 0; --i) {
      char crate = crates[i];
      stacks[to].push(crate);
    }

    // printstack(stacks);
  }

    for (auto& s : stacks) {
      cout << s.top();
    }
    cout << endl;
}

