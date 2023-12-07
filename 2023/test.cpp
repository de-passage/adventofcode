#include "poker.hpp"
#include "utils.hpp"

#define TEST(what, op, ref)                                                    \
  if (what op ref) {                                                           \
    log_(cout, "Test passed: ");                                               \
    cout << " ";                                                               \
    log_(cout, what);                                                          \
    cout << " ";                                                               \
    log_(cout, #op);                                                           \
    cout << " ";                                                               \
    log_(cout, ref);                                                           \
    cout << std::endl;                                                         \
  } else {                                                                     \
    cout << "Test failed (" << #what << ' ' << #op << ' ' << #ref << "):";     \
    log_(cout, what);                                                          \
    cout << " ";                                                               \
    log_(cout, #op);                                                           \
    cout << " ";                                                               \
    log_(cout, ref);                                                           \
    cout << std::endl;                                                         \
    exit(1);                                                                   \
  }

#define LOG(what)                                                              \
  std::cout << #what << std::endl;                                             \
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
  TEST(ranges_actual, ==,
       make_vec(range{1, 3}, range{4, 5}, range{7, 8}, range{8, 10}));

  LOG(range_merge_insert(ranges_actual, range{8, 15}));
  TEST(ranges_actual, ==,
       make_vec(range{1, 3}, range{4, 5}, range{7, 8}, range{8, 15}));

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

void poker__make_hand_test() {
  LOG(hand h = make_hand((int[5]){1, 2, 3, 4, 5}));
  TEST(h.major, ==, 1);
  TEST(h.minor, ==, 1);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 2);
  TEST(h.cards[2], ==, 3);
  TEST(h.cards[3], ==, 4);
  TEST(h.cards[4], ==, 5);
  TEST(h.similarity[0], ==, 5);
  TEST(h.similarity[1], ==, 0);
  TEST(h.similarity[2], ==, 0);
  TEST(h.similarity[3], ==, 0);
  TEST(h.similarity[4], ==, 0);

  LOG(h = make_hand((int[5]){1, 1, 1, 1, 1}));
  TEST(h.major, ==, 5);
  TEST(h.minor, ==, 1);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 1);
  TEST(h.cards[3], ==, 1);
  TEST(h.cards[4], ==, 1);
  TEST(h.similarity[0], ==, 0);
  TEST(h.similarity[1], ==, 0);
  TEST(h.similarity[2], ==, 0);
  TEST(h.similarity[3], ==, 0);
  TEST(h.similarity[4], ==, 1);

  LOG(h = make_hand((int[5]){1, 1, 1, 1, 2}));
  TEST(h.major, ==, 4);
  TEST(h.minor, ==, 1);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 1);
  TEST(h.cards[3], ==, 1);
  TEST(h.cards[4], ==, 2);

  LOG(h = make_hand((int[5]){2, 1, 1, 2, 1}));
  TEST(h.major, ==, 3);
  TEST(h.minor, ==, 2);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 1);
  TEST(h.cards[3], ==, 2);
  TEST(h.cards[4], ==, 2);

  LOG(h = make_hand((int[5]){1, 2, 2, 2, 1}));
  TEST(h.major, ==, 3);
  TEST(h.minor, ==, 2);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 2);
  TEST(h.cards[3], ==, 2);
  TEST(h.cards[4], ==, 2);

  LOG(h = make_hand((int[5]){1, 2, 3, 2, 1}));
  TEST(h.major, ==, 2);
  TEST(h.minor, ==, 2);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 2);
  TEST(h.cards[3], ==, 2);
  TEST(h.cards[4], ==, 3);
}

