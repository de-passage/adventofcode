#ifndef HEADER_GUARD_BIT_ARRAY_HPP
#define HEADER_GUARD_BIT_ARRAY_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace dpsg {
template <size_t S> struct bit_array {
  uint8_t values[(S + 7) / 8];

  inline constexpr uint8_t operator[](size_t i) const noexcept {
    return (values[i / 8] >> (i % 8)) & 1;
  }

  struct proxy {
    uint8_t *const dest;
    const uint8_t offset;
    const uint8_t mask = 1 << offset;
    constexpr inline const proxy &operator=(uint8_t v) noexcept {
      *dest = (*dest & ~mask) | (v << offset);
      return *this;
    }
    constexpr inline void set() noexcept { *dest |= mask; }
    constexpr inline void unset() noexcept { *dest &= ~mask; }
    constexpr inline void toggle() noexcept { *dest ^= ~mask; }
    constexpr operator uint8_t() const noexcept { return (*dest & mask) >> offset; }
  };

  inline constexpr proxy operator[](size_t i) noexcept {
    return {
        .dest = values + (i / 8),
        .offset = uint8_t(i % 8),
    };
  }

  constexpr inline size_t size() const noexcept { return S; }

  using reference = proxy;
  using const_reference = proxy;
};

template<typename ...Values, std::enable_if_t<std::conjunction_v<std::is_convertible<Values, uint8_t>...>, int> = 0>
constexpr inline bit_array<sizeof...(Values)*8> make_bit_array(Values... vs) noexcept {
  return bit_array<sizeof...(Values)*8>{
    .values = {uint8_t(vs)...},
  };
}

constexpr bit_array<0> a0{};
static_assert(sizeof(a0) == 0);
constexpr bit_array<1> a1{};
static_assert(sizeof(a1) == 1);
constexpr bit_array<8> a8{};
static_assert(sizeof(a8) == 1);
constexpr bit_array<9> a9{};
static_assert(sizeof(a9) == 2);
static_assert(sizeof(make_bit_array(0, 0)) == 2);
static_assert(make_bit_array(0,0xFF)[0] == 0);
static_assert(make_bit_array(0,0xFF)[1] == 0);
static_assert(make_bit_array(0,0xFF)[2] == 0);
static_assert(make_bit_array(0,0xFF)[3] == 0);
static_assert(make_bit_array(0,0xFF)[4] == 0);
static_assert(make_bit_array(0,0xFF)[5] == 0);
static_assert(make_bit_array(0,0xFF)[6] == 0);
static_assert(make_bit_array(0,0xFF)[7] == 0);
static_assert(make_bit_array(0,0xFF)[8] == 1);
static_assert(make_bit_array(0,0xFF)[9] == 1);
static_assert(make_bit_array(0,0xFF)[10] == 1);
static_assert(make_bit_array(0,0xFF)[11] == 1);
static_assert(make_bit_array(0,0xFF)[12] == 1);
static_assert(make_bit_array(0,0xFF)[13] == 1);
static_assert(make_bit_array(0,0xFF)[14] == 1);
static_assert(make_bit_array(0,0xFF)[15] == 1);
}

#endif // HEADER_GUARD_BIT_ARRAY_HPP
