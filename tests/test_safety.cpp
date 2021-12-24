#include "common_callables.h"

static_assert(std::is_invocable_v<decltype(foo), decltype(f)>);
static_assert(std::is_invocable_v<decltype(foo), decltype(&f)>);

auto get_h()
{
    return h;
}

using T = function_ref<int(A)>;
static_assert(not std::is_default_constructible_v<T>);

void test_safety()
{
    {
        function_ref fr = get_h();
        A a;
        fr(a);
        fr = &h;
        fr(a);
    }

    {
        T fr = nontype<[](A) { return BODY(); }>;
        A a;
        fr(a);
    }

    {
        function_ref fr = f;
        fr = std::ref(f);
    }

    {
        auto fn = [](A) { return BODY(); };
        T fr = std::ref(fn);
        A a;
        fr(a);
    }

    {
        [](T f) { return f(A{}); }(&A::data);
    }
}