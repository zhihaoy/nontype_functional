#include "common_callables.h"

struct D
{
    int operator()() noexcept
    {
        return BODY();
    }

    int operator()() const noexcept
    {
        return BODY();
    }
};

void dad(function_ref<int() const noexcept> f)
{
    f();
}

struct A_nice
{
    int g() noexcept
    {
        return BODY();
    }

    int h() const noexcept
    {
        return BODY();
    }
};

void test_const_noexcept()
{
    D d;
    D const cd;
    foo(d);
    dad(d);
    foo(cd);
    dad(cd);

    static_assert(
        not std::is_invocable_v<decltype(dad), decltype([]() { return 0; })>);
    static_assert(
        not std::is_invocable_v<
            decltype(dad), decltype([i = 0]() mutable noexcept { return i; })>);

    dad([]() noexcept { return BODY(); });

    A a;
    dad({nontype<&A::data>, a});
    dad({nontype<&A::data>, &a});
    dad({nontype<&A::data>, std::ref(a)});
    dad({nontype<&A::data>, std::cref(a)});
    dad({nontype<&A::data>, A{}});

    using U = function_ref<int() const noexcept>;
    static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A>);
    static_assert(not std::is_constructible_v<U, nontype_t<&A::k>, A>);
    static_assert(
        not std::is_constructible_v<U, nontype_t<&A_nice::g>, A_nice>);

    A_nice w;
    dad({nontype<&A_nice::h>, w});
    dad({nontype<&A_nice::h>, &w});
    dad({nontype<&A_nice::h>, std::as_const(w)});
    dad({nontype<&A_nice::h>, A_nice{}});

    static_assert(not std::is_constructible_v<U, nontype_t<h>, A>);
    int h_good(A) noexcept;
    static_assert(std::is_constructible_v<U, nontype_t<h_good>, A>);
}