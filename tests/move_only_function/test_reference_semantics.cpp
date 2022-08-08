#include "common_callables.h"

struct counter
{
    int n = 0;
    void operator()() & { ++n; }
};

suite reference_semantics = []
{
    using namespace bdd;

    feature("move_only_function supports reference_wrapper") = []
    {
        given("a stateful callable object") = []
        {
            counter obj;

            when("initializes move_only_function via refwrap") = [=]() mutable
            {
                move_only_function<void()> fn = std::ref(obj);

                then("calling the it affects the original object") = [&]
                {
                    fn();
                    fn();

                    expect(obj.n == 2_i);
                };

                given("a different stateful object") = [&]
                {
                    counter rhs;

                    when("swapping with a different move_only_function that "
                         "refers to it") = [&]
                    {
                        move_only_function<void()> fn2(
                            std23::in_place_type<
                                std::reference_wrapper<counter>>,
                            rhs);
                        swap(fn, fn2);

                        then("their references are swapped") = [&]
                        {
                            expect(obj.n == 2_i);
                            expect(rhs.n == 0_i);

                            fn();
                            expect(rhs.n == 1_i);

                            fn2();
                            expect(obj.n == 3_i);
                        };
                    };
                };
            };

            when("initializes const-correct move_only_function") = [=]() mutable
            {
                obj.n = 3;
                move_only_function<void() const> fn = std::ref(obj);

                then("const-correctness is broke by refwrap") = [&]
                {
                    fn();

                    expect(obj.n == 4_i);
                };

                using T = decltype(fn);
                static_assert(not std::is_constructible_v<T, counter>,
                              "counter's call op is not const");
                static_assert(
                    std::is_constructible_v<T, std::reference_wrapper<counter>>,
                    "refwrap's call op is unconditionally const");
                static_assert(not std::is_constructible_v<
                                  T, std::reference_wrapper<counter const>>,
                              "refwrap's call op is constrained");
            };
        };
    };
};

using T = move_only_function<void(int)>;
using X = decltype([](int) {});

static_assert(std::is_constructible_v<T, X>);
static_assert(std::is_constructible_v<T, std::reference_wrapper<X>>);
static_assert(std::is_assignable_v<T, X>);
static_assert(std::is_constructible_v<T, std::reference_wrapper<X>>);

static_assert(std::is_constructible_v<T, std::in_place_type_t<X>>);
static_assert(std::is_constructible_v<
              T, std::in_place_type_t<std::reference_wrapper<X>>, X &>);

// extension
static_assert(std::is_nothrow_constructible_v<T, std::reference_wrapper<X>>);
static_assert(std::is_nothrow_assignable_v<T, std::reference_wrapper<X>>);
static_assert(std::is_nothrow_constructible_v<T, void (&)(int)>);
static_assert(std::is_nothrow_constructible_v<T, void (*)(int)>);
static_assert(std::is_nothrow_assignable_v<T, void (&)(int)>);
static_assert(std::is_nothrow_assignable_v<T, void (*)(int)>);

static_assert(std::is_nothrow_constructible_v<
              T, std::in_place_type_t<std::reference_wrapper<X>>, X &>);
static_assert(std::is_nothrow_constructible_v<
              T, std::in_place_type_t<void (*)(int)>, void(int)>);
static_assert(
    std::is_nothrow_constructible_v<T, std::in_place_type_t<void (*)(int)>, X>);
