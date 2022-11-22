#include "common_callables.h"

template<class T> T &&identity_fn(T &&x)
{
    return x;
}

struct identity_fobj
{
    int x;

    int const &operator()() const { return x; }
};

suite return_reference = []
{
    using namespace bdd;

    feature("reference return value") = []
    {
        given("a variable") = []
        {
            int x;
            function_ref<int &(int &)> fn = identity_fn<int &>;
            expect(std::addressof(fn(x)) == &x);
        };

        given("a data member") = []
        {
            identity_fobj obj;

            when("bind to a non-const wrapper") = [&]
            {
                function_ref<int const &()> fn = obj;
                expect(std::addressof(fn()) == &obj.x);
            };

            when("bind to a const wrapper") = [&]
            {
                function_ref<int const &() const> fn = obj;
                expect(std::addressof(fn()) == &obj.x);
            };
        };

        given("a pointer to member object") = []
        {
            identity_fobj obj;

            when("used as an unbound method") = [=]
            {
                function_ref<int const &(identity_fobj const &)> fn =
                    nontype<&identity_fobj::x>;
                expect(std::addressof(fn(obj)) == &obj.x);
            };

            when("used as a non-const bound method") = [=]() mutable
            {
                function_ref fn = {nontype<&identity_fobj::x>, obj};
                expect(std::addressof(fn()) == &obj.x);
            };

            when("used as a const bound method") = [=]
            {
                function_ref fn = {nontype<&identity_fobj::x>, obj};
                expect(std::addressof(fn()) == &obj.x);
            };
        };
    };
};
