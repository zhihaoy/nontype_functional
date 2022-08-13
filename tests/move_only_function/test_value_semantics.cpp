#include "common_callables.h"

#include <memory>

class move_counter
{
    std::unique_ptr<int> n_ = std::make_unique<int>(0);

  public:
    int operator()() & { return (*n_)++; }
};

suite value_semantics = []
{
    using namespace bdd;

    given("a stateful move_only_function") = []
    {
        move_only_function<int() &> fn = move_counter();

        when("it holds some state") = [&]
        {
            fn();
            fn();
            expect(fn() == 2_i);

            then("the object can be self-swapped") = [&]
            {
                swap(fn, fn);

                expect(fn != nullptr);
                expect(fn() == 3_i);
            };

            when("moving from the object") = [&]
            {
                auto fn2 = std::move(fn);

                then("the new object inherits the state") = [&]
                { expect(fn2() == 4_i); };

                then("self-move does not leak") = [&]
                {
                    fn2 = std::move(fn2);

                    expect(fn2 != nullptr); // extension
                };
            };
        };
    };

    given("two stateful move_only_function objects") = []
    {
        move_only_function<int() &> fn1 = move_counter();
        move_only_function<int() &> fn2 = move_counter();

        when("each holds different state") = [&]
        {
            fn1();
            expect(fn1() == 1_i);

            fn2();
            fn2();
            fn2();
            expect(fn2() == 3_i);

            then("swapping them exchanges their states") = [&]
            {
                swap(fn1, fn2);

                expect(fn1() == 4_i);
                expect(fn2() == 2_i);
            };
        };
    };
};

using T = move_only_function<void(int)>;
using R = T::result_type;

static_assert(std::is_nothrow_default_constructible_v<T>);
static_assert(std::is_nothrow_constructible_v<T, std::nullptr_t>);
static_assert(not std::is_copy_constructible_v<T>);
static_assert(not std::is_copy_assignable_v<T>);
static_assert(std::is_nothrow_assignable_v<T, std::nullptr_t>);
static_assert(std::is_nothrow_move_constructible_v<T>);
static_assert(std::is_nothrow_move_assignable_v<T>);
static_assert(std::is_nothrow_swappable_v<T>);

static_assert(std::is_same_v<std::invoke_result_t<T, char>, R>);

struct reject_rvalue
{
    reject_rvalue(reject_rvalue &) = default;

    void operator()(int) {}
    void mf(int) {}
};

static_assert(not std::is_move_constructible_v<reject_rvalue>);
static_assert(std::is_invocable_r_v<void, reject_rvalue, int>);

static_assert(std::is_constructible_v<T, reject_rvalue &>);
static_assert(not std::is_constructible_v<T, reject_rvalue>,
              "target object must be initialized");

static_assert(
    std::is_constructible_v<T, nontype_t<&reject_rvalue::mf>, reject_rvalue &>);
static_assert(not std::is_constructible_v<T, nontype_t<&reject_rvalue::mf>,
                                          reject_rvalue>,
              "bounded target object must be initialized");
