#include <cmath>
#include <unordered_set>
#include "utils.hpp"

constexpr u64 count_digits(u64 n)
{
    u64 r = 0;
    while (n)
    {
        r++;
        n /= 10;
    }
    return r;
}

inline bool odd(auto n) { return n % 2 != 0; }

constexpr u64 repeat(u64 n, u64 times, u64 dcount)
{
    u64 offset = pow(10, dcount);
    u64 acc = n;
    while (times--)
    {
        acc *= offset;
        acc += n;
    }
    return acc;
}
constexpr u64 repeat(u64 n, u64 times)
{
    return repeat(n, times, count_digits(n));
}
// template<u64 S> struct show; show<repeat(1,2,1)> k;
static_assert(repeat(1, 1) == 11);
static_assert(repeat(1, 2) == 111);
static_assert(repeat(123, 2) == 123123123);

// we need:
// iterate over numbers with 1 repeating digit
// --                        2  --
// ...
// iterate over numbers with n/2 repeating digits.
// => for i in (1 .. n/2) :
//    trim the start and end, for n in (start .. end), if start <= n <= end -> add to sum

constexpr u64 higher_digits(u64 n, u64 count)
{
    return n / pow(10, count);
}

DPSG_AOC_MAIN(file)
{
    std::string line;

    u64 sum = 0;
    while (std::getline(file, line))
    {
        u64 cursor = 0;
        while (true)
        {
            auto start_str = dpsg::next_number(line, cursor);
            if (!start_str)
            {
                break;
            }
            cursor = start_str->position.end;
            i64 start = start_str->value;
            auto end_str = dpsg::next_number(line, cursor);
            if (!end_str)
            {
                break;
            }
            cursor = end_str->position.end;
            i64 end = end_str->value;

            if (start == 0)
            {
                start = 10;
            } // no need to consider numbers 0-9

            u64 sz = count_digits(start);
            u64 ez = count_digits(end);
            dpsg::logln("\e[2msz: ", sz, "\tez: ", ez);
            std::unordered_set<u64> set;

            // i == nb of digits to repeat in the answer
            for (int i = 1; i <= ez / 2; ++i)
            {
                dpsg::logln("Processing i = ", i);

                u64 first = higher_digits(start, sz - i);
                u64 last = higher_digits(end, ez - i);
                u64 power_first = pow(10, i - 1);
                u64 power = power_first * 10;

                // j == Total nb of digits in the answer. Only valid if there is some x, x*i == j
                for (int j = sz; j <= ez; ++j)
                {
                    dpsg::logln("Processing j = ", j);
                    // don't process if we can't create a number with the appropriate nb of digits (j)
                    if (j % i != 0)
                    {
                        continue;
                    }

                    u64 current = first;
                    while (true)
                    {
                        dpsg::logln("Processing current = ", current);
                        u64 nb = repeat(current, j / i - 1, i);

                        if (nb >= start && nb <= end)
                        {
                            dpsg::logln("\e[0;1;33m", nb, "\e[m");
                            set.insert(nb);
                        }
                        else
                        {
                            dpsg::logln("\e[0;2;31m", nb, "\e[m");
                        }

                        if (current == last)
                        {
                            break;
                        }
                        current = (current + 1) % power;
                        if (current == 0)
                        {
                            current = power_first;
                        };
                    }
                }
            }

            for (auto v : set)
            {
                sum += v;
            }
        }
    }
    std::cout << "Sum: " << sum << std::endl;
}