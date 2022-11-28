#include "common_callables.h"

template<class T> T &&identity_fn(T &&x)
{
    return x;
}

template<class T> class refwrap
{
    T &x;

  public:
    refwrap(T &obj) : x(obj) {}

    T &operator()() const { return x; }
    T &get() const { return x; }
};

suite return_reference = []
{
    using namespace bdd;

    feature("reference return value") = []
    {
        given("a variable") = []
        {
            int x;
            move_only_function<int &(int &)> fn = identity_fn<int &>;
            expect(std::addressof(fn(x)) == &x);
        };

        given("a data member") = []
        {
            int x;
            refwrap obj = x;

            when("bind to a non-const wrapper") = [&]
            {
                move_only_function<int &()> fn = obj;
                expect(std::addressof(fn()) == &x);
            };

            when("bind to a const wrapper") = [&]
            {
                move_only_function<int &() const> fn = obj;
                expect(std::addressof(fn()) == &x);
            };
        };

        given("a pointer to member object") = []
        {
            int x;
            refwrap obj = x;

            when("used as an unbound method") = [&]
            {
                move_only_function<int &(refwrap<int> const &)> fn =
                    nontype<&refwrap<int>::get>;
                expect(std::addressof(fn(obj)) == &x);
            };

            when("used as a non-const bound method") = [&]
            {
                move_only_function<int &()> fn = {nontype<&refwrap<int>::get>,
                                                  obj};
                expect(std::addressof(fn()) == &x);
            };

            when("used as a const bound method") = [&]
            {
                move_only_function<int &() const> fn = {
                    nontype<&refwrap<int>::get>, obj};
                expect(std::addressof(fn()) == &x);
            };
        };
    };
};
