#include "common_callables.h"

int f_good() noexcept;
int h_good(A) noexcept;

void test_ctad()
{
    {
        auto fr = function_ref(f);
        static_assert(std::is_same_v<decltype(fr), function_ref<int()>>);
    }

    {
        auto fr = function_ref(f_good);
        static_assert(
            std::is_same_v<decltype(fr), function_ref<int() noexcept>>);
    }

    {
        auto fr = function_ref(h);
        static_assert(std::is_same_v<decltype(fr), function_ref<int(A)>>);
    }

    {
        auto fr = function_ref(h_good);
        static_assert(
            std::is_same_v<decltype(fr), function_ref<int(A) noexcept>>);
    }

    {
        auto fr = function_ref(nontype<f>);
        static_assert(std::is_same_v<decltype(fr), function_ref<int()>>);
    }

    {
        auto fr = function_ref(nontype<f_good>);
        static_assert(
            std::is_same_v<decltype(fr), function_ref<int() noexcept>>);
    }

    {
        auto fr = function_ref(nontype<h>);
        static_assert(std::is_same_v<decltype(fr), function_ref<int(A)>>);
    }

    {
        auto fr = function_ref(nontype<h_good>);
        static_assert(
            std::is_same_v<decltype(fr), function_ref<int(A) noexcept>>);
    }

    {
        A a;
        auto fr = function_ref(nontype<&A::g>, &a);
        static_assert(std::is_same_v<decltype(fr), function_ref<int()>>);
    }

    {
        A a;
        auto fr = function_ref(nontype<&A::k>, &a);
        static_assert(std::is_same_v<decltype(fr), function_ref<int()>>,
                      "bound member function's qualifier is not deduced");
    }

    {
        A a;
        auto fr = function_ref(nontype<&A::data>, &a);
        static_assert(std::is_same_v<decltype(fr), function_ref<int &()>>);

        auto fc = function_ref(nontype<&A::data>, std::as_const(a));
        static_assert(std::is_same_v<decltype(fc), function_ref<int const &()>>,
                      "type to retrieve data member depends on object");
    }

    {
        A a;
        auto fr = function_ref(nontype<&h>, a);
        static_assert(std::is_same_v<decltype(fr), function_ref<int()>>);
    }

    {
        A a;
        auto fr = function_ref(nontype<&h_good>, a);
        static_assert(
            std::is_same_v<decltype(fr), function_ref<int() noexcept>>);
    }
}