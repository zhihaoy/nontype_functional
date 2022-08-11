#include "common_callables.h"

#include <optional>

using T = move_only_function<int() const>;

suite nttp_callable = []
{
    using namespace bdd;

    feature("type-erase a bound instance method") = []
    {
        given("an object without call operator") = []
        {
            auto obj = std::make_optional<A>();

            when("binding a pointer-to-member to a pointer to the object") = [&]
            {
                T fn = {nontype<&A::data>, &obj.value()};

                then("you can observe reference semantics") = [&]
                {
                    obj->data = 27;
                    expect(fn() == 27_i);
                };

                when("rebinding a copy of the object") = [&]
                {
                    fn = {nontype<&A::data>, obj};

                    then("you can observe value semantics instead") = [&]
                    {
                        obj->data = 84;
                        expect(fn() == 27_i);
                    };

                    static_assert(std::is_constructible_v<T, nontype_t<h>,
                                                          decltype(*obj)>);
                    static_assert(
                        not std::is_constructible_v<T, nontype_t<h>,
                                                    decltype(obj)>,
                        "non-member does not dereference pointer-like objects");
                    using U = decltype(obj);
                    static_assert(
                        std::is_constructible_v<
                            T, nontype_t<[](U const &x) { return h(*x); }>, U>,
                        "...but you can DIY");
                };
            };
        };
    };

    feature("type-erase an unbound instance method") = []
    {
        given("a call wrapper and its argument") = [&]
        {
            move_only_function<int(A &)> fn;
            A obj;

            when("move_only_function is assigned a pointer-to-member") = [&]
            {
                fn = nontype<&A::data>;

                then("it behaves as if it is memfn") = [&]
                {
                    obj.data = 33;
                    expect(fn(obj) == 33);
                };
            };

            when("move_only_function is assigned a function") = [&]
            {
                fn = nontype<h>;

                then("it behaves as if it is the function") = [&]
                { expect(fn(obj) == free_function); };
            };
        };
    };
};

using U = move_only_function<int(A)>;

static_assert(std::is_nothrow_constructible_v<U, nontype_t<&A::data>>);
static_assert(std::is_nothrow_constructible_v<U, nontype_t<h>>,
              "unbounded cases are always noexcept");
static_assert(std::is_nothrow_assignable_v<U, nontype_t<&A::data>>);
static_assert(std::is_nothrow_assignable_v<U, nontype_t<h>>);

static_assert(std::is_constructible_v<T, nontype_t<&A::data>, A>);
static_assert(std::is_constructible_v<T, nontype_t<h>, A>,
              "initializing bounded objects potentially throws");

// extension
static_assert(std::is_nothrow_constructible_v<T, nontype_t<&A::data>, A *>,
              "...unless we stored a pointer");
static_assert(std::is_nothrow_constructible_v<T, nontype_t<&A::data>,
                                              std::reference_wrapper<A>>,
              "...or reference_wrapper");
