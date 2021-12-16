#include "common_callables.h"

static_assert(std::is_invocable_v<decltype(foo), decltype(f)>);
static_assert(std::is_invocable_v<decltype(foo), decltype(&f)>);

auto get_h()
{
    return h;
}

void test_safety()
{
    using T = function_ref<int(A)>;
    static_assert(std::is_constructible_v<T, decltype(h)>);
    static_assert(std::is_constructible_v<T, decltype(&h)>);
    static_assert(not std::is_constructible_v<T, decltype(&A::g)>);
    static_assert(not std::is_constructible_v<T, decltype(&A::k)>);
    static_assert(not std::is_constructible_v<T, decltype(&A::data)>);

    static_assert(std::is_assignable_v<T &, decltype(h)>);
    static_assert(std::is_assignable_v<T &, decltype(&h)>);
    static_assert(not std::is_assignable_v<T &, decltype(&A::g)>);
    static_assert(not std::is_assignable_v<T &, decltype(&A::k)>);
    static_assert(not std::is_assignable_v<T &, decltype(&A::data)>);

    using U = function_ref<int(A) const>;
    static_assert(std::is_constructible_v<U, decltype(h)>);
    static_assert(std::is_constructible_v<U, decltype(&h)>);
    static_assert(not std::is_constructible_v<U, decltype(&A::g)>);
    static_assert(not std::is_constructible_v<U, decltype(&A::k)>);
    static_assert(not std::is_constructible_v<U, decltype(&A::data)>);

    static_assert(std::is_assignable_v<U &, decltype(h)>);
    static_assert(std::is_assignable_v<U &, decltype(&h)>);
    static_assert(not std::is_assignable_v<U &, decltype(&A::g)>);
    static_assert(not std::is_assignable_v<U &, decltype(&A::k)>);
    static_assert(not std::is_assignable_v<U &, decltype(&A::data)>);

    {
        T fr = get_h();
        A a;
        fr(a);
        fr = &h;
        fr(a);
    }

    {
        T fr;
        fr = nontype<[](A) { return BODY(); }>;
        A a;
        fr(a);
    }

    {
        function_ref fr = f;
        fr = std::ref(f);
    }

    {
        T fr;
        auto fn = [](A) { return BODY(); };
        fr = std::ref(fn);
        A a;
        fr(a);
    }
}