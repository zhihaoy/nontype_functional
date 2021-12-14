#include "function_ref.h"

#include <cstdio>

#ifdef _MSC_VER
#define BODY() std::puts(__FUNCSIG__)
#else
#define BODY() std::puts(__PRETTY_FUNCTION__)
#endif

using std23::function_ref;
using std23::nontype;

void foo(function_ref<int()> f)
{
    f();
}

int f()
{
    return BODY();
}

template <class T> T g()
{
    return BODY();
}

struct A
{
    int g()
    {
        return BODY();
    }

    int k() const
    {
        return BODY();
    }

    int data = 99;
};

int h(A)
{
    return BODY();
}

struct C
{
    int operator()()
    {
        return BODY();
    }

    int operator()() const
    {
        return BODY();
    }
};

struct Wat
{
    operator auto() const
    {
        return f;
    }
};

constexpr int cf()
{
    return 2;
}

struct B
{
    int data = 101;
};

constexpr int constexpr_test1()
{
    function_ref<int()> fr;
    fr = nontype<f>;  // f is not constexpr
    fr = nontype<cf>; // cf is
    return fr();
}

constexpr int constexpr_test2()
{
    B b;
    function_ref<int(B)> fmr = nontype<&B::data>;
    return fmr(b);
}

static_assert(constexpr_test1() == 2);
static_assert(constexpr_test2() == 101);

struct X
{
    void f()
    {
        BODY();
    }
    void g()
    {
        BODY();
    }
};

int main()
{
    A a;
    A const b;
    foo(nontype<f>);
    foo(nontype<g<int>>);
    foo({nontype<&A::g>, a});
    foo({nontype<&A::g>, &a});
    foo({nontype<&A::k>, b});
    foo({nontype<&A::k>, &b});
    foo({nontype<&A::data>, a});
    foo({nontype<&A::data>, std::ref(a)});
    foo({nontype<&A::data>, std::cref(a)});
    foo({nontype<h>, a});

    foo(f);
    foo([] { return BODY(); });
    C c;
    C const cc;
    foo(c);
    foo(cc);
}
