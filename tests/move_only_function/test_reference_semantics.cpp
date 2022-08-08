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
            };
        };
    };
};

using T = move_only_function<void() const>;
static_assert(not std::is_constructible_v<T, counter>,
              "counter's call op is not const");
static_assert(std::is_constructible_v<T, std::reference_wrapper<counter>>,
              "refwrap's call op is unconditionally const");
static_assert(
    not std::is_constructible_v<T, std::reference_wrapper<counter const>>,
    "refwrap's call op is constrained");
