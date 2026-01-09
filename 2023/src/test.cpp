#include "day12.hpp"
#include "poker.hpp"
#include "symetry.hpp"
#include "utils.hpp"

#define TEST(what, op, ref)                                                    \
  {                                                                            \
    auto what_ = (what);                                                       \
    if (what_ op ref) {                                                        \
      log_(cout, "Test passed: ");                                             \
      cout << " ";                                                             \
      log_(cout, what_);                                                       \
      cout << " ";                                                             \
      log_(cout, #op);                                                         \
      cout << " ";                                                             \
      log_(cout, ref);                                                         \
      cout << std::endl;                                                       \
    } else {                                                                   \
      cout << "Test failed (" << #what << ' ' << #op << ' ' << #ref << "):  "; \
      log_(cout, what_);                                                       \
      cout << " ";                                                             \
      log_(cout, #op);                                                         \
      cout << " ";                                                             \
      log_(cout, ref);                                                         \
      cout << std::endl;                                                       \
      exit(1);                                                                 \
    }                                                                          \
  }

#define LOG(what)                                                              \
  std::cout << #what << std::endl;                                             \
  what;

using namespace std;
using namespace dpsg;

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
  TEST(ranges_actual, ==, make_vec(range{1, 3}, range{4, 5}, range{7, 10}));

  LOG(range_merge_insert(ranges_actual, range{8, 15}));
  TEST(ranges_actual, ==, make_vec(range{1, 3}, range{4, 5}, range{7, 15}));

  LOG(range_merge_insert(ranges_actual, range{2, 8}));
  TEST(ranges_actual, ==, make_vec(range{1, 15}));

  LOG(range_merge_insert(ranges_actual, range{9, 10}));
  TEST(ranges_actual, ==, make_vec(range{1, 15}));

  LOG(range_merge_insert(ranges_actual, range{0, 0}));
  TEST(ranges_actual, ==, make_vec(range{1, 15}));

  LOG(range_merge_insert(ranges_actual, range{17, 16}));
  TEST(ranges_actual, ==, make_vec(range{1, 15}));

  LOG(range_merge_insert(ranges_actual, range{0, 19}));
  TEST(ranges_actual, ==, make_vec(range{0, 19}));

  LOG(range_merge_insert(ranges_actual, range{30, 40}));
  TEST(ranges_actual, ==, make_vec(range{0, 19}, range{30, 40}));

  LOG(range_merge_insert(ranges_actual, range{50, 60}));
  TEST(ranges_actual, ==, make_vec(range{0, 19}, range{30, 40}, range{50, 60}));

  LOG(range_merge_insert(ranges_actual, range{70, 80}));
  TEST(ranges_actual, ==,
       make_vec(range{0, 19}, range{30, 40}, range{50, 60}, range{70, 80}));

  LOG(range_merge_insert(ranges_actual, range{20, 25}));
  TEST(ranges_actual, ==,
       make_vec(range{0, 19}, range{20, 25}, range{30, 40}, range{50, 60},
                range{70, 80}));

  LOG(range_merge_insert(ranges_actual, range{100, 101}));
  TEST(ranges_actual, ==,
       make_vec(range{0, 19}, range{20, 25}, range{30, 40}, range{50, 60},
                range{70, 80}, range{100, 101}));

  LOG(range_merge_insert(ranges_actual, range{69, 70}));
  TEST(ranges_actual, ==,
       make_vec(range{0, 19}, range{20, 25}, range{30, 40}, range{50, 60},
                range{69, 80}, range{100, 101}));

  LOG(range_merge_insert(ranges_actual, range{24, 69}));
  TEST(ranges_actual, ==,
       make_vec(range{0, 19}, range{20, 80}, range{100, 101}));
}

void combine_ints_test() {
  TEST(combine_ints(1, 2), ==, 12);
  TEST(combine_ints(10, 2), ==, 102);
  TEST(combine_ints(1, 20), ==, 120);
}

void poker__make_hand_test() {
  using namespace poker_1;
  LOG(hand h = make_hand((int[5]){1, 2, 3, 4, 5}));
  TEST(h.cards[0].value, ==, 5);
  TEST(h.cards[1].value, ==, 4);
  TEST(h.cards[2].value, ==, 3);
  TEST(h.cards[3].value, ==, 2);
  TEST(h.cards[4].value, ==, 1);
  TEST(h.cards[0].quantity, ==, 1);
  TEST(h.cards[1].quantity, ==, 1);
  TEST(h.cards[2].quantity, ==, 1);
  TEST(h.cards[3].quantity, ==, 1);
  TEST(h.cards[4].quantity, ==, 1);

  LOG(h = make_hand((int[5]){1, 1, 1, 1, 1}));
  TEST(h.cards[0].value, ==, 1);
  TEST(h.cards[1].value, ==, 0);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);
  TEST(h.cards[0].quantity, ==, 5);
  TEST(h.cards[1].quantity, ==, 0);
  TEST(h.cards[2].quantity, ==, 0);
  TEST(h.cards[3].quantity, ==, 0);
  TEST(h.cards[4].quantity, ==, 0);

  LOG(h = make_hand((int[5]){1, 1, 1, 1, 2}));
  TEST(h.cards[0].value, ==, 1);
  TEST(h.cards[1].value, ==, 2);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);
  TEST(h.cards[0].quantity, ==, 4);
  TEST(h.cards[1].quantity, ==, 1);
  TEST(h.cards[2].quantity, ==, 0);
  TEST(h.cards[3].quantity, ==, 0);
  TEST(h.cards[4].quantity, ==, 0);

  LOG(h = make_hand((int[5]){2, 1, 1, 2, 1}));
  TEST(h.cards[0].value, ==, 1);
  TEST(h.cards[1].value, ==, 2);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);
  TEST(h.cards[0].quantity, ==, 3);
  TEST(h.cards[1].quantity, ==, 2);
  TEST(h.cards[2].quantity, ==, 0);
  TEST(h.cards[3].quantity, ==, 0);
  TEST(h.cards[4].quantity, ==, 0);

  LOG(h = make_hand((int[5]){1, 2, 2, 2, 1}));
  TEST(h.cards[0].value, ==, 2);
  TEST(h.cards[1].value, ==, 1);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);
  TEST(h.cards[0].quantity, ==, 3);
  TEST(h.cards[1].quantity, ==, 2);
  TEST(h.cards[2].quantity, ==, 0);
  TEST(h.cards[3].quantity, ==, 0);
  TEST(h.cards[4].quantity, ==, 0);

  LOG(h = make_hand((int[5]){1, 2, 3, 2, 1}));
  TEST(h.cards[0].value, ==, 2);
  TEST(h.cards[1].value, ==, 1);
  TEST(h.cards[2].value, ==, 3);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);
  TEST(h.cards[0].quantity, ==, 2);
  TEST(h.cards[1].quantity, ==, 2);
  TEST(h.cards[2].quantity, ==, 1);
  TEST(h.cards[3].quantity, ==, 0);
  TEST(h.cards[4].quantity, ==, 0);
}

void poker__parse_hand_test() {
  using namespace poker_1;
  hand h = parse_hand("12345");
  TEST(h, ==, make_hand((int[5]){1, 2, 3, 4, 5}));
  TEST(h.cards[0].value, ==, 5);
  TEST(h.cards[1].value, ==, 4);
  TEST(h.cards[2].value, ==, 3);
  TEST(h.cards[3].value, ==, 2);
  TEST(h.cards[4].value, ==, 1);

  h = parse_hand("11111");
  TEST(h, ==, make_hand((int[5]){1, 1, 1, 1, 1}));
  TEST(h.cards[0].value, ==, 1);
  TEST(h.cards[1].value, ==, 0);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);
  TEST(h.cards[0].quantity, ==, 5);
  TEST(h.cards[1].quantity, ==, 0);
  TEST(h.cards[2].quantity, ==, 0);
  TEST(h.cards[3].quantity, ==, 0);
  TEST(h.cards[4].quantity, ==, 0);

  h = parse_hand("11112");
  TEST(h, ==, make_hand((int[5]){1, 1, 1, 1, 2}));
  TEST(h.cards[0].value, ==, 1);
  TEST(h.cards[1].value, ==, 2);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);

  h = parse_hand("21121");
  TEST(h, ==, make_hand((int[5]){2, 1, 1, 1, 2}));
  TEST(h.cards[0].value, ==, 1);
  TEST(h.cards[1].value, ==, 2);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);

  h = parse_hand("12221");
  TEST(h, ==, make_hand((int[5]){2, 1, 2, 1, 2}));
  TEST(h.cards[0].value, ==, 2);
  TEST(h.cards[1].value, ==, 1);
  TEST(h.cards[2].value, ==, 0);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);

  h = parse_hand("12321");
  TEST(h, ==, make_hand((int[5]){2, 1, 2, 1, 3}));
  TEST(h.cards[0].value, ==, 2);
  TEST(h.cards[1].value, ==, 1);
  TEST(h.cards[2].value, ==, 3);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);

  h = parse_hand("TQJKA");
  TEST(h.cards[0].value, ==, 14);
  TEST(h.cards[1].value, ==, 13);
  TEST(h.cards[2].value, ==, 12);
  TEST(h.cards[3].value, ==, 11);
  TEST(h.cards[4].value, ==, 10);
  TEST(h.cards[0].quantity, ==, 1);
  TEST(h.cards[1].quantity, ==, 1);
  TEST(h.cards[2].quantity, ==, 1);
  TEST(h.cards[3].quantity, ==, 1);
  TEST(h.cards[4].quantity, ==, 1);

  h = parse_hand("KK776");
  TEST(h.cards[0].value, ==, 13);
  TEST(h.cards[1].value, ==, 7);
  TEST(h.cards[2].value, ==, 6);
  TEST(h.cards[3].value, ==, 0);
  TEST(h.cards[4].value, ==, 0);
  TEST(h.cards[0].quantity, ==, 2);
  TEST(h.cards[1].quantity, ==, 2);
  TEST(h.cards[2].quantity, ==, 1);
  TEST(h.cards[3].quantity, ==, 0);
  TEST(h.cards[4].quantity, ==, 0);
}

void poker__ordering_test() {
  using namespace poker_1;
  TEST(parse_hand("11111"), >, parse_hand("11112"));
  TEST(parse_hand("11112"), <, parse_hand("11113"));
  TEST(parse_hand("11122"), <, parse_hand("11111"));
  TEST(parse_hand("11122"), <, parse_hand("11112"));
  TEST(parse_hand("11123"), <, parse_hand("11122"));
  TEST(parse_hand("15122"), <, parse_hand("15111"));
  TEST(parse_hand("54321"), >, parse_hand("23456"));
  TEST(parse_hand("QQQQA"), <, parse_hand("QKKKK"));
  TEST(parse_hand("JJQQA"), <, parse_hand("QQKKA"));
  TEST(parse_hand("JJJQA"), <, parse_hand("JJJAA"));
  TEST(parse_hand("JJJQA"), <, parse_hand("22211"));
  TEST(parse_hand("KK776"), <, parse_hand("555JT"));
  TEST(parse_hand("JJTTK"), <, parse_hand("KK776"));
  TEST(parse_hand("JJTTK") > parse_hand("KK776"), ==, false);
  TEST(parse_hand("KK776") > parse_hand("555JT"), ==, false);
  TEST(parse_hand("KK677"), >, parse_hand("KTJJT"));
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
  TEST(result4.right.size(), ==, 0);
  TEST(result4.extruded, ==, (range{5, 10}));

  LOG(auto result5 = extrude(range{0, 10}, range{15, 20}));
  TEST(result5.left.size(), ==, 0);
  TEST(result5.right.size(), ==, 0);
  TEST(result5.extruded.size(), ==, 0);

  LOG(auto result6 = extrude(range{5, 10}, range{0, 15}));
  TEST(result6.left.size(), ==, 0);
  TEST(result6.right.size(), ==, 0);
  TEST(result6.extruded, ==, (range{5, 10}));
}

size_t compute_day12(string s) {
  auto [states, groups] = process_input_line(s);
  auto p = count_possibilities(states, groups);
  return p;
}

size_t compute_day12bis(string s) {
  auto [states, groups] = process_input_line(s);
  std::vector<State> many_states;
  std::vector<int> many_groups;
  for (int i = 0; i < 5; ++i) {
    if (i > 0) {
      many_states.push_back(State::Unknown);
    }
    many_states.insert(many_states.end(), states.begin(), states.end());
    many_groups.insert(many_groups.end(), groups.begin(), groups.end());
  }
  auto p = count_possibilities(many_states, many_groups);
  return p;
}

void day12_test() {
  TEST(compute_day12(" 0"), ==, 1);
  TEST(compute_day12("? 1"), ==, 1);
  TEST(compute_day12("? "), ==, 1);
  TEST(compute_day12("#.#.### 1,1,3"), ==, 1);
  TEST(compute_day12("???.### 1,1,3"), ==, 1);
  TEST(compute_day12(".??..??...?##. 1,1,3"), ==, 4);
  TEST(compute_day12("?#?#?#?#?#?#?#? 1,3,1,6"), ==, 1);
  TEST(compute_day12("????.#...#... 4,1,1"), ==, 1);
  TEST(compute_day12("????.######..#####. 1,6,5"), ==, 4);
  TEST(compute_day12("?###???????? 3,2,1"), ==, 10);

  TEST(compute_day12bis("???.### 1,1,3"), ==, 1);
  TEST(compute_day12bis(".??..??...?##. 1,1,3"), ==, 16384);
  TEST(compute_day12bis("?#?#?#?#?#?#?#? 1,3,1,6"), ==, 1);
  TEST(compute_day12bis("????.#...#... 4,1,1"), ==, 16);
  TEST(compute_day12bis("????.######..#####. 1,6,5"), ==, 2500);
  TEST(compute_day12bis("?###???????? 3,2,1"), ==, 506250);
  TEST(compute_day12bis(".???##?????????#?.?? 1,12"), ==, 2510);

  TEST(compute_day12bis(".???##?????????#?.?? 1,12"), ==, 2510);
  TEST(compute_day12bis("#####??.?????? 7,1"), ==, 371446);
  TEST(compute_day12bis("#.???#???.#? 1,2,1,1"), ==, 9904);
  TEST(compute_day12bis("?#???#?..???# 2,1,1,1"), ==, 7232);
  TEST(compute_day12bis("?#..???.#?.?.. 1,2,2,1"), ==, 512);
  TEST(compute_day12bis("?#?????#?#??#?????# 2,1,8,1,1"), ==, 650566);
  TEST(compute_day12bis("?#???????? 1,4,1"), ==, 35087);
  TEST(compute_day12bis("#???#??#?##.???? 1,1,7,1,1"), ==, 243);
  TEST(compute_day12bis("?#?.#???#?###??? 2,3,8"), ==, 32);
  TEST(compute_day12bis(".??#????#. 3,1"), ==, 243);
  TEST(compute_day12bis("??#.??#??#?? 1,1,2,1"), ==, 35628);
  TEST(compute_day12bis("?#??#?#??.?.#.#?? 8,1,1,3"), ==, 32);
  TEST(compute_day12bis("????###??#.???#..?? 7,3,1"), ==, 1250);
  TEST(compute_day12bis(".##????.?? 3,2,1"), ==, 162);
  TEST(compute_day12bis("#.#???#?##??.????.? 1,2,7,1,2"), ==, 1);
  TEST(compute_day12bis(".#?.??#?????.??.?? 2,8,1"), ==, 2500);
  TEST(compute_day12bis("#???..?.?. 3,1"), ==, 1250);
  TEST(compute_day12bis("????#?#???..??? 1,6"), ==, 2083456);
  TEST(compute_day12bis("?????#???.??.???? 6,1,1,1,2"), ==, 60000);
  TEST(compute_day12bis("?..?.#????#?###?#? 1,1,1,10"), ==, 162);
  TEST(compute_day12bis(".#??#?????#. 4,5"), ==, 1);
  TEST(compute_day12bis(".?????????. 1,4"), ==, 2050816);
  TEST(compute_day12bis(".??.?.#?#??. 1,4"), ==, 768);
  TEST(compute_day12bis("??##???.#???#??.? 5,3,2,1"), ==, 3888);
  TEST(compute_day12bis("?#????#??.? 1,4,1"), ==, 24802);
  TEST(compute_day12bis("#??#.??###???. 4,5,1"), ==, 243);
  TEST(compute_day12bis("?#?.#??.???# 3,2,3"), ==, 1);
  TEST(compute_day12bis("?#??.???#??? 1,1,5"), ==, 33512);
  TEST(compute_day12bis("???.#????.? 2,4,1"), ==, 162);
  TEST(compute_day12bis("???#?.#??#???#?.?# 1,1,1,1,3,1"), ==, 1024);
  TEST(compute_day12bis("???.#???.?#???? 1,1,3,2,1"), ==, 192080);
  TEST(compute_day12bis(".#??##????. 5,2"), ==, 32);
  TEST(compute_day12bis("?????#???????.??? 3,4,1,1,3"), ==, 6480);
  TEST(compute_day12bis("?...???####????#? 1,13"), ==, 1);
  TEST(compute_day12bis(".?##?#????????#???. 6,6"), ==, 32768);
  TEST(compute_day12bis("???.?#???? 2,3,1"), ==, 39366);
  TEST(compute_day12bis(".????????#???? 5,3,1"), ==, 982339);
  TEST(compute_day12bis("..?#??..??.?? 4,1,1"), ==, 9604);
  TEST(compute_day12bis(".?#???.?#? 2,1,1"), ==, 2505);
  TEST(compute_day12bis("#??..???????#? 2,1,5"), ==, 22143);
}

void symetry_test() {
  TEST(lowest_symetric_index(make_vec(1, 2, 3, 4, 5)), ==, 0);
  TEST(lowest_symetric_index(make_vec(1, 2, 3, 4, 4)), ==, 4);
  TEST(lowest_symetric_index(make_vec(1, 2, 3, 3, 4)), ==, 0);
  TEST(lowest_symetric_index(make_vec(1, 4, 3, 3, 4)), ==, 3);
  TEST(lowest_symetric_index(make_vec(1, 2, 2, 3, 4)), ==, 0);
  TEST(lowest_symetric_index(make_vec(1, 2, 2, 1, 4)), ==, 2);
  TEST(lowest_symetric_index(make_vec(1, 1, 2, 3, 4)), ==, 1);
  TEST(lowest_symetric_index(make_vec(4, 3, 2, 1, 1, 2, 3, 4)), ==, 4);
}

int main() {
  std::cout << std::boolalpha;
  // LOG(range_merge_insert_test());
  // LOG(combine_ints_test());
  // LOG(poker__make_hand_test());
  // LOG(poker__parse_hand_test());
  // LOG(poker__ordering_test());
  // LOG(extrude_test());
  // LOG(day12_test());
  LOG(symetry_test());
  return 0;
}
