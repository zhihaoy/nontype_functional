#include "common_callables.h"

constexpr auto call = [](function_ref<int()> fr) { return fr(); };

suite basics = []
{
    using namespace bdd;

    "basics"_test = []
    {
        given("a function") = [] { expect(call(f) == free_function); };

        given("a closure") = []
        { expect(call([] { return BODYN(1); }) == 1_i); };

        given("a function template specialization") = []
        { expect(call(g<int>) == function_template); };

        given("a mutable callable object") = []
        {
            C c;
            expect(call(c) == non_const);
        };

        given("an immutable callable object") = []
        {
            C const c;
            expect(call(c) == const_);
        };
    };
};
