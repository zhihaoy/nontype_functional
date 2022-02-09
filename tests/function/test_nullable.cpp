#include "common_callables.h"

suite nullable = []
{
    using namespace bdd;

    feature("function recognizes certain nullable callable objects") = [&]
    {
        given("a non-empty function object") = [&]
        {
            function fn = f;
            expect(fn != nullptr);

            when("it is assigned from a null function pointer") = [=]() mutable
            {
                decltype(f) *p = {};
                fn = p;

                then("it becomes empty") = [&]
                {
                    expect(!fn);
                    expect(throws<std::bad_function_call>(fn));
                };
            };
        };

        given("a function initialized from a null pointer to member") = []
        {
            struct A
            {
                int data = 99;
            } a;
            int A::*pm = {};
            function<int(A)> fn = pm;

            then("it is empty") = [&]
            {
                expect(!fn);
                expect(throws<std::bad_function_call>(std::bind_front(fn, a)));
            };

            when("it is assigned from a PM constant") = [=]() mutable
            {
                fn = &A::data;

                then("it is not empty") = [&]
                {
                    expect(bool(fn));
                    expect(fn(a) == 99_i);
                };
            };
        };

        given("an empty function object") = []
        {
            function<int()> fn;
            expect(!fn);

            when("it is assigned from an empty std::function") = [=]() mutable
            {
                fn = std::function<int()>();

                then("it is not empty but behaves like empty") = [&]
                {
                    expect(fn != nullptr);
                    expect(throws<std::bad_function_call>(fn));
                };
            };

            when("it initialises function of a different signature") = [=]
            {
                function<long()> fn2 = fn;

                then("that function object is empty") = [&]
                {
                    expect(!fn2);
                    expect(throws<std::bad_function_call>(fn2));
                };
            };
        };
    };
};