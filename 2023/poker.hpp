#ifndef HEADER_GUARD_POKER_HPP
#define HEADER_GUARD_POKER_HPP

#include <algorithm>
#include <cstring>

#include <iostream>

struct hand {
  struct  {
    size_t value;
    size_t quantity;

    constexpr friend bool operator==(const auto &a, const auto& b) {
      return a.value == b.value && a.quantity == b.quantity;
    }

    constexpr friend std::ostream& operator<<(std::ostream& os, const auto& c) {
      return os << '{' << c.value << ", " << c.quantity << '}';
    }

    constexpr friend bool operator<(const auto &a, const auto& b) {
      return a.quantity < b.quantity || (a.quantity == b.quantity && a.value < b.value);
    }

    constexpr friend bool operator>(const auto &a, const auto& b) {
      return b < a;
    }
  } cards[5];
  int order[5];
};

hand make_hand(int cards[5]) {
  hand h;
  std::memcpy(h.order, cards, sizeof(h.order));
  std::memset(h.cards, 0, sizeof(h.cards));
  std::sort(cards, cards + 5);
  size_t last = cards[0];
  size_t acc = 0;
  size_t idx = 0;

  for (size_t i = 0; i < 5; i++) {
    if ((size_t)cards[i] == last) {
      acc++;
    } else {
      h.cards[idx++] = {.value = last, .quantity=acc};
      last = cards[i];
      acc = 1;
    }
  }
  h.cards[idx++] = {last, acc};
  std::sort(h.cards, h.cards + 5, std::greater<>());
  return h;
}

hand parse_hand(std::string_view s) {
  int cards[5];
  for (int i = 0; i < 5; ++i)  {
    switch (s[i]) {
      case 'A':
        cards[i] = 14;
        break;
      case 'K':
        cards[i] = 13;
        break;
      case 'Q':
        cards[i] = 12;
        break;
      case 'J':
        cards[i] = 11;
        break;
      case 'T':
        cards[i] = 10;
        break;
      default:
        cards[i] = s[i] - '0';
    }
  }
  return make_hand(cards);
}

std::ostream& operator<<(std::ostream& os, const hand& h) {
  os << '<';
  for (int i = 0; i < 5 && h.cards[i].quantity > 0; i++) {
    auto value = h.cards[i].value;
    auto quantity = h.cards[i].quantity;
    for (size_t j = 0; j < quantity; ++j) {
      switch (value) {
        case 14:
          os << 'A';
          break;
        case 13:
          os << 'K';
          break;
        case 12:
          os << 'Q';
          break;
        case 11:
          os << 'J';
          break;
        case 10:
          os << 'T';
          break;
        default:
          os << value;
      }
    }
  }
  return os << '>';
}

hand parse_hand(const char s[6]) {
  return parse_hand(std::string_view(s, 5));
}

bool operator<(const hand &a, const hand &b) {
  for (int i = 0; i < 5; ++i) {
    if (a.cards[i].quantity < b.cards[i].quantity) {
      return true;
    } else if (a.cards[i].quantity > b.cards[i].quantity) {
      return false;
    }
  }
  for (int i = 0; i < 5; ++i) {
    if (a.order[i] < b.order[i]) {
      return true;
    } else if (a.order[i] > b.order[i]) {
      return false;
    }
  }
  return false;
}

bool operator>(const hand &a, const hand &b) {
  for (int i = 0; i < 5; ++i) {
    if (a.cards[i].quantity > b.cards[i].quantity) {
      return true;
    } else if (a.cards[i].quantity < b.cards[i].quantity) {
      return false;
    }
  }
  for (int i = 0; i < 5; ++i) {
    if (a.order[i] > b.order[i]) {
      return true;
    } else if (a.order[i] < b.order[i]) {
      return false;
    }
  }
  return false;
}

bool operator==(const hand &a, const hand &b) {
  return std::equal(a.cards, a.cards + 5, b.cards);
}


#endif // HEADER_GUARD_POKER_HPP
