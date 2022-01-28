#pragma once

#include "function_ref.h"

#include <boost/ut.hpp>
#include <cstdio>

#ifdef _MSC_VER
#define BODYN(n) ((::boost::ut::log << __FUNCSIG__), n)
#else
#define BODYN(n) ((::boost::ut::log << __PRETTY_FUNCTION__), n)
#endif
#define BODY() BODYN(0)

using std23::function_ref;
using std23::nontype;
using std23::nontype_t;

using namespace boost::ut;

enum overloads
{
    free_function,
    function_template,
    non_const,
    const_,
};

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
