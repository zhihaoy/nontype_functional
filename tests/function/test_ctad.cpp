#include "common_callables.h"

template<class T>
inline constexpr bool deduction_enabled = type_traits::is_valid<T>(
    [](auto x) -> decltype(void(function(std::move(x)))) {});

void test_ctad()
{
    {
        function fn = [i = 0] { return 0; };
        static_assert(std::is_same_v<decltype(fn), function<int()>>);
    }

    {
        function fn = [i = 0]() mutable { return 0; };
        static_assert(std::is_same_v<decltype(fn), function<int()>>);
    }

    {
        auto closure = [i = 0]() noexcept { return 0; };
        static_assert(not deduction_enabled<decltype(closure)>);
    }

    struct lvalue_only
    {
        void operator()(double &) & {}
    };

    struct rvalue_only
    {
        void operator()(double &) && {}
    };

    {
        lvalue_only lv;
        function fn = lv;
        static_assert(std::is_same_v<decltype(fn), function<void(double &)>>);
    }

    static_assert(deduction_enabled<lvalue_only>);
    static_assert(not deduction_enabled<rvalue_only>,
                  "function requires F to be Lvalue-Callable");

    static_assert(deduction_enabled<decltype(f)>);
    static_assert(not deduction_enabled<decltype(f_good)>,
                  "function has no noexcept signature");
}