#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include "utils.hpp"

using namespace std;

using large_int = uint64_t;

large_int COMMON_DIVISOR = 1;

struct monkey {
  template <class... Items>
  monkey(function<large_int(large_int)> op, int div, int iftrue, int iffalse,
         Items... starters)
      : op{op}, divisor(div), iftrue(iftrue), iffalse(iffalse) {
    (objects.push(starters), ...);
  }

  void monkey_turn(monkey *monkeys) {
    while (!objects.empty()) {
      large_int obj = objects.front();
      objects.pop();
      inspection++;
      obj = op(obj);
      if (obj % divisor == 0) {
        monkeys[iftrue].catch_obj(manage_stress(obj));
      } else {
        monkeys[iffalse].catch_obj(manage_stress(obj));
      }
    }
  }

  large_int manage_stress(large_int obj) {
    return obj % COMMON_DIVISOR;
  }

  void catch_obj(large_int obj) { objects.push(obj); }

  large_int inspection = 0;

  large_int iftrue;
  large_int iffalse;
  large_int divisor = 1;
  queue<large_int> objects = {};
  function<large_int(large_int)> op;
};

struct times {
  large_int v;
  times(large_int v) : v(v) {}
  large_int operator()(large_int old) { return v * old; }
};

struct add {
  large_int v;
  add(large_int v) : v(v) {}
  large_int operator()(large_int old) { return v + old; }
};

struct square {
  large_int operator()(large_int old) { return old * old; }
};


DPSG_AOC_MAIN(file) {
  constexpr uint64_t MONKEYS = 8;
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

  for (int i = 0; i < MONKEYS; ++i) {
    COMMON_DIVISOR *= monkeys[i].divisor;
  }

  for (uint64_t i = 0; i < 10000; ++i) {
    for (uint64_t j = 0; j < MONKEYS; ++j) {
      monkeys[j].monkey_turn(monkeys);
    }
    for (int r : {1, 20, 1000, 2000, 5000, 7000, 10000}) {
      if (i+1 == r) {
        cout << "turn " << i+1 << endl;
        for (uint64_t k = 0; k < MONKEYS; ++k) {
          cout << "Monkey " << k << " inspected items " << monkeys[k].inspection
               << " times" << endl;
        }
      }
    }
  }

  sort(monkeys, monkeys + MONKEYS, [](const monkey &m1, const monkey &m2) {
    return m1.inspection > m2.inspection;
  });
  cout << monkeys[0].inspection * monkeys[1].inspection << endl;
}
