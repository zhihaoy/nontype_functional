#include "common_callables.h"

suite basics = []
{
    using namespace bdd;

    "basics"_test = []
    {
        given("a default constructed move_only_function") = []
        {
            move_only_function<int()> fn;

            then("it is empty") = [&]
            {
                expect(!fn);
                expect(fn == nullptr);
                expect(nullptr == fn);
            };
        };
    };
};
