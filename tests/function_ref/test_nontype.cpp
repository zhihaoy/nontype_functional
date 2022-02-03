#include "common_callables.h"

void test_nontype()
{
    A a;
    A const b;
    foo(nontype<f>);
    foo(nontype<g<int>>);
    foo({nontype<&A::g>, a});
    foo({nontype<&A::g>, &a});
    foo({nontype<&A::k>, a});
    foo({nontype<&A::k>, &a});
    foo({nontype<&A::k>, b});
    foo({nontype<&A::k>, &b});
    foo({nontype<&A::data>, a});
    std::reference_wrapper r = a;
    foo({nontype<&A::data>, r});
    foo({nontype<h>, a});
    foo({nontype<h>, r});
}
