#include "common_callables.h"

#include <algorithm>
#include <array>

static void noop(int &)
{}

inline constexpr auto behaves_like_noop = [](auto &f)
{
    int x = 0;
    f(x);
    return x == 0_i;
};

inline constexpr auto behaves_like_null = [](auto &f)
{
    int x = 0;
    return throws<std::bad_function_call>(std::bind_front(f, std::ref(x)));
};

inline constexpr auto behaves_like_nonpure = [](auto &f)
{
    int x = 0;
    f(x);
    return x == 1_i;
};

suite value_semantics = []
{
    using namespace bdd;

    feature("function is copyable") = []
    {
        struct counter
        {
            int n = 0;
            int operator()() { return n++; }
        };

        given("a stateful callable object") = []
        {
            counter cf;

            when("a function object is initialized from it") = [&]
            {
                function fn = cf;

                then("it owns a copy of that callable object") = [&]
                {
                    expect(fn() == 0_i);
                    expect(fn() == 1_i);
                    expect(fn() == 2_i);
                    expect(cf() == 0_i);
                    expect(cf() == 1_i);
                    expect(fn() == 3_i);
                };

                when("a copy of the function object is obtained") = [&]
                {
                    function fn2 = fn;

                    then("we get a distinct copy") = [&]
                    {
                        expect(fn() == 4_i);
                        expect(fn() == 5_i);
                        expect(fn2() == 4_i);
                    };

                    when("the original is set to empty") = [&]
                    {
                        fn = nullptr;

                        then("the copy is unaffected") = [&]
                        {
                            expect(!fn);
                            expect(bool(fn2));
                        };

                        when("assign from the copy") = [&]
                        {
                            fn = fn2;

                            then("we continue from the new state") = [&]
                            {
                                expect(fn() == 5_i);
                                expect(fn() == 6_i);
                                expect(fn2() == 5_i);
                            };
                        };
                    };
                };
            };
        };
    };

    feature("function is movable") = []
    {
        auto closure = [n = 1.](int x) mutable { return n *= x; };

        given("a function object that owns a stateful closure") = [&]
        {
            function fn = closure;

            when("its state updates") = [&]
            {
                expect(fn(2) == 2.0_d);
                expect(fn(3) == 6.0_d);

                then("it can be reset by move assignment") = [&]
                {
                    function other = closure;
                    fn = std::move(other);

                    expect(fn(3) == 3.0_d);

                    when("it is moved into a new object") = [&]
                    {
                        auto fn2{std::move(fn)};

                        then("the object inherits its states") = [&]
                        {
                            expect(bool(fn2));
                            expect(fn2(3) == 9.0_d);
                        };
                    };
                };
            };
        };
    };

    feature("function is swappable") = []
    {
        given("three function objects") = []
        {
            function a = noop;
            function<decltype(noop)> b = nullptr;
            function c = [](int &out) noexcept { out += 1; };

            then("they can be self swapped") = [=]() mutable
            {
                swap(a, a);
                expect(behaves_like_noop(a));

                swap(b, b);
                expect(behaves_like_null(b));

                swap(c, c);
                expect(behaves_like_nonpure(c));
            };

            when("swapping a with b") = [=]() mutable
            {
                swap(a, b);

                expect(behaves_like_null(a));
                expect(behaves_like_noop(b));
            };

            when("swapping b with c") = [=]() mutable
            {
                swap(b, c);

                expect(behaves_like_nonpure(b));
                expect(behaves_like_null(c));
            };

            when("swapping a with c") = [=]() mutable
            {
                swap(a, c);

                expect(behaves_like_nonpure(a));
                expect(behaves_like_noop(c));
            };

            when("rotate them") = [&]
            {
                std::array ls{a, b, c};
                auto first = begin(ls);
                std::rotate(first, std::next(first), end(ls));

                expect(behaves_like_null(ls[0]));    // b
                expect(behaves_like_nonpure(ls[1])); // c
                expect(behaves_like_noop(ls[2]));    // a
            };
        };
    };
};

using T = function<void(int)>;
using R = T::result_type;

static_assert(std::is_nothrow_default_constructible_v<T>);
static_assert(std::is_nothrow_constructible_v<T, std::nullptr_t>);
static_assert(std::is_copy_constructible_v<T>);
static_assert(std::is_copy_assignable_v<T>);
static_assert(std::is_nothrow_assignable_v<T, std::nullptr_t>);
static_assert(std::is_nothrow_move_constructible_v<T>);
static_assert(std::is_nothrow_move_assignable_v<T>);
static_assert(std::is_nothrow_swappable_v<T>);

static_assert(std::is_same_v<std::invoke_result_t<T, char>, R>);
static_assert(std::is_same_v<std::invoke_result_t<T const, char>, R>);

struct move_only
{
    move_only(move_only const &) = delete;
    move_only(move_only &&) = default;

    void operator()(int) {}
};

static_assert(std::is_move_constructible_v<move_only>);
static_assert(std::is_invocable_r_v<void, move_only, int>);

static_assert(not std::is_constructible_v<T, move_only>,
              "target object must be copy-constructible");
static_assert(not std::is_constructible_v<T, nontype_t<&move_only::operator()>,
                                          move_only>,
              "bounded target object must be copy-constructible");

struct reject_rvalue
{
    reject_rvalue(reject_rvalue &&) = delete;
    reject_rvalue(reject_rvalue const &) = default;

    void operator()(int) {}
};

static_assert(std::is_copy_constructible_v<reject_rvalue>);
static_assert(std::is_invocable_r_v<void, reject_rvalue, int>);

static_assert(std::is_constructible_v<T, reject_rvalue &>);
static_assert(not std::is_constructible_v<T, reject_rvalue>,
              "target object must be initialized");

static_assert(std::is_constructible_v<T, nontype_t<&reject_rvalue::operator()>,
                                      reject_rvalue &>);
static_assert(not std::is_constructible_v<
                  T, nontype_t<&reject_rvalue::operator()>, reject_rvalue>,
              "bounded target object must be initialized");
