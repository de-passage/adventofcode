#include "utils.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

using module_list = std::vector<std::string>;

enum class pulse {
  high,
  low,
};

struct module_map {
  std::unordered_map<std::string, std::unique_ptr<struct basic_module>> modules;
  std::vector<std::tuple<std::string, std::string, pulse>> pulses;

  void pulse(const std::string &from, const std::string &to, pulse p) {
    pulses.emplace_back(from, to, p);
  }

  // Returns true if there are more pulses to process
  bool process_pulse() {
    auto pulses = std::exchange(this->pulses, {});
    for (auto &[from, to, p] : pulses) {
      _do_pulse(from, to, p);
    }
    return !pulses.empty();
  }

  size_t low_count() const { return _low_count; }
  size_t high_count() const { return _high_count; }

private:
  void _do_pulse(const std::string &from, const std::string &to, enum pulse p);
  size_t _high_count = 0;
  size_t _low_count = 0;
};

struct basic_module {
private:
  module_map *ports;

protected:
  module_list _outputs;
  module_list _inputs;
  std::string _name;

  basic_module(module_map *ports, std::string name)
      : ports(ports), _name{std::move(name)} {}

  friend void connect(basic_module &from, basic_module &to) {
    from._outputs.push_back(to._name);
    to._inputs.push_back(from._name);
  }

  void send(pulse p) {
    for (auto &output : _outputs) {
      ports->pulse(_name, output, p);
    }
  }

  void send_low() { send(pulse::low); }
  void send_high() { send(pulse::high); }

public:
  virtual void receive(const std::string &from, pulse p) = 0;
  virtual ~basic_module() = default;
  virtual bool is_in_initial_state() const = 0;
};

void module_map::_do_pulse(const std::string &from, const std::string &to,
                           enum pulse p) {
  using namespace dpsg::vt100;
  dpsg::logln((bold | green), from, reset | bold, " -",
              (p == pulse::high ? "high" : "low"), "-> ", (bold | green), to,
              reset);
  modules.at(to)->receive(from, p);
  if (p == pulse::high) {
    ++_high_count;
  } else {
    ++_low_count;
  }
}

struct debug_module : basic_module {
  debug_module(module_map *ports, std::string name)
      : basic_module(ports, std::move(name)) {}
  virtual void receive(const std::string &, pulse) override {
    // Nothing to do
  }
  virtual bool is_in_initial_state() const override { return true; }
};

struct broadcaster : basic_module {
  broadcaster(module_map *ports, std::string name)
      : basic_module(ports, std::move(name)) {}

  virtual void receive(const std::string &, pulse p) override { send(p); }
  virtual bool is_in_initial_state() const override { return true; }
};

struct flip_flop : basic_module {
  enum class state { on, off } state = state::off;

  flip_flop(module_map *ports, std::string name)
      : basic_module(ports, std::move(name)) {}

  virtual void receive(const std::string &, pulse p) override {
    if (p == pulse::low) {
      if (state == state::on) {
        state = state::off;
        send_low();
      } else {
        state = state::on;
        send_high();
      }
    } else {
      // ignoring high pulses
    }
  }

  virtual bool is_in_initial_state() const override {
    return state == state::off;
  }
};

struct conjunction : basic_module {
  conjunction(module_map *ports, std::string name)
      : basic_module(ports, std::move(name)) {}

  std::unordered_set<std::string> inputs_state;

  virtual void receive(const std::string &from, pulse p) override {
    if (p == pulse::low) {
      inputs_state.erase(from);
      send_high();
    } else {
      inputs_state.insert(from);
      if (inputs_state.size() == _inputs.size()) {
        send_low();
      } else {
        send_high();
      }
    }
  }

  virtual bool is_in_initial_state() const override {
    return inputs_state.empty();
  }
};

std::vector<std::string> parse_outputs(std::string_view line) {
  std::vector<std::string> outputs;
  auto pos = line.find_first_not_of(" ,");
  while (pos != std::string_view::npos) {
    auto end = line.find_first_of(", ", pos);
    outputs.emplace_back(line.substr(pos, end - pos));
    pos = line.find_first_not_of(", ", end);
  }
  return outputs;
}

DPSG_AOC_MAIN(file) {
  using namespace std;
  using namespace dpsg;
  using namespace dpsg::vt100;
  string line;

  std::unordered_map<std::string, std::vector<std::string>> connections;
  module_map modules;

  while (getline(file, line)) {
    auto first = line[0];
    if (first == '%') {
      auto rest = string_view(line).substr(1);
      auto name = rest.substr(0, rest.find_first_of(' '));
      auto outputs = parse_outputs(rest.substr(rest.find('>') + 1));
      auto module = make_unique<flip_flop>(&modules, string(name));

      modules.modules.emplace(name, std::move(module));
      connections.emplace(name, std::move(outputs));
    } else if (first == '&') {
      auto rest = string_view(line).substr(1);
      auto name = rest.substr(0, rest.find_first_of(' '));
      auto outputs = parse_outputs(rest.substr(rest.find('>') + 1));
      auto module = make_unique<::conjunction>(&modules, string(name));

      modules.modules.emplace(name, std::move(module));
      connections.emplace(name, std::move(outputs));

    } else {

      auto name = line.substr(0, line.find_first_of(' '));
      if (name != "broadcaster") {
        std::cerr << "Unknown module: " << name << '\n';
        exit(1);
      }
      auto outputs = parse_outputs(line.substr(line.find('>') + 1));
      auto module = make_unique<broadcaster>(&modules, string(name));

      modules.modules.emplace(name, std::move(module));
      connections.emplace(name, std::move(outputs));
    }
  }

  for (auto &[name, outputs] : connections) {
    logln("Connecting module ", (bold | green), name, reset, " to ",
          (bold | yellow), outputs, reset);
    auto &module = modules.modules.at(name);
    for (auto &output : outputs) {
      auto module_it = modules.modules.find(output);
      if (module_it == modules.modules.end()) {
        logln("Creating module ", (bold | green), output, reset);
        module_it =
            modules.modules
                .emplace(output, make_unique<debug_module>(&modules, output))
                .first;
      }
      connect(*module, *modules.modules.at(module_it->first));
    }
  }

  auto it = 0;

  do {
    modules.pulse("button", "broadcaster", pulse::low);
    while (modules.process_pulse()) { /* nothing more to do here */
    }
    logln("Pulse count at iteration ", (bold | blue), ++it, reset, " : low pulses ",
          (bold | green), modules.low_count(), reset, " / ", (bold | red), modules.high_count(), reset, " high pulses\n");
  } while (
      !std::all_of(modules.modules.begin(), modules.modules.end(),
                   [](auto &p) { return p.second->is_in_initial_state(); }) && it < 1000);

  auto mult = 1000 / it;
  auto low_count = modules.low_count() * mult;
  auto high_count = modules.high_count() * mult;

  logln("Low pulses: ", (bold | green), low_count, reset, " / ", (bold | red), high_count, reset, " high pulses");

  std::cout << "Total: " << low_count * high_count << '\n';
}
