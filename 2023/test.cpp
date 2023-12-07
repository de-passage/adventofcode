#include "utils.hpp"

#define TEST(what, op, ref)                                                    \
  if (what op ref) {                                                           \
    log_(cout, "Test passed: ");                                               \
    cout << " ";                                                               \
    log_(cout, what);                                                          \
    cout << " ";                                                               \
    log_(cout, #op);                                                           \
    cout << " ";                                                               \
    log_(cout, ref);                                                          \
    cout << std::endl;                                                         \
  } else {                                                                     \
    log_(cout, "Test failed: ");                                               \
    log_(cout, what);                                                          \
    cout << " ";                                                               \
    log_(cout, #op);                                                           \
    cout << " ";                                                               \
    log_(cout, ref);                                                          \
    cout << std::endl;                                                         \
    exit(1);                                                                   \
  }

#define LOG(what) \
  std::cout << #what << std::endl;\
  what;

using namespace std;

template <typename T, typename... Ts> vector<T> make_vec(T arg, Ts... args) {
  vector<T> vec;
  vec.reserve(1 + sizeof...(args));
  vec.push_back(arg);
  (vec.push_back(args), ...);
  return vec;
}

void range_merge_insert_test() {
  LOG(vector<range> ranges_actual = make_vec(range{2, 3}));
  LOG(range_merge_insert(ranges_actual, range{2, 3}));
  TEST(ranges_actual, ==, make_vec(range{2, 3}));

  LOG(range_merge_insert(ranges_actual, range{4, 5}));
  TEST(ranges_actual, ==, make_vec(range{2, 3}, range{4, 5}));

  LOG(range_merge_insert(ranges_actual, range{8, 10}));
  TEST(ranges_actual, ==, make_vec(range{2, 3}, range{4, 5}, range{8, 10}));

  LOG(range_merge_insert(ranges_actual, range{1, 3}));
  TEST(ranges_actual, ==, make_vec(range{1, 3}, range{4, 5}, range{8, 10}));

  LOG(range_merge_insert(ranges_actual, range{7, 8}));
  TEST(ranges_actual, ==, make_vec(range{1, 3}, range{4, 5}, range{7, 8}, range{8, 10}));

  LOG(range_merge_insert(ranges_actual, range{8, 15}));
  TEST(ranges_actual, ==, make_vec(range{1, 3}, range{4, 5}, range{7, 8}, range{8, 15}));

  LOG(range_merge_insert(ranges_actual, range{2, 8}));
  TEST(ranges_actual, ==, make_vec(range{1, 8}, range{8, 15}));

  LOG(range_merge_insert(ranges_actual, range{9, 10}));
  TEST(ranges_actual, ==, make_vec(range{1, 8}, range{8, 15}));

  LOG(range_merge_insert(ranges_actual, range{0, 0}));
  TEST(ranges_actual, ==, make_vec(range{1, 8}, range{8, 15}));

  LOG(range_merge_insert(ranges_actual, range{17, 16}));
  TEST(ranges_actual, ==, make_vec(range{1, 8}, range{8, 15}));

  LOG(range_merge_insert(ranges_actual, range{0, 19}));
  TEST(ranges_actual, ==, make_vec(range{0, 19}));
}

void combine_ints_test() {
  TEST(combine_ints(1, 2), ==, 12);
  TEST(combine_ints(10, 2), ==, 102);
  TEST(combine_ints(1, 20), ==, 120);
}
int main() {
  range_merge_insert_test();
  combine_ints_test();
  return 0;
}
