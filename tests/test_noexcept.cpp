#include "common_callables.h"

int f_good() noexcept
{
    return BODY();
}

void bar(function_ref<int() noexcept> f)
{
    f();
}

struct A_good
{
    int g() noexcept
    {
        return BODY();
    }
};

int h_good(A) noexcept
{
    return BODY();
}

void test_noexcept()
{
    // static_assert(std::is_invocable_v<decltype(foo), decltype(f)>);
    static_assert(not std::is_invocable_v<decltype(bar), decltype(f)>);

    bar(f_good);

    static_assert(std::is_invocable_v<decltype(foo), C>);
    static_assert(not std::is_invocable_v<decltype(bar), C>);

    bar([]() noexcept { return BODY(); });

    static_assert(std::is_invocable_v<decltype(foo), nontype_t<f>>);
    static_assert(not std::is_invocable_v<decltype(bar), nontype_t<f>>);

    bar(nontype<f_good>);

    using T = function_ref<int()>;
    using U = function_ref<int() noexcept>;
    static_assert(std::is_constructible_v<T, nontype_t<&A::g>, A &>);
    static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A &>);

    A_good x;
    bar({nontype<&A_good::g>, x});
    bar({nontype<&A_good::g>, &x});
    bar({nontype<&A_good::g>, std::ref(x)});

    A a;
    bar({nontype<&A::data>, a});

    static_assert(std::is_constructible_v<T, nontype_t<h>, A &>);
    static_assert(not std::is_constructible_v<U, nontype_t<h>, A &>);

    bar({nontype<h_good>, a});
}