#include "common_callables.h"

constexpr auto call = [](function_ref<int()> f) { return f(); };

suite nttp_callable = []
{
    using namespace bdd;

    feature("unbound instance method") = []
    {
        given("a function") = [] { expect(call(cw<f>) == free_function); };

        given("a function template specialization") = []
        { expect(call(cw<g<int>>) == function_template); };

        given("a closure") = []
        { expect(call(cw<[] { return BODYN(42); }>) == 42_i); };
    };

    feature("bound instance method") = []
    {
        given("a const object and a non-const object") = []
        {
            A a;
            A const b;

            when("binding non-const method to non-const object") = [&]
            {
                expect(call({cw<&A::g>, a}) == ch<'g'>);
                expect(call({cw<&A::g>, &a}) == ch<'g'>);
            };

            when("binding const method to non-const object") = [&]
            {
                expect(call({cw<&A::k>, a}) == ch<'k'>);
                expect(call({cw<&A::k>, &a}) == ch<'k'>);
            };

            when("binding const method to const object") = [&]
            {
                expect(call({cw<&A::k>, b}) == ch<'k'>);
                expect(call({cw<&A::k>, &b}) == ch<'k'>);
            };

            when("binding pointer to data member to object") = [&]
            {
                expect(call({cw<&A::data>, a}) == 99_i);
                expect(call({cw<&A::data>, &a}) == 99_i);
            };

            when("binding free function to object") = [&]
            { expect(call({cw<h>, a}) == free_function); };

            when("binding closure to object") = [&] {
                expect(call({cw<[](A p) { return BODYN(p.data); }>, a}) ==
                       99_i);
            };

            when("binding closure to pointer") = [&] {
                expect(call({cw<[](A *p) { return BODYN(p->data); }>, &a}) ==
                       99_i);
            };

            when("passing objects using reference_wrapper") = [&]
            {
                std::reference_wrapper r = b;
                expect(call({cw<&A::data>, r}) == 99_i);
                expect(call({cw<h>, r}) == free_function);
            };
        };
    };
};
