#include "common_callables.h"

suite nullable = []
{
    using namespace bdd;

    feature("move_only_function recognizes certain nullable callable objects") =
        []
    {
        given("a non-empty move_only_function") = []
        {
            move_only_function<int()> fn = f;
            expect(fn != nullptr);

            when("it is assigned from a null function pointer") = [&]
            {
                decltype(f) *p = {};
                fn = p;

                then("it becomes empty") = [&] { expect(!fn); };
            };
        };

        given("a move_only_function initialized from a null PMF") = []
        {
            struct A
            {
                int data = 99;
            } a;
            int A::*pm = {};
            move_only_function<int(A)> fn = pm;

            then("it is empty") = [&] { expect(!fn); };

            when("it is assigned from a PM constant") = [&]
            {
                fn = &A::data;

                then("it is not empty") = [&]
                {
                    expect(bool(fn));
                    expect(fn(a) == 99_i);
                };
            };
        };

        given("an empty move_only_function") = []
        {
            move_only_function<int()> fn;
            expect(!fn);

            when("it initialises a different specialization") = [&]
            {
                move_only_function<long()> fn2 = std::move(fn);

                then("that object becomes empty") = [&] { expect(!fn2); };
            };
        };
    };
};
