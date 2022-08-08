#include "common_callables.h"

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
