#include "function_ref.h"

#include <cstdio>

#ifdef _MSC_VER
#define BODY() std::puts(__FUNCSIG__)
#else
#define BODY() std::puts(__PRETTY_FUNCTION__)
#endif

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
    fr = in_place_value<f>;  // f is not constexpr
    fr = in_place_value<cf>; // cf is
    return fr();
}

constexpr int constexpr_test2()
{
    B b;
    function_ref<int(B)> fmr = in_place_value<&B::data>;
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
    foo(in_place_value<f>);
    foo(in_place_value<g<int>>);
    foo({in_place_value<&A::g>, a});
    foo({in_place_value<&A::g>, &a});
    foo({in_place_value<&A::k>, b});
    foo({in_place_value<&A::k>, &b});
    foo({in_place_value<&A::data>, a});
    foo({in_place_value<&A::data>, std::ref(a)});
    foo({in_place_value<&A::data>, std::cref(a)});
    foo({in_place_value<h>, a});

    foo(f);
    foo([] { return BODY(); });
    C c;
    C const cc;
    foo(c);
    foo(cc);

    X my_x_obj;
    function_ref<void()> fr{in_place_value<&X::f>, &my_x_obj};
    fr();
    fr = {in_place_value<&X::g>, &my_x_obj};
    fr();
}
