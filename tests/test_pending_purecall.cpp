#include "purecall_in_ctor.h"

void test_pending_purecall()
{
    courier c;
    derived d(c);
    std::printf("data = %d\n", c.cb());
}
