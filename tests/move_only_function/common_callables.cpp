#include "common_callables.h"

int f()
{
    return BODYN(0);
}

int f_good() noexcept
{
    return BODYN(1);
}
