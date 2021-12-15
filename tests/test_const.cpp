#include "common_callables.h"

void meow(function_ref<int() const> f)
{
    f();
}

struct C_mut
{
    int operator()()
    {
        return BODY();
    }
};

void test_const()
{
    static_assert(
        not std::is_invocable_v<decltype(meow),
                                decltype([i = 0]() mutable { return i; })>);

    meow([] { return BODY(); });
    meow([]() mutable { return BODY(); }); // function pointer

    static_assert(std::is_invocable_v<decltype(foo), C_mut>);
    static_assert(std::is_invocable_v<decltype(foo), C_mut &>);
    static_assert(std::is_invocable_v<decltype(foo), C_mut &&>);
    static_assert(not std::is_invocable_v<decltype(foo), C_mut const &>);
    static_assert(not std::is_invocable_v<decltype(foo), C_mut const &&>);

    static_assert(not std::is_invocable_v<decltype(meow), C_mut>);
    static_assert(not std::is_invocable_v<decltype(meow), C_mut &>);
    static_assert(not std::is_invocable_v<decltype(meow), C_mut &&>);
    static_assert(not std::is_invocable_v<decltype(meow), C_mut const &>);
    static_assert(not std::is_invocable_v<decltype(meow), C_mut const &&>);

    C c;
    C const cc;
    meow(c);
    meow(cc);
    meow(C{});
    meow(static_cast<C const &&>(cc));

    using T = function_ref<int()>;
    using U = function_ref<int() const>;

    static_assert(std::is_constructible_v<T, nontype_t<&A::g>, A &>);
    static_assert(not std::is_constructible_v<T, nontype_t<&A::g>, A>);

    static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A &>);
    static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A>);

    static_assert(std::is_constructible_v<T, nontype_t<&A::k>, A &>);
    static_assert(not std::is_constructible_v<T, nontype_t<&A::k>, A>);

    A a;
    meow({nontype<&A::k>, a});
    meow({nontype<&A::k>, A{}});
    meow({nontype<&A::k>, &a});
    meow({nontype<&A::k>, std::ref(a)});

    static_assert(not std::is_constructible_v<T, nontype_t<h>, A>);

    meow({nontype<h>, a});
    meow({nontype<h>, A{}});

    static_assert(not std::is_constructible_v<T, nontype_t<h>,
                                              std::reference_wrapper<A>>);
    static_assert(not std::is_constructible_v<T, nontype_t<h>,
                                              std::reference_wrapper<A const>>);

    meow({nontype<h>, std::ref(a)});  // h(std::ref(a))
    meow({nontype<h>, std::cref(a)}); // h(std::cref(a))

    static_assert(not std::is_constructible_v<T, nontype_t<h>, A *>);
    static_assert(not std::is_constructible_v<T, nontype_t<h>, A const *>);

    static_assert(not std::is_constructible_v<U, nontype_t<h>, A *>);
    static_assert(not std::is_constructible_v<U, nontype_t<h>, A const *>);
}