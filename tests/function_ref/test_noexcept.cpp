#include "common_callables.h"

constexpr auto call = [](function_ref<int() noexcept> f)
{
    static_assert(noexcept(f()));
    return f();
};

int f_good() noexcept
{
    return BODYN(free_function);
}

struct A_good
{
    int g() noexcept { return BODYN('g'); }
};

int h_good(A) noexcept
{
    return BODYN('h');
}

suite noexcept_qualified = []
{
    using namespace bdd;

    feature("call will be noexcept") = []
    {
        given("a noexcept function") = []
        { expect(call(f_good) == free_function); };

        given("a noexcept closure") = []
        { expect(call([]() noexcept { return BODYN(1); }) == 1_i); };

        given("a noexcept structual callable") = []
        { expect(call(cw<f_good>) == free_function); };
    };

    feature("call from noexcept member function") = []
    {
        given("an object without operator()") = []
        {
            A_good x;

            when("binding by name") = [&]
            { expect(call({cw<&A_good::g>, x}) == ch<'g'>); };

            when("binding by pointer") = [&]
            { expect(call({cw<&A_good::g>, &x}) == ch<'g'>); };

            when("binding by reference_wrapper") = [&]
            {
                std::reference_wrapper r = x;
                expect(call({cw<&A_good::g>, r}) == ch<'g'>);
            };
        };

        given("an object without a noexcept member function") = []
        {
            A a;

            then("you can treat member access as a nothrow call") = [&]
            { expect(call({cw<&A::data>, a}) == 99_i); };

            then("you can treat a noexcept free function as its memfn") = [&]
            { expect(call({cw<h_good>, a}) == ch<'h'>); };
        };
    };
};

static_assert(std::is_invocable_v<decltype(foo), decltype(f)>);
static_assert(not std::is_invocable_v<decltype(call), decltype(f)>,
              "function may throw");

static_assert(std::is_invocable_v<decltype(foo), C>);
static_assert(not std::is_invocable_v<decltype(call), C>,
              "operator() may throw");

static_assert(std::is_invocable_v<decltype(foo), constant_wrapper<f>>);
static_assert(not std::is_invocable_v<decltype(call), constant_wrapper<f>>,
              "function may throw regardless whether its pointer is constant");

using T = function_ref<int()>;
using U = function_ref<int() noexcept>;

static_assert(std::is_constructible_v<T, constant_wrapper<&A::g>, A &>);
static_assert(not std::is_constructible_v<U, constant_wrapper<&A::g>, A &>,
              "member function may throw");

static_assert(std::is_constructible_v<T, constant_wrapper<h>, A &>);
static_assert(not std::is_constructible_v<U, constant_wrapper<h>, A &>,
              "explicit member function may throw");