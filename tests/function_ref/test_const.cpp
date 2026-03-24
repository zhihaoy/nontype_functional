#include "common_callables.h"

constexpr auto call = [](function_ref<int() const> f) { return f(); };

struct C_mut
{
    int operator()() { return BODYN(non_const); }
};

suite const_qualified = []
{
    using namespace bdd;

    feature("callable as const") = []
    {
        given("a closure") = []
        { expect(call([] { return BODYN(1); }) == 1_i); };

        given("a mutable callable object") = []
        {
            C c;
            expect(call(c) == const_);
        };

        given("an immutable callable object") = []
        {
            C const c;
            expect(call(c) == const_);
        };

        given("a prvalue") = [] { expect(call(C{}) == const_); };
        given("a const xvalue") = []
        {
            C const cc;
            expect(call(static_cast<C const &&>(cc)) == const_);
        };
    };

    feature("call from const member function") = []
    {
        given("an object without operator()") = []
        {
            A a;

            when("binding by name") = [&]
            { expect(call({nontype<&A::k>, a}) == ch<'k'>); };

            when("binding by pointer") = [&]
            { expect(call({nontype<&A::k>, &a}) == ch<'k'>); };

            when("binding by reference_wrapper") = [&]
            {
                std::reference_wrapper r = a;
                expect(call({nontype<&A::k>, r}) == ch<'k'>);
            };

            when("binding free function by name") = [&]
            { expect(call({nontype<h>, a}) == free_function); };
        };
    };

    feature("non-const signature can rebind to const signature") = []
    {
        given("non-const function_ref initialized from const") = []
        {
            C c1;
            function_ref<int() const> r1 = c1;
            function_ref<int()> r2 = r1;

            when("assign something else to the const one") = [&]
            {
                r1 = f;

                then("the non-const one is unaffected") = [&]
                {
                    expect(r1() == free_function);
                    expect(r2() == const_);
                };
            };
        };
    };
};

static_assert(not std::is_invocable_v<decltype(call), decltype([i = 0]() mutable
                                                               { return i; })>,
              "const-qualified signature cannot reference mutable lambda");

static_assert(std::is_invocable_v<decltype(foo), C_mut>);
static_assert(std::is_invocable_v<decltype(foo), C_mut &>);
static_assert(std::is_invocable_v<decltype(foo), C_mut &&>);
static_assert(not std::is_invocable_v<decltype(foo), C_mut const &>,
              "const object is not callable");
static_assert(not std::is_invocable_v<decltype(foo), C_mut const &&>);

static_assert(not std::is_invocable_v<decltype(call), C_mut>,
              "const-qualified rvalue is not callable");
static_assert(not std::is_invocable_v<decltype(call), C_mut &>,
              "const-qualified lvalue is not callable");
static_assert(not std::is_invocable_v<decltype(call), C_mut &&>);
static_assert(not std::is_invocable_v<decltype(call), C_mut const &>);
static_assert(not std::is_invocable_v<decltype(call), C_mut const &&>);

using T = function_ref<int()>;
using U = function_ref<int() const>;

static_assert(std::is_constructible_v<T, nontype_t<&A::g>, A &>);
static_assert(not std::is_constructible_v<T, nontype_t<&A::g>, A>,
              "cannot bind rvalue");
static_assert(not std::is_constructible_v<T, nontype_t<&A::g>, A const>);

static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A &>);
static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A>);

static_assert(std::is_constructible_v<T, nontype_t<&A::k>, A &>);
static_assert(not std::is_constructible_v<T, nontype_t<&A::k>, A>);

static_assert(not std::is_constructible_v<T, nontype_t<h>, A>,
              "free function does not bind rvalue either");
static_assert(not std::is_constructible_v<T, nontype_t<h>, A const>);

static_assert(not std::is_constructible_v<T, nontype_t<h>, A *>,
              "free function does not bind pointers");
static_assert(not std::is_constructible_v<T, nontype_t<h>, A const *>);

static_assert(not std::is_constructible_v<U, nontype_t<h>, A *>);
static_assert(not std::is_constructible_v<U, nontype_t<h>, A const *>);

static_assert(not std::is_assignable_v<U, T>);
static_assert(std::is_convertible_v<U, T>);
static_assert(std::is_nothrow_constructible_v<T, U>);
static_assert(std::is_nothrow_assignable_v<T, U>,
              "non-const rebind from const");
