#include "common_callables.h"

int f()
{
    return BODYN(free_function);
}

int A::g()
{
    return BODYN(empty);
}

int h(A)
{
    return BODYN(free_function);
}
