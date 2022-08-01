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

        given("a move_only_function initialized from function") = []
        {
            move_only_function<int()> fn = f;

            then("it is not empty") = [&]
            {
                expect(bool(fn));
                expect(fn != nullptr);
                expect(nullptr != fn);
            };

            when("called") = [&] { expect(fn() == 0_i); };
        };

        given("a move_only_function initialized from closure") = []
        {
            move_only_function<int()> fn = [] { return 42; };

            then("it is not empty") = [&]
            {
                expect(bool(fn));
                expect(fn != nullptr);
                expect(nullptr != fn);
            };

            when("called") = [&] { expect(fn() == 42_i); };
        };
    };
};
