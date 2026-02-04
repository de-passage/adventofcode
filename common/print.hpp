// Lightweight compatibility header to provide dpsg::print / dpsg::println
// Tries to use std::print (C++23). If unavailable, falls back to
// std::format -> printf. As a last resort uses printf directly.
#pragma once

#include <cstdio>
#include <utility>

// Feature detection: prefer header detection then feature-test macros
#if defined(__has_include)
#  if __has_include(<print>)
#    include <print>
#    define DPSG_HAVE_STD_PRINT 1
#  endif
#  if __has_include(<format>)
#    include <format>
#    define DPSG_HAVE_STD_FORMAT 1
#  endif
#else
#  ifdef __cpp_lib_print
#    include <print>
#    define DPSG_HAVE_STD_PRINT 1
#  elif defined __cpp_lib_format
#    include <format>
#    define DPSG_HAVE_STD_FORMAT 1
#  endif
#endif

#if !(defined(DPSG_HAVE_STD_PRINT) || defined(DPSG_HAVE_STD_FORMAT))
#  error "No suitable print implementation found. Please use a compiler with C++23 std::print or C++20 std::format support."
#endif

namespace dpsg {

#if defined(DPSG_HAVE_STD_PRINT)

template <class... Args>
inline void print(const std::format_string<Args...>& fmt, Args&&... args)
{
    std::print(fmt, std::forward<Args>(args)...);
}

template <class... Args>
inline void println(const std::format_string<Args...>& fmt, Args&&... args)
{
    std::print(fmt, std::forward<Args>(args)...);
    std::putchar('\n');
}

inline void println() {
    std::println();
}

template <class... Args>
inline void print(FILE* file, const std::format_string<Args...>& fmt, Args&&... args)
{
    std::print(fmt, std::forward<Args>(args)...);
}

template <class... Args>
inline void println(FILE* file, const std::format_string<Args...>& fmt, Args&&... args)
{
    std::print(fmt, std::forward<Args>(args)...);
    std::putchar('\n');
}

inline void println(FILE* file) {
    std::println();
}

#elif defined(DPSG_HAVE_STD_FORMAT)

template <class... Args>
inline void print(const std::format_string<Args...>& fmt, Args&&... args)
{
    auto s = std::format(fmt, std::forward<Args>(args)...);
    std::printf("%s", s.c_str());
}

template <class... Args>
inline void println(const std::format_string<Args...>& fmt, Args&&... args)
{
    auto s = std::format(fmt, std::forward<Args>(args)...);
    std::printf("%s\n", s.c_str());
}

inline void println() {
    std::putchar('\n');
}

template <class... Args>
inline void print(FILE* file, const std::format_string<Args...>& fmt, Args&&... args)
{
    auto s = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(file, "%s", s.c_str());
}

template <class... Args>
inline void println(FILE* file, const std::format_string<Args...>& fmt, Args&&... args)
{
    auto s = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(file, "%s\n", s.c_str());
}

inline void println(FILE* file)
{
    std::fprintf(file, "\n");
}

#else

// Error above, these stubs are here to avoid printing too much redondant info
inline void print(...) {}
inline void println(...) {}

#endif

} // namespace dpsg
