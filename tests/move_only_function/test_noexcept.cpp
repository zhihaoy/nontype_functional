#include "common_callables.h"

constexpr auto throwable_call = [](move_only_function<int()> f) { return f(); };

constexpr auto nothrow_call = [](move_only_function<int() noexcept> f)
{ return f(); };

template<class... T> struct overload : T...
{
    using T::operator()...;
};

template<class... T> overload(T...) -> overload<T...>;

constexpr auto call = overload{throwable_call, nothrow_call};

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

    feature("noexcept signature accepts nothrow callable objects") = []
    {
        given("a noexcept function") = []
        { expect(nothrow_call(f_good) == free_function); };

        given("a noexcept closure") = []
        {
            expect(nothrow_call([]() noexcept { return BODYN(noexcept_); }) ==
                   noexcept_);
        };

        given("a noexcept structual callable") = []
        { expect(nothrow_call(nontype<f_good>) == free_function); };
    };

    feature("noexcept signature allows binding nothrow nttp") = []
    {
        given("an object without operator()") = []
        {
            A_good x;

            when("binding by name") = [&] {
                expect(nothrow_call({nontype<&A_good::g>, x}) == ch<'g'>);
            };

            when("binding by pointer") = [&] {
                expect(nothrow_call({nontype<&A_good::g>, &x}) == ch<'g'>);
            };

            when("binding by reference_wrapper") = [&] {
                expect(nothrow_call({nontype<&A_good::g>, std::ref(x)}) ==
                       ch<'g'>);
            };
        };

        given("an object without a noexcept member function") = []
        {
            A a;

            then("you can treat member access as a nothrow call") = [&] {
                expect(nothrow_call({nontype<&A::data>, a}) == 99_i);
            };

            then("you can treat a noexcept free function as its memfn") = [&] {
                expect(nothrow_call({nontype<h_good>, a}) == ch<'h'>);
            };
        };
    };

    feature("noexcept signature and unqual signature can overload") = []
    {
        static_assert(not std::is_invocable_v<decltype(call), decltype(f_good)>,
                      "a noexcept function is ambiguous to this overload set");

        given("a function that potentially throws") = []
        {
            then("it binds only to the throwable signature") = []
            { expect(call(f) == free_function); };
        };
    };
};

static_assert(std::is_invocable_v<decltype(throwable_call), decltype(f)>);
static_assert(std::is_invocable_v<decltype(throwable_call), decltype(f_good)>);
static_assert(not std::is_invocable_v<decltype(nothrow_call), decltype(f)>);
static_assert(std::is_invocable_v<decltype(nothrow_call), decltype(f_good)>);

using X = move_only_function<int(A)>;
using Y = move_only_function<int(A) noexcept>;
using Xp = move_only_function<int(A_good)>;
using Yp = move_only_function<int(A_good) noexcept>;

static_assert(std::is_invocable_v<X, A>);
static_assert(not std::is_nothrow_invocable_v<X, A>);
static_assert(std::is_nothrow_invocable_v<Y, A>);

static_assert(std::is_constructible_v<X, decltype(&A::g)>);
static_assert(std::is_constructible_v<Xp, decltype(&A_good::g)>);
static_assert(std::is_constructible_v<X, decltype(&A::data)>);
static_assert(not std::is_constructible_v<Y, decltype(&A::g)>);
static_assert(std::is_constructible_v<Yp, decltype(&A_good::g)>);
static_assert(std::is_constructible_v<Y, decltype(&A::data)>,
              "pointer-to-data-member is deemed noexcept");

using T = move_only_function<int()>;
using U = move_only_function<int() noexcept>;

static_assert(std::is_constructible_v<T, nontype_t<&A::g>, A>);
static_assert(not std::is_constructible_v<U, nontype_t<&A::g>, A>,
              "member function may throw");
static_assert(std::is_constructible_v<T, nontype_t<&A_good::g>, A_good>);
static_assert(std::is_constructible_v<U, nontype_t<&A_good::g>, A_good>);
static_assert(std::is_constructible_v<T, nontype_t<h>, A>);
static_assert(not std::is_constructible_v<U, nontype_t<h>, A>,
              "explicit member function may throw");
static_assert(std::is_constructible_v<T, nontype_t<h_good>, A>);
static_assert(std::is_constructible_v<U, nontype_t<h_good>, A>);
