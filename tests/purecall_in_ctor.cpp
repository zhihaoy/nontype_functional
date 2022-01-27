#include "purecall_in_ctor.h"

abstract_base::abstract_base(courier &c)
{
    c.cb = {nontype<&abstract_base::f>, this};
}

int derived::f()
{
    return data;
}