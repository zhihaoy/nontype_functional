#pragma once

#include "std23/move_only_function.h"

#include <boost/ut.hpp>

using namespace boost::ut;

using std23::move_only_function;
using std23::nontype;
using std23::nontype_t;

#ifdef _MSC_VER
#define BODYN(n) ((::boost::ut::log << __FUNCSIG__ << '\n'), n)
#else
#define BODYN(n) ((::boost::ut::log << __PRETTY_FUNCTION__ << '\n'), n)
#endif

template<auto N> struct int_c : detail::op
{
    using value_type = decltype(N);
    static constexpr auto value = N;

    [[nodiscard]] constexpr operator value_type() const noexcept { return N; }
    [[nodiscard]] constexpr auto get() const { return N; }
};

inline constexpr int_c<0> free_function;
inline constexpr int_c<1> function_template;
inline constexpr int_c<2> empty;
inline constexpr int_c<3> const_;
inline constexpr int_c<4> lref;
inline constexpr int_c<5> const_lref;
inline constexpr int_c<6> rref;
inline constexpr int_c<7> const_rref;
inline constexpr int_c<8> noexcept_;

template<char V> inline constexpr int_c<V> ch;

int f();

struct A
{
    int g();

    int data = 99;
};

int h(A);
