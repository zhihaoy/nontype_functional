#include "common_callables.h"

void test_nontype()
{
    A a;
    A const b;
    foo(nontype<f>);
    foo(nontype<g<int>>);
    foo({nontype<&A::g>, a});
    foo({nontype<&A::g>, &a});
    foo({nontype<&A::k>, b});
    foo({nontype<&A::k>, &b});
    foo({nontype<&A::data>, a});
    foo({nontype<&A::data>, std::ref(a)});
    foo({nontype<&A::data>, std::cref(a)});
    foo({nontype<h>, a});
}
