#include "utils.hpp"

#include <memory>
#include <unordered_map>
#include <variant>

struct part_combination {
  int64_t x_min = 1;
  int64_t x_max = 4000;
  int64_t m_min = 1;
  int64_t m_max = 4000;
  int64_t a_min = 1;
  int64_t a_max = 4000;
  int64_t s_min = 1;
  int64_t s_max = 4000;
};

std::ostream &operator<<(std::ostream &out, part_combination p) {
  return out << "part_combination{" << p.x_min << " <= x <= " << p.x_max << ", "
             << p.m_min << " <= m <= " << p.m_max << ", " << p.a_min
             << " <= a <= " << p.a_max << ", " << p.s_min << " <= s <= " << p.s_max
             << "}";
}

struct next_rule {
  next_rule(std::string rule, part_combination part)
      : rule{std::move(rule)}, part{part} {}
  std::string rule;
  part_combination part;
};

std::ostream &operator<<(std::ostream &out, next_rule p) {
  return out << "next_rule{" << p.part << ", " << p.rule << "}";
}

using rule_pair = std::pair<next_rule, part_combination>;
inline rule_pair make_rule_pair(std::string rule, part_combination p) {
  return rule_pair{next_rule{rule, p}, p};
}

using application_result = std::variant<rule_pair, std::string>;

struct basic_rule {
  std::string target;
  basic_rule(std::string target) noexcept : target{std::move(target)} {}
  virtual application_result apply(part_combination p) const = 0;
  virtual ~basic_rule() = default;
  virtual std::ostream &print(std::ostream &out) const {
    return out << "rule{" << target << "}";
  }
};

template<auto part_combination::*Min, auto part_combination::*Max>
struct part_range {
  static constexpr inline auto min = Min;
  static constexpr inline auto max = Max;
};

template<class Id>
struct part_identifier;

template<>
struct part_identifier<struct x_tag> {
  using value = part_range<&part_combination::x_min, &part_combination::x_max>;
};

template<>
struct part_identifier<struct m_tag> {
  using value = part_range<&part_combination::m_min, &part_combination::m_max>;
};

template<>
struct part_identifier<struct a_tag> {
  using value = part_range<&part_combination::a_min, &part_combination::a_max>;
};

template<>
struct part_identifier<struct s_tag> {
  using value = part_range<&part_combination::s_min, &part_combination::s_max>;
};

template<class Comp>
struct part_selector {
  template<class FeatTag>
  static constexpr inline void apply(part_combination &left, part_combination &right, int64_t threshold);
};

template<>
struct part_selector<std::greater<>> {
  template<class FeatTag>
  static constexpr inline void apply(part_combination &left, part_combination &right, int64_t threshold) {
    left.*part_identifier<FeatTag>::value::min = threshold + 1;
    right.*part_identifier<FeatTag>::value::max = threshold;
  }
};

template<>
struct part_selector<std::less<>> {
  template<class FeatTag>
  static constexpr inline void apply(part_combination &left, part_combination &right, int64_t threshold) {
    left.*part_identifier<FeatTag>::value::max = threshold - 1;
    right.*part_identifier<FeatTag>::value::min = threshold;
  }
};

template <class Comp, class FeatTag> struct rule : basic_rule {
  rule(int64_t threshold, std::string target)
      : basic_rule{std::move(target)}, threshold{threshold} {}

  int64_t threshold;

  virtual application_result apply(part_combination p) const override {
    auto pair = make_rule_pair(target, p);
    part_selector<Comp>::template apply<FeatTag>(pair.first.part, pair.second, threshold);
    return pair;
  }

  virtual std::ostream &print(std::ostream &out) const override;
};

struct identity : basic_rule {
  identity(std::string target) : basic_rule{std::move(target)} {}
  virtual application_result apply(part_combination p) const override {
    return application_result{std::in_place_type<rule_pair>, make_rule_pair(target, p)};
  }

  virtual std::ostream &print(std::ostream &out) const override {
    return out << "identity{" << target << "}";
  }
};

template<class Comp>
struct string_for_comp;

template<>
struct string_for_comp<std::greater<>> {
  static constexpr const char *value = " > ";
};

template<>
struct string_for_comp<std::less<>> {
  static constexpr const char *value = " < ";
};

template<class Tag>
struct string_for_part;

template<>
struct string_for_part<struct x_tag> {
  static constexpr const char *value = "x";
};

template<>
struct string_for_part<struct m_tag> {
  static constexpr const char *value = "m";
};

template<>
struct string_for_part<struct a_tag> {
  static constexpr const char *value = "a";
};

