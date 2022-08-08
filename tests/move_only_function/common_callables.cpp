#include "common_callables.h"

int f()
{
    return BODYN(free_function);
}

int f_good() noexcept
{
    return BODYN(free_function);
}
