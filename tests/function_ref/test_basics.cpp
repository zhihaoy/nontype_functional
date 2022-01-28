#include "common_callables.h"

void test_basics()
{
    foo(f);
    foo([] { return BODY(); });
    C c;
    C const cc;
    foo(c);
    foo(cc);
}
