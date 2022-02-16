#include "common_callables.h"

#include <string>

struct Base
{
    int &n;
};

class Track : Base
{
  public:
    Track(int &target) : Base{target} {}
    Track(Track const &other) : Base(other) { ++n; }
};

static_assert(not std::is_trivially_copy_constructible_v<Track>);
static_assert(not std::is_trivially_move_constructible_v<Track>);

static void make_call(Track &&)
{}

inline constexpr char some_str[] = "if you see this, then we're fine";

static auto f_str()
{
    return some_str;
};

suite call_pattern = []
{
    using namespace bdd;

    "call_pattern"_test = []
    {
        given("a signature that takes parameter by value") =
            []<template<class> class C, class T>(C<T> ty)
        {
            boost::ut::log << ty;

            T fr = make_call;
            int n = 0;

            when("passing rvalue argument") = [=]() mutable
            {
                fr(Track{n});

                then("no copy is made") = [&] { expect(n == 0_i); };
            };

            when("passing lvalue argument") = [=]() mutable
            {
                Track t{n};
                fr(t);

                then("made at most one copy") = [&] { expect(n <= 1_i); };
            };
        } | std::tuple(type<function_ref<void(Track)>>,
                       type<std::function<void(Track)>>);

        given("a function_ref that has a nontrivial return type") = []
        {
            function_ref<std::string()> fr = f_str;

            then("its erased functions can return different types") = [&]
            {
                expect(fr() == some_str);

                auto fn = [i = 0] { return some_str; };
                fr = fn;

                expect(fr() == some_str);
            };
        };
    };
};
