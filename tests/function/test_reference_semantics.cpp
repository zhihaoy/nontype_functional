#include "common_callables.h"

suite reference_semantics = []
{
    using namespace bdd;

    feature("function supports reference_wrapper") = []
    {
        struct counter
        {
            int n = 0;
            int operator()() { return ++n; }
        };

        given("a stateful callable object") = []
        {
            counter obj;

            when("passing it to function via refwrap") = [&]
            {
                function<int()> fn = std::ref(obj);

                then("calling function affects the original object") = [&]
                {
                    expect(fn() == 1_i);
                    expect(fn() == 2_i);

                    expect(obj.n == 2_i);
                };

                then("a copy of the function also references the source") =
                    [fn, &obj]
                {
                    expect(fn() == 3_i);
                    expect(obj.n == 3_i);
                };

                when("reseting the function with a copy of the source") = [&]
                {
                    fn = obj;

                    then("they maintain disjointed states") = [&]
                    {
                        obj.n = 10;

                        expect(fn() == 4_i);
                    };

                    when("the function reattaches to the source") = [&]
                    {
                        fn = std::ref(obj);

                        then("they share states again") = [&]
                        {
                            expect(obj() == 11_i);
                            expect(fn() == 12_i);
                        };
                    };
                };
            };
        };
    };
};

using T = function<void(int)>;
using X = decltype([](int) {});

static_assert(std::is_constructible_v<T, X>);
static_assert(std::is_constructible_v<T, std::reference_wrapper<X>>);
static_assert(std::is_assignable_v<T, X>);
static_assert(std::is_nothrow_assignable_v<T, std::reference_wrapper<X>>);

// extension
static_assert(std::is_nothrow_constructible_v<T, std::reference_wrapper<X>>);
static_assert(std::is_nothrow_constructible_v<T, void (&)(int)>);
static_assert(std::is_nothrow_constructible_v<T, void (*)(int)>);
static_assert(std::is_nothrow_assignable_v<T, void (&)(int)>);
static_assert(std::is_nothrow_assignable_v<T, void (*)(int)>);