void poker__parse_hand_test() {
  hand h = parse_hand("12345");
  TEST(h.major, ==, 1);
  TEST(h.minor, ==, 1);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 2);
  TEST(h.cards[2], ==, 3);
  TEST(h.cards[3], ==, 4);
  TEST(h.cards[4], ==, 5);

  h = parse_hand("11111");
  TEST(h.major, ==, 5);
  TEST(h.minor, ==, 1);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 1);
  TEST(h.cards[3], ==, 1);
  TEST(h.cards[4], ==, 1);

  h = parse_hand("11112");
  TEST(h.major, ==, 4);
  TEST(h.minor, ==, 1);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 1);
  TEST(h.cards[3], ==, 1);
  TEST(h.cards[4], ==, 2);

  h = parse_hand("21121");
  TEST(h.major, ==, 3);
  TEST(h.minor, ==, 2);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 1);
  TEST(h.cards[3], ==, 2);
  TEST(h.cards[4], ==, 2);

  h = parse_hand("12221");
  TEST(h.major, ==, 3);
  TEST(h.minor, ==, 2);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 2);
  TEST(h.cards[3], ==, 2);
  TEST(h.cards[4], ==, 2);

  h = parse_hand("12321");
  TEST(h.major, ==, 2);
  TEST(h.minor, ==, 2);
  TEST(h.cards[0], ==, 1);
  TEST(h.cards[1], ==, 1);
  TEST(h.cards[2], ==, 2);
  TEST(h.cards[3], ==, 2);
  TEST(h.cards[4], ==, 3);

  h = parse_hand("TQJKA");
  TEST(h.major, ==, 1);
  TEST(h.minor, ==, 1);
  TEST(h.cards[0], ==, 10);
  TEST(h.cards[1], ==, 11);
  TEST(h.cards[2], ==, 12);
  TEST(h.cards[3], ==, 13);
  TEST(h.cards[4], ==, 14);
}

void poker__ordering_test() {
  TEST(parse_hand("11111"), >, parse_hand("11112"));
  TEST(parse_hand("11112"), <, parse_hand("11113"));
  TEST(parse_hand("11122"), <, parse_hand("11111"));
  TEST(parse_hand("11122"), <, parse_hand("11112"));
  TEST(parse_hand("11123"), <, parse_hand("11122"));
  TEST(parse_hand("15122"), <, parse_hand("15111"));
  TEST(parse_hand("54321"), <, parse_hand("23456"));
  TEST(parse_hand("QQQQA"), <, parse_hand("QKKKK"));
}


void extrude_test() {
  LOG(auto result1 = extrude(range{0, 10}, range{0, 10}));
  TEST(result1.left, ==, (range{0, 0}));
  TEST(result1.right, ==, (range{10, 10}));
  TEST(result1.extruded, ==, (range{0, 10}));

  LOG(auto result2 = extrude(range{0, 10}, range{0, 5}));
  TEST(result2.left, ==, (range{0, 0}));
  TEST(result2.right, ==, (range{5, 10}));
  TEST(result2.extruded, ==, (range{0, 5}));

  LOG(auto result3 = extrude(range{0, 10}, range{5, 10}));
  TEST(result3.left, ==, (range{0, 5}));
  TEST(result3.right, ==, (range{10, 10}));
  TEST(result3.extruded, ==, (range{5, 10}));

  LOG(auto result4 = extrude(range{0, 10}, range{5, 15}));
  TEST(result4.left, ==, (range{0, 5}));
  TEST(result4.right, ==, (range{10, 15}));

  LOG(auto result5 = extrude(range{0, 10}, range{15, 20}));
  TEST(result5.left, ==, (range{0, 0}));
  TEST(result5.right, ==, (range{10, 10}));
  TEST(result5.extruded, ==, (range{0, 0}));

  LOG(auto result6 = extrude(range{5, 10}, range{0, 15}));
  TEST(result6.left, ==, (range{5, 5}));
  TEST(result6.right, ==, (range{10, 10}));
  TEST(result6.extruded, ==, (range{5, 10}));

}

int main() {
  LOG(range_merge_insert_test());
  LOG(combine_ints_test());
  LOG(poker__make_hand_test());
  LOG(poker__parse_hand_test());
  LOG(poker__ordering_test());
  LOG(extrude_test());
  return 0;
}
