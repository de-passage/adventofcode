#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <numeric>
#include <limits>
#include "utils.hpp"

using namespace std;
using dir_ptr=unique_ptr<struct directory>;
using dir_list=unordered_map<string, dir_ptr>;
using file_list=vector<struct file>;

struct file {
  int size = 0;
  string name;

  file(string name, int size) : size(size), name(move(name)) {}
};

constexpr long long MAX_SIZE=100000;
constexpr long long TOTAL=70000000;
constexpr long long REQUIRED=30000000;

struct directory {
  long long _size = -1;
  long long _file_size = -1;
  dir_list directories;
  file_list files;
  directory* parent;
  directory(directory* parent) : parent{parent} {}

  directory* add_directory(string s) {
    return directories.emplace(s, make_unique<directory>(this)).first->second.get();
  }

  void add_file(string s, int i) {
    files.emplace_back(s,i);
  }

  directory* get_dir_ptr(const string& str) {
    return directories.find(str)->second.get();
  }

  long long size() {
    if (_size < 0) {
      _size = accumulate(directories.begin(), directories.end(), 0LL, [](long long acc, const auto& p) {
                   return acc + p.second->size();
                 });
    }
    if (_file_size < 0) {
      _file_size = accumulate(files.begin(), files.end(), 0LL, [](long long acc, const auto& p) {
                   return acc + p.size;
                 });

    }
    return _size + _file_size;
  }

  long long countSmall() {
    long long s = size();
    long long subSize = accumulate(directories.begin(), directories.end(), 0LL, [](long long acc, const auto& p) {
                 return acc + p.second->countSmall();
               });
//    cerr << "size: " << s << " | subsize: " << subSize << endl;
    return subSize + (s <= MAX_SIZE ? s : 0);
  }

  long long find_smallest(long long threshold, long long current_smallest = numeric_limits<long long>::max()) {
    if (size() < threshold) return current_smallest;

    current_smallest = min(size(), current_smallest);
    long long smallest = accumulate(directories.begin(), directories.end(), current_smallest, [threshold](long long acc, const auto& p) {
                                  long long s = p.second->find_smallest(threshold, acc);
                                      if (s < threshold) return acc;
                                      return min(acc, s);
                                });

    current_smallest = min(smallest, current_smallest);
    return current_smallest;
  }
};

enum class line_case {
  command,
  dir,
  file
};

line_case case_of(const string& str) {
  switch(str[0]) {
    case '$':
      return line_case::command;
    case 'd':
      return line_case::dir;
    default:
      return line_case::file;
  }
}

DPSG_AOC_MAIN(file) {
  string line;
  directory root(nullptr);
  directory* current_node = addressof(root);
  while(getline(file, line)) {
//    cerr << " ==== " << line <<" ==== " <<  endl;
    switch(case_of(line)) {
      case line_case::command: {
//        cerr << "command ";
        int beg=line.find_first_of(' ');
        int end=line.find_first_of(' ', beg+1);
        switch(line[beg+1]) {
          case 'c': {
//            cerr << " cd ";
            if (line[end+1] == '.') {
//              cerr << "<up>";
              current_node = current_node->parent;
            } else if (line[end+1] != '/'){
              string name=line.substr(end+1);
//              cerr << name;
              current_node = current_node->get_dir_ptr(move(name));
            }
            break;
          }
          default: {
//            cerr << "ls";
            // listing the directory
            break;
          }
        }
//        cerr << endl;
        break;
      }
      case line_case::dir: {
        int beg=line.find_first_of(' ');
        string name=line.substr(beg+1);
//        cerr << "new directory: " << name << endl;
        current_node->add_directory(move(name));
        break;
      }
      case line_case::file: {
        int sep = line.find_first_of(' ');
        string name=line.substr(sep+1);
        long size = std::stol(line.substr(0, sep));
//        cerr << "new file: " << name << " (" << size << ")" << endl;
        current_node->add_file(name, size);
        break;
      }
    }
  }

  // free space
  long long leftover = TOTAL - root.size();
  long long needed = REQUIRED - leftover;
  cout << root.find_smallest(needed) << endl;
}
