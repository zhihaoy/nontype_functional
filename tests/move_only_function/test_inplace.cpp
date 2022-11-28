#include "common_callables.h"

#include <numeric>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

class NotMovable : std::vector<int>
{
  public:
    NotMovable(NotMovable &&) = delete;
    NotMovable &operator=(NotMovable &&) = delete;

    using std::vector<int>::vector;

    int operator()(int initial, std::string &out)
    {
        auto r = std::accumulate(begin(), end(), initial);
        out = std::to_string(r);
        return r;
    }
};

static_assert(not std::is_move_constructible_v<NotMovable>);
static_assert(not std::is_move_assignable_v<NotMovable>);

class NotDefaultConstructible : public NotMovable
{
    using NotMovable::NotMovable;

    NotDefaultConstructible() = delete;
};

using T = move_only_function<void(int, std::string &)>;

struct copy_init
{
    void operator()(T);
};

suite inplace = []
{
    using std23::in_place_type;
    using namespace bdd;
    using type_traits::is_valid;

    feature("move_only_function does not require callable to be movable") = []
    {
        given("a default constructible target type") = []
        {
            T fn(in_place_type<NotMovable>);

            then("move_only_function behaves as if it is the target") = [&]
            {
                std::string s;
                fn(42, s);

                expect(s == "42"sv);
            };

            when("the move_only_function is moved into a new object") = [&]
            {
                auto fn2 = std::move(fn);

                then("the new object behaves as if it is the target") = [&]
                {
                    std::string s;
                    fn2(-1, s);

                    expect(s == "-1"sv);
                };
            };

            then("you may not in-place construct a function parameter") = []
            {
                static_assert(
                    not is_valid<copy_init>(
                        [](auto f) -> decltype(f(in_place_type<NotMovable>)) {
                        }),
                    "not a converting constructor");
            };
        };
    };

    feature("move_only_function can in-place construct using any ctors") = []
    {
        given("a target type without a default ctor") = []
        {
            T fn(in_place_type<NotDefaultConstructible>, 3u, 2); // [2, 2, 2]

            then("direct-non-list-initialization may be requested") = [&]
            {
                std::string s;
                fn(0, s);

                expect(s == "6"sv);
            };

            then("you may not in-place construct a function parameter") = []
            {
                static_assert(
                    not is_valid<copy_init>(
                        [](auto f) -> decltype(f(
                                       {in_place_type<NotDefaultConstructible>,
                                        3, 2})) {}),
                    "does not convert from braced-init-list");
            };
        };

        given("a target type with a initializer_list ctor") = []
        {
            std::allocator<int> a;
            T fn(in_place_type<NotDefaultConstructible>, {3, 1, 5, 4}, a);

            then("the ctor may be requested by passing braced-init-list") = [&]
            {
                std::string s;
                fn(0, s);

                expect(s == "13"sv);
            };

            then("you may not in-place construct a function parameter") = [&]
            {
                static_assert(
                    not is_valid<copy_init>(
                        [](auto f) -> decltype(f(
                                       {in_place_type<NotDefaultConstructible>,
                                        {3, 1, 5, 4}})) {}),
                    "does not convert from braced-init-list");
            };
        };

        feature("in-place construct types without operator()") = []
        {
            given("a type with user-defined ctors") = []
            {
                move_only_function<std::string()> fn(
                    nontype<[](std::vector<double> &v)
                            { return std::to_string(v.size()); }>,
                    in_place_type<std::vector<double>>, 4u);

                then("in-place construct") = [&] { expect(fn() == "4"sv); };
            };

            given("a type with a initializer_list ctor") = []
            {
                move_only_function<int()> fn(
                    nontype<[](std::string const &s) { return std::stoi(s); }>,
                    in_place_type<std::string>, {'5', '9', '2', '\0'});

                then("passing braced-init-list") = [&]
                { expect(fn() == 592_i); };
            };
        };
    };
};

static_assert(std::is_constructible_v<T, std::in_place_type_t<NotMovable>>,
              "std23::in_place_type should be as same as std::in_place_type");
static_assert(not std::is_constructible_v<
              T, std::in_place_type_t<NotDefaultConstructible>>);
static_assert(std::is_constructible_v<
              T, std::in_place_type_t<NotDefaultConstructible>, std::size_t>);

using W = move_only_function<int()>;
using Vec = std::vector<double>;

// extension
static_assert(
    std::is_nothrow_constructible_v<W, nontype_t<[](Vec *) { return 0; }>,
                                    std23::in_place_type_t<Vec *>>);
static_assert(std::is_nothrow_constructible_v<
              W, nontype_t<[](Vec &) { return 0; }>,
              std23::in_place_type_t<std::reference_wrapper<Vec>>, Vec &>);
