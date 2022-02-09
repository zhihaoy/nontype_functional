#pragma once

#include "std23/function_ref.h"

#include <boost/ut.hpp>

#ifdef _MSC_VER
#define BODYN(n) ((::boost::ut::log << __FUNCSIG__ << '\n'), n)
#else
#define BODYN(n) ((::boost::ut::log << __PRETTY_FUNCTION__ << '\n'), n)
#endif

using std23::function_ref;
using std23::nontype;
using std23::nontype_t;

using namespace boost::ut;

template<auto N> struct int_c : detail::op
{
    using value_type = decltype(N);
    static constexpr auto value = N;

    [[nodiscard]] constexpr operator value_type() const { return N; }
    [[nodiscard]] constexpr auto get() const { return N; }
};

inline constexpr int_c<0> free_function;
inline constexpr int_c<1> function_template;
inline constexpr int_c<2> non_const;
inline constexpr int_c<3> const_;

template<char V> inline constexpr int_c<V> ch;

void foo(function_ref<int()> f);

int f();

template<class T> T g()
{
    return BODYN(function_template);
}

struct A
{
    int g();
    int k() const;

    int data = 99;
};

int h(A);

struct C
{
    int operator()();
    int operator()() const;
};