template<>
struct string_for_part<struct s_tag> {
  static constexpr const char *value = "s";
};


template<class Comp, class FeatTag>
std::ostream &rule<Comp, FeatTag>::print(std::ostream &out) const {
  return out << "rule{" << string_for_part<FeatTag>::value
             << string_for_comp<Comp>::value << threshold << " -> " << target << "}";
}


using rule_ptr = std::unique_ptr<basic_rule>;
using rule_set = std::vector<rule_ptr>;

rule_ptr parse_rule(std::string_view str) {
  auto op_pos = str.find_first_of("><");

  if (op_pos == std::string_view::npos) {
    return std::make_unique<identity>(std::string{str});
  }

  char op = str[op_pos];
  char sym = str[0];

  auto target_pos = str.find(':');
  auto target = str.substr(target_pos + 1);
  int64_t threshold = std::stoll(str.substr(op_pos + 1, target_pos - op_pos).data());

  switch (sym) {
    case 'x':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, struct x_tag>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, struct x_tag>>(threshold, std::string{target});
      }
    case 'm':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, struct m_tag>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, struct m_tag>>(threshold, std::string{target});
      }
    case 'a':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, struct a_tag>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, struct a_tag>>(threshold, std::string{target});
      }
    case 's':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, struct s_tag>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, struct s_tag>>(threshold, std::string{target});
      }
    default:
      std::cerr << "Invalid symbol in rule " << sym << '\n';
      exit(1);
  }
}

rule_set parse_rules(std::string_view str) {
  rule_set rules;

  while (!str.empty()) {
    auto pos = str.find_first_of(",}");
    rules.emplace_back(parse_rule(str.substr(0, pos)));
    str.remove_prefix(pos + 1);
  }

  return rules;
}

std::ostream& operator<<(std::ostream& out, const std::unique_ptr<basic_rule>& p) {
  return p->print(out);
}

size_t combinations_for(part_combination p) {
  size_t acc = 1;
  acc *= p.x_max - p.x_min + 1;
  acc *= p.m_max - p.m_min + 1;
  acc *= p.a_max - p.a_min + 1;
  acc *= p.s_max - p.s_min + 1;
  return acc;
}

DPSG_AOC_MAIN(file) {

  using namespace std;
  using namespace dpsg;
  using namespace dpsg::vt100;

  string line;
  unordered_map<string, rule_set> rule_map;

  while (getline(file, line)) {
    if (line.empty()) {
      break;
    }

    auto pos = line.find_first_of('{');
    auto name = line.substr(0, pos);
    auto rule = parse_rules(line.substr(pos + 1));

    logln("Rules for ", (bold|red), name, reset, ": ", (bold|yellow), rule, reset);
    rule_map.emplace(name, std::move(rule));

  }

  // We don't care about the parts
  std::vector<next_rule> rules;
  rules.emplace_back(std::string{"in"}, part_combination{});

  size_t acc = 0;

  while (!rules.empty()) {
    std::vector<next_rule> new_rules;
    for (auto rule_to_process : rules) {
      if (rule_to_process.rule == "A") {
        logln("Found an ", (bold|green), " accepted ", reset, "part combination: ", (blue|bold), rule_to_process.part, reset);
        acc += combinations_for(rule_to_process.part);
      }
      if (rule_to_process.rule == "R") {
        logln("Found a ", (bold|red), " rejected ", reset, "part combination: ", (blue|bold), rule_to_process.part, reset);
        continue;
      }
      auto& ruleset = rule_map[rule_to_process.rule];

      logln("From rule ", (bold|blue), rule_to_process, reset, " we get ", (bold|green), ruleset, reset);
      for (auto& rule : ruleset) {
        auto next_rules = rule->apply(rule_to_process.part);
        if (std::holds_alternative<std::string>(next_rules)) {
          logln("From rule ", (bold|blue), rule, reset, " we produce ", (bold|green), std::get<std::string>(next_rules), reset);
          new_rules.emplace_back(std::get<std::string>(next_rules), rule_to_process.part);
        } else if (std::holds_alternative<rule_pair>(next_rules)) {
          auto new_rule_pair = std::get<rule_pair>(std::move(next_rules));
          logln("From rule ", (bold|blue), rule, reset, " we produce\n -> ", (bold|magenta), new_rule_pair.first, reset, "\n -> ", (bold|green), new_rule_pair.second, reset);

          new_rules.emplace_back(std::move(new_rule_pair.first));
          rule_to_process.part = std::move(new_rule_pair.second);
        }
      }
    }

    rules = std::move(new_rules);
  }
  std::cout << "Combinations: " << (vt100::reverse) << acc << '\n';
}
