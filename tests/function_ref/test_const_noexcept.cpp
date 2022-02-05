#include "common_callables.h"

struct D
{
    int operator()() noexcept { return BODYN(11); }
    int operator()() const noexcept { return BODYN(22); }
};

constexpr auto unqual_call = [](function_ref<int()> f) { return f(); };
constexpr auto call = [](function_ref<int() const noexcept> f) { return f(); };

struct A_nice
{
    int g() noexcept { return BODYN('g'); }
    int h() const noexcept { return BODYN('h'); }
};

suite const_noexcept_qualified = []
{
    using namespace bdd;

    feature("callable is const noexcept") = []
    {
        given("a const object and a non-const object") = []
        {
            D d;
            D const cd;

            when("unqualified signature call non-const object") = [&]
            { expect(unqual_call(d) == 11_i); };

            when("qualified signature call non-const object") = [&]
            { expect(call(d) == 22_i); };

            when("unqualified signature call const object") = [&]
            { expect(unqual_call(cd) == 22_i); };

            when("qualified signature call const object") = [&]
            { expect(call(cd) == 22_i); };
        };

        given("a noexcept closure") = []
        { expect(call([]() noexcept { return BODYN(42); }) == 42_i); };
    };

    feature("call from const noexcept member function") = []
    {
        given("a non-const object with no qualified member function") = []
        {
            A a;

            then("pointer to data member is deemed a pure accessor") = [&]
            {
                expect(call({nontype<&A::data>, a}) == 99_i);
                expect(call({nontype<&A::data>, &a}) == 99_i);

                std::reference_wrapper r = a;
                expect(call({nontype<&A::data>, r}) == 99_i);
            };
        };

        given("a non-const object with qualified member functions") = []
        {
            A_nice w;

            then("can call const member function") = [&]
            {
                expect(call({nontype<&A_nice::h>, w}) == ch<'h'>);
                expect(call({nontype<&A_nice::h>, &w}) == ch<'h'>);
            };

            then("can call const member function on const lvalue") = [&] {
                expect(call({nontype<&A_nice::h>, std::as_const(w)}) ==
                       ch<'h'>);
            };
        };
    };
};

static_assert(
    not std::is_invocable_v<decltype(call), decltype([] { return 0; })>,
    "noexcept signature does not accept potentially-throwing lambda");

static_assert(
    not std::is_invocable_v<decltype(call),
                            decltype([i = 0]() mutable noexcept { return i; })>,
    "const noexcept signature does not accept noexcept but mutable lambda");

using U = function_ref<int() const noexcept>;

static_assert(not std::is_constructible_v<U, nontype_t<&A::data>, A>,
              "cannot bind rvalue");

static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A &>,
              "not qualified");
static_assert(not std::is_constructible_v<U, nontype_t<&A::k>, A &>,
              "not noexcept-qualified");
static_assert(not std::is_constructible_v<U, nontype_t<&A_nice::g>, A_nice &>,
              "not const-qualified");

static_assert(not std::is_constructible_v<U, nontype_t<&A_nice::h>, A_nice>,
              "cannot bind rvalue");

static_assert(not std::is_constructible_v<U, nontype_t<h>, A &>,
              "not noexcept");
int h_good(A) noexcept;
static_assert(std::is_constructible_v<U, nontype_t<h_good>, A &>);
