#include "common_callables.h"

void foo(function_ref<int()> f)
{
    f();
}

int f()
{
    return BODYN(free_function);
}

int A::g()
{
    return BODYN('g');
}

int A::k() const
{
    return BODYN('k');
}

int h(A)
{
    return BODYN(free_function);
}

int C::operator()()
{
    return BODYN(non_const);
}

int C::operator()() const
{
    return BODYN(const_);
}
