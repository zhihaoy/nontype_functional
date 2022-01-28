#pragma once

#include "function_ref.h"

#include <cassert>
#include <cstdio>

#ifdef _MSC_VER
#define BODY() std::puts(__FUNCSIG__)
#else
#define BODY() std::puts(__PRETTY_FUNCTION__)
#endif

using std23::function_ref;
using std23::nontype;
using std23::nontype_t;

void foo(function_ref<int()> f);

int f();

template<class T> T g()
{
    return BODY();
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

struct X
{
    void f();
    void g();
};
