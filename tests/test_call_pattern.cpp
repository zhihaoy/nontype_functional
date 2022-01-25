#include "common_callables.h"

struct Base
{
    int &n;
};

class Track : Base
{
  public:
    Track(int &target) : Base{target}
    {
    }

    Track(Track const &other) : Base(other)
    {
        ++n;
    }
};

static_assert(not std::is_trivially_copy_constructible_v<Track>);
static_assert(not std::is_trivially_move_constructible_v<Track>);

static void make_call(Track &&)
{
}

void test_call_pattern()
{
    {
        int n = 0;
        function_ref<void(Track)> fr = make_call;
        fr(Track{n});
        printf("function_ref: rvalue copied %d time(s)\n", n);
    }

    {
        int n = 0;
        function_ref<void(Track)> fr = make_call;
        Track t{n};
        fr(t);
        printf("function_ref: lvalue copied %d time(s)\n", n);
    }

    {
        int n = 0;
        std::function<void(Track)> fr = make_call;
        fr(Track{n});
        printf("std::function: rvalue copied %d time(s)\n", n);
    }

    {
        int n = 0;
        std::function<void(Track)> fr = make_call;
        Track t{n};
        fr(t);
        printf("std::function: lvalue copied %d time(s)\n", n);
    }
}
