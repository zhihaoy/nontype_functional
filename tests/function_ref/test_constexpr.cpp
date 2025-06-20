#include "common_callables.h"

namespace
{

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
    function_ref<int()> fr = cw<f>;
    fr = cw<cf>; // cf is a constexpr function
    return fr();
}

constexpr int constexpr_test2()
{
    B b;
    function_ref<int(B)> fmr = cw<&B::data>;
    return fmr(b);
}

static_assert(constexpr_test1() == 2);
static_assert(constexpr_test2() == 101);

constexpr int f()
{
    return 42;
}

constexpr std23::function_ref<int()> r = std23::cw<f>;
static_assert(r() == 42);

} // namespace
