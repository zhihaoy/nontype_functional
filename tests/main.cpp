#include "common_callables.h"

void test_basics();
void test_nontype();
void test_noexcept();
void test_const();
void test_safety();
void test_const_noexcept();
void test_call_pattern();

int main()
{
    test_basics();
    test_nontype();
    test_noexcept();
    test_const();
    test_safety();
    test_const_noexcept();
    test_call_pattern();
}
