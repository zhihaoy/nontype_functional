#include "common_callables.h"

static_assert(std::is_invocable_v<decltype(foo), decltype(f)>);
static_assert(std::is_invocable_v<decltype(foo), decltype(&f)>);

auto get_h()
{
    return h;
}

int h_another(A const &)
{
    return 'h';
}

suite safety = []
{
    using namespace bdd;

    "safety"_test = []
    {
        given("a function_ref initialized from a function pointer") = []
        {
            function_ref fr = get_h();
            A a;

            then("it is not dangling") = [&]
            { expect(fr(a) == free_function); };

            when("it rebinds to a different function pointer") = [&]
            {
                fr = &h_another;

                then("it does not dangle either") = [&]
                { expect(fr(a) == ch<'h'>); };
            };

            when("it rebinds to a nttp pointer to member function") = [&]
            {
                fr = nontype<&A::g>;

                then("it never dangles") = [&] { expect(fr(a) == ch<'g'>); };
            };
        };
    };
};

using T = function_ref<int(A)>;

static_assert(not std::is_default_constructible_v<T>,
              "function_ref is not nullable");
static_assert(not std::is_constructible_v<T, decltype(&A::g)>,
              "function_ref doesn't initialize from member pointers");
static_assert(not std::is_constructible_v<T, decltype(&A::k)>);
static_assert(not std::is_constructible_v<T, decltype(&A::data)>);

using U = function_ref<int()>;

static_assert(std::is_trivially_copy_constructible_v<U>);
static_assert(std::is_trivially_copy_assignable_v<U>);
static_assert(std::is_nothrow_constructible_v<U, C>);
static_assert(not std::is_assignable_v<U, C>,
              "function_ref does not assign from types that may dangle");
static_assert(not std::is_assignable_v<U, C &>,
              "function_ref rejects lvalue of those types as well");
static_assert(not std::is_assignable_v<U &, C>);
static_assert(not std::is_assignable_v<U &, C &>);
