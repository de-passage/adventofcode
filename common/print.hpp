// Lightweight compatibility header to provide dpsg::print / dpsg::println
// Tries to use std::print (C++23). If unavailable, falls back to
// std::format -> printf. As a last resort uses printf directly.
#pragma once

#include <cstdio>
#include <utility>

// Feature detection: prefer header detection then feature-test macros
#if defined(__has_include)
#  if __has_include(<print>) && defined(__cpp_lib_print)
#    include <print>
#    define DPSG_HAVE_STD_PRINT 1
#  endif
#  if __has_include(<format>) && defined(__cpp_lib_format)
#    include <format>
#    define DPSG_HAVE_STD_FORMAT 1
#  endif
#else
#  ifdef __cpp_lib_print
#    include <print>
#    define DPSG_HAVE_STD_PRINT 1
#  endif
#  ifdef __cpp_lib_format
#    include <format>
#    define DPSG_HAVE_STD_FORMAT 1
#  endif
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

#else

// Fallback: assume caller uses printf-style formatting. This is the
// weakest fallback but allows builds to succeed on older standard libs.
template <class... Args>
inline void print(const char* fmt, Args&&... args)
{
    std::printf(fmt, std::forward<Args>(args)...);
}

template <class... Args>
inline void println(const char* fmt, Args&&... args)
{
    std::printf(fmt, std::forward<Args>(args)...);
    std::putchar('\n');
}

#endif

} // namespace dpsg
