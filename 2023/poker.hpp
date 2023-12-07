#ifndef HEADER_GUARD_POKER_HPP
#define HEADER_GUARD_POKER_HPP

#include <algorithm>
#include <cstring>

struct hand {
  int major; // Highest number of similar cards
  int minor; // Second highest number of similar cards
  int cards[5]; // Sorted number of cards
  int similarity[5]; // Value of similar cards
};

#include <iostream>

hand make_hand(int cards[5]) {
  hand h;
  std::memset(h.similarity, 0, sizeof(h.similarity));
  std::memcpy(h.cards, cards, sizeof(h.cards));
  std::sort(h.cards, h.cards + 5);
  h.major = 1;
  h.minor = 1;
  int current_major = h.cards[0];
  for (int i = 0; i < 5; i++) {
    int this_value = 1;

    for (int j = i + 1; j < 5; j++) {
      this_value += h.cards[i] == h.cards[j];
    }

    if (h.similarity[this_value - 1] < h.cards[i]) {
      bool already_set = false;
      for (int j = this_value; j < 5; j++) {
        if (h.similarity[j] == h.cards[i]) {
          already_set = true;
          break;
        }
      }
      if (!already_set) {
        h.similarity[this_value - 1] = h.cards[i];
      }
    }

    if (this_value > h.major) {
      if (h.cards[i] != current_major) {
        h.minor = h.major;
        current_major = h.cards[i];
      }
      h.major = this_value;
    } else if (this_value > h.minor && h.cards[i] != current_major) {
      h.minor = this_value;
    }
  }
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
  for (int i = 0; i < 5; i++) {
    switch (h.cards[i]) {
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
        os << h.cards[i];
    }
  }
  return os << '>';
}

hand parse_hand(const char s[6]) {
  return parse_hand(std::string_view(s, 5));
}

bool operator<(const hand &a, const hand &b) {
  for (int i = 4; i >= 0; --i) {
    if (a.similarity[i] < b.similarity[i]) {
      return true;
    } else if (a.similarity[i] > b.similarity[i]) {
      return false;
    }
  }
  return false;
}

bool operator>(const hand &a, const hand &b) {
  return b < a;
}

bool operator==(const hand &a, const hand &b) {
  return std::equal(a.similarity, a.similarity + 5, b.similarity);
}


#endif // HEADER_GUARD_POKER_HPP
