#include "common_callables.h"

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

suite call_pattern = []
{
    using namespace bdd;

    "call_pattern"_test = []
    {
        given("a signature that takes parameter by value") =
            []<class Ty>(Ty)
        {
            using T = Ty::type;
            boost::ut::log << type<T>;

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
        } | std::tuple<std::type_identity<function_ref<void(Track)>>,
                       std::type_identity<std::function<void(Track)>>>{};
    };
};
