#include "utils.hpp"

#include <memory>
#include <unordered_map>

struct part {
  int64_t x;
  int64_t m;
  int64_t a;
  int64_t s;
};

struct basic_rule {
  std::string target;
  basic_rule(std::string target) noexcept : target{std::move(target)} {}
  virtual bool apply(part p) const = 0;
  virtual ~basic_rule() = default;
  virtual std::ostream& print(std::ostream& out) const {
    return out << "rule{" << target << "}";
  }
};

template<class Comp, auto part::*Ptr>
struct rule : basic_rule {
  rule(int64_t threshold, std::string target) : basic_rule{std::move(target)}, threshold{threshold} {}
  int64_t threshold;

  virtual bool apply(part p) const override {
    return Comp{}(p.*Ptr, threshold);
  }

  virtual std::ostream& print(std::ostream& out) const override;
};

template<class Comp> struct string_for_comp;
template<> struct string_for_comp<std::greater<>> {
  static constexpr const char* value = " > ";
};
template<> struct string_for_comp<std::less<>> {
  static constexpr const char* value = " < ";
};

template<auto part::*Ptr> struct string_for_part;
template<> struct string_for_part<&part::x> {
  static constexpr const char* value = "x";
};
template<> struct string_for_part<&part::m> {
  static constexpr const char* value = "m";
};
template<> struct string_for_part<&part::a> {
  static constexpr const char* value = "a";
};
template<> struct string_for_part<&part::s> {
  static constexpr const char* value = "s";
};

template<class Comp, auto part::*Ptr>
std::ostream& rule<Comp, Ptr>::print(std::ostream& out) const {
  return out << "rule{" << string_for_part<Ptr>::value << string_for_comp<Comp>::value << threshold << " " << target << "}";
}

struct always : basic_rule {
  always(std::string target) : basic_rule{std::move(target)} {}
  virtual bool apply(part) const override {
    return true;
  }
};

std::ostream& operator<<(std::ostream& out, const part& p) {
  return out << "{ x: " << p.x << ", m: " << p.m << ", a: " << p.a << ", s: " << p.s << " }";
}

std::ostream& operator<<(std::ostream& out, const std::unique_ptr<basic_rule>& p) {
  return p->print(out);
}

using rule_ptr = std::unique_ptr<basic_rule>;
using rule_set = std::vector<rule_ptr>;

template<class T>
struct parse_result {
  T value;
  std::string_view rest;
};

rule_ptr parse_rule(std::string_view str) {
  auto op_pos = str.find_first_of("><");

  if (op_pos == std::string_view::npos) {
    return std::make_unique<always>(std::string{str});
  }

  char op = str[op_pos];
  char sym = str[0];

  auto target_pos = str.find(':');
  auto target = str.substr(target_pos + 1);
  int64_t threshold = std::stoll(str.substr(op_pos + 1, target_pos - op_pos).data());

  switch (sym) {
    case 'x':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, &part::x>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, &part::x>>(threshold, std::string{target});
      }
    case 'm':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, &part::m>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, &part::m>>(threshold, std::string{target});
      }
    case 'a':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, &part::a>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, &part::a>>(threshold, std::string{target});
      }
    case 's':
      if (op == '>') {
        return std::make_unique<rule<std::greater<>, &part::s>>(threshold, std::string{target});
      } else {
        return std::make_unique<rule<std::less<>, &part::s>>(threshold, std::string{target});
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

part parse_part(std::string_view str) {
  part p{0, 0, 0, 0};

  enum class state {
    x,
    m,
    a,
    s,
    exp_symbol,
  } currently_parsing = state::exp_symbol;

  for (auto c : str) {
    switch(c) {
      case 'x':
        currently_parsing = state::x;
        break;
      case 'm':
        currently_parsing = state::m;
        break;
      case 'a':
        currently_parsing = state::a;
        break;
      case 's':
        currently_parsing = state::s;
        break;
      case '}':
        return p;
      case '{': [[fallthrough]];
      case ',':
        currently_parsing = state::exp_symbol;
        break;
      case '=':
        break;
      default:
        switch (currently_parsing) {
          case state::x:
            p.x = p.x * 10 + (c - '0');
            break;
          case state::m:
            p.m = p.m * 10 + (c - '0');
            break;
          case state::a:
            p.a = p.a * 10 + (c - '0');
            break;
          case state::s:
            p.s = p.s * 10 + (c - '0');
            break;
          case state::exp_symbol:
            break;
        }
    }
  }

  return p;
}


int main(int argc, const char** argv) {

  using namespace std;
  using namespace dpsg;
  using namespace dpsg::vt100;

  auto input = get_input("day19.txt", argc, argv);

  string line;

  unordered_map<string, rule_set> rule_map;

  // parsing rules
  while(getline(input, line)) {
    if (line.empty()) {
      break;
    }

    auto pos = line.find_first_of('{');
    auto name = line.substr(0, pos);
    auto rule = parse_rules(line.substr(pos + 1));

    logln("Rules for ", (bold|red), name, reset, ": ", (bold|yellow), rule, reset);
    rule_map.emplace(name, std::move(rule));
  }

  auto sum = 0;
  // parsing input
  while(getline(input, line)) {
    if (line.empty()) {
      break;
    }


    auto part = parse_part(line);
    logln("Part: ", (bold|blue), part, reset);

    std::string_view current_rule = "in";

    while (current_rule != "R" && current_rule != "A") {
      auto& rule_set = rule_map.at(string{current_rule});
      auto it = std::find_if(rule_set.begin(), rule_set.end(), [&part](const auto& rule) {
        return rule->apply(part);
      });

      if (it == rule_set.end()) {
        logln("No rule found for ", (bold|red), current_rule, reset);
        current_rule = "R";
      } else {
        logln("Rule ", (bold|yellow), *it, reset, " applies");
        current_rule = (*it)->target;
      }
    }

    if (current_rule == "R") {
      logln("Part is ", (red|bold), "rejected", reset);
    } else {
      log("Part is ", (bold|green), "accepted", reset, " -> ");
      sum += part.x + part.m + part.a + part.s;
      logln("Sum: ", (bold|yellow), sum, reset);
    }

  }

  std::cout << "Sum: " << (yellow|bold) << sum << '\n';

}
