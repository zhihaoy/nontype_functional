#include "common_callables.h"

// static_assert(std::is_invocable_v<decltype(foo), decltype(f)>);
static_assert(std::is_convertible_v<decltype(f), function_ref<int()>>);
static_assert(not std::is_convertible_v<decltype(&f), function_ref<int()>>);

void test_safety()
{
    using T = function_ref<int(A)>;
    static_assert(std::is_constructible_v<T, decltype(h)>);
    static_assert(not std::is_constructible_v<T, decltype(&h)>);
    static_assert(not std::is_constructible_v<T, decltype(&A::g)>);
    static_assert(not std::is_constructible_v<T, decltype(&A::k)>);
    static_assert(not std::is_constructible_v<T, decltype(&A::data)>);

    static_assert(std::is_assignable_v<T &, decltype(h)>);
    static_assert(not std::is_assignable_v<T &, decltype(&h)>);
    static_assert(not std::is_assignable_v<T &, decltype(&A::g)>);
    static_assert(not std::is_assignable_v<T &, decltype(&A::k)>);
    static_assert(not std::is_assignable_v<T &, decltype(&A::data)>);

    using U = function_ref<int(A) const>;
    static_assert(std::is_constructible_v<U, decltype(h)>);
    static_assert(not std::is_constructible_v<U, decltype(&h)>);
    static_assert(not std::is_constructible_v<U, decltype(&A::g)>);
    static_assert(not std::is_constructible_v<U, decltype(&A::k)>);
    static_assert(not std::is_constructible_v<U, decltype(&A::data)>);

    static_assert(std::is_assignable_v<U &, decltype(h)>);
    static_assert(not std::is_assignable_v<U &, decltype(&h)>);
    static_assert(not std::is_assignable_v<U &, decltype(&A::g)>);
    static_assert(not std::is_assignable_v<U &, decltype(&A::k)>);
    static_assert(not std::is_assignable_v<U &, decltype(&A::data)>);
}