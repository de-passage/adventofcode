#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include "utils.hpp"

using namespace std;

struct monkey {
  template <class... Items>
  monkey(function<int(int)> op, int div, int iftrue, int iffalse,
         Items... starters)
      : op{op}, divisor(div), iftrue(iftrue), iffalse(iffalse) {
    (objects.push(starters), ...);
  }

  void monkey_turn(monkey* monkeys) {
    while(!objects.empty()) {
      int obj = objects.front();
      objects.pop();
      inspection++;
      obj = op(obj) / 3;
      if (obj % divisor == 0) {
        monkeys[iftrue].catch_obj(obj);
      } else {
        monkeys[iffalse].catch_obj(obj);
      }
    }
  }

  void catch_obj(int obj) {
    objects.push(obj);
  }

  int inspection = 0;


  int iftrue;
  int iffalse;
  int divisor = 1;
  queue<int> objects = {};
  function<int(int)> op;
};

struct times {
  int v;
  times(int v) : v(v) {}
  int operator()(int old) { return v * old; }
};

struct add {
  int v;
  add(int v) : v(v) {}
  int operator()(int old) { return v + old; }
};

struct square {
  int operator()(int old) { return old * old; }
};

DPSG_AOC_MAIN(file) {
  constexpr int MONKEYS = 8;
  monkey monkeys[MONKEYS] = {
    {times(3), 13, 1, 7, 84, 72, 58, 51},
    {add(8), 2, 7, 5, 88, 58, 58},
    {square(), 7, 3, 4, 93, 82, 71, 77, 83, 53, 71, 89},
    {add(2), 17, 4, 6, 81, 68, 65, 81, 73, 77, 96},
    {add(3), 5, 6, 0, 75, 80, 50, 73, 88},
    {times(17), 11, 2, 3, 59, 72, 99, 87, 91, 81},
    {add(6), 3, 1, 0, 86, 69},
    {add(1), 19, 2, 5, 91}
  };

  for(int i = 0; i < 20; ++i) {
    for (int i = 0; i < MONKEYS; ++i) {
      monkeys[i].monkey_turn(monkeys);
    }
  }

  for (int i = 0; i < MONKEYS; ++i) {
    cout << "Monkey " << i << " inspected items " << monkeys[i].inspection << " times" << endl;
  }

  sort(monkeys, monkeys + MONKEYS, [] (const monkey& m1, const monkey& m2) {
          return m1.inspection > m2.inspection;
       });
  cout << monkeys[0].inspection * monkeys[1].inspection << endl;
}
