#include "common_callables.h"

constexpr auto throwable_call = [](move_only_function<int()> f) { return f(); };

constexpr auto nothrow_call = [](move_only_function<int() noexcept> f)
{ return f(); };

template<class... T> struct overload : T...
{
    using T::operator()...;
};

constexpr auto call = overload{throwable_call, nothrow_call};

int f_good() noexcept
{
    return BODYN(free_function);
}

struct A_good : A
{
    int g() noexcept { return BODYN('g'); }
};

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
