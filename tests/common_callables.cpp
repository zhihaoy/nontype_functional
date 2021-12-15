#include "common_callables.h"

void foo(function_ref<int()> f)
{
    f();
}

int f()
{
    return BODY();
}

int A::g()
{
    return BODY();
}

int A::k() const
{
    return BODY();
}

int h(A)
{
    return BODY();
}

int C::operator()()
{
    return BODY();
}

int C::operator()() const
{
    return BODY();
}

void X::f()
{
    BODY();
}

void X::g()
{
    BODY();
}
