#include "common_callables.h"

template<class T>
inline constexpr bool deduction_enabled = type_traits::is_valid<T>(
    [](auto x) -> decltype(void(function(std::move(x)))) {});

void test_ctad()
{
    {
        function fn = [i = 0] { return 0; };
        static_assert(std::is_same_v<decltype(fn), function<int()>>);
        static_assert(std::is_same_v<decltype(fn)::result_type, int>);
    }

    {
        function fn = [i = 0](int &&, int const) mutable { return "lit"; };
        static_assert(
            std::is_same_v<decltype(fn), function<char const *(int &&, int)>>);
        static_assert(std::is_same_v<decltype(fn)::result_type, char const *>);
    }

    {
        function fn = f_good;
        static_assert(std::is_same_v<decltype(fn), function<int()>>,
                      "[conv.fctptr]/1");
        static_assert(std::is_same_v<decltype(fn)::result_type, int>);
    }

    {
        function fn = [i = 0]() noexcept { return 0; };
        static_assert(std::is_same_v<decltype(fn), function<int()>>,
                      "[func.wrap.func.con]/16");
        static_assert(std::is_same_v<decltype(fn)::result_type, int>);
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
        static_assert(std::is_same_v<decltype(fn)::result_type, void>);
    }

    static_assert(deduction_enabled<lvalue_only>);
    static_assert(not deduction_enabled<rvalue_only>,
                  "function requires F to be Lvalue-Callable");
}