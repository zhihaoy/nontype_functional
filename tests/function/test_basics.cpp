#include "common_callables.h"

suite basics = []
{
    using namespace bdd;

    "basics"_test = []
    {
        given("a default constructed function object") = []
        {
            function<int()> fn;

            then("it is empty") = [&]
            {
                expect(!fn);
                expect(fn == nullptr);
                expect(nullptr == fn);
            };

            when("called") = [&]
            { expect(throws<std::bad_function_call>(fn)); };
        };

        given("a function object initialized from function") = []
        {
            function fn = f;

            then("it is not empty") = [&]
            {
                expect(bool(fn));
                expect(fn != nullptr);
                expect(nullptr != fn);
            };

            when("called") = [&] { expect(fn() == 0_i); };
        };

        given("a function object initialized from closure") = []
        {
            function fn = [] { return 42; };

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