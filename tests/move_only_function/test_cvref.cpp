#include "common_callables.h"

#include <memory>

using T = std::unique_ptr<char>;

struct UnspecificValueCategory : T
{
    int operator()(T) { return empty; }
};

struct LvalueOnly : T
{
    int operator()(T) & { return lref; }
};

struct RvalueOnly : T
{
    int operator()(T) && { return rref; }
};

struct EitherValueCategory : LvalueOnly, RvalueOnly
{
    using LvalueOnly::operator();
    using RvalueOnly::operator();
};

template<class T> struct ImmutableCall;

template<>
struct ImmutableCall<UnspecificValueCategory> : UnspecificValueCategory
{
    using UnspecificValueCategory::operator();
    int operator()(T) const { return const_; }
};

template<> struct ImmutableCall<LvalueOnly> : LvalueOnly
{
    using LvalueOnly::operator();
    int operator()(T) const & { return const_lref; }
};

template<> struct ImmutableCall<RvalueOnly> : RvalueOnly
{
    using RvalueOnly::operator();
    int operator()(T) const && { return const_rref; }
};

template<>
struct ImmutableCall<EitherValueCategory> : ImmutableCall<LvalueOnly>,
                                            ImmutableCall<RvalueOnly>
{
    using ImmutableCall<LvalueOnly>::operator();
    using ImmutableCall<RvalueOnly>::operator();
};

suite cvref = []
{
    using namespace bdd;

    feature("empty cv-ref qualifier") =
        [call = [](move_only_function<int(T)> f) { return f(nullptr); }]
    {
        given("a callable object with unqual call operator") = [=]
        {
            expect(call(UnspecificValueCategory{}) == empty);

            then("reference_wrapper can call it without moving") = [=]
            {
                UnspecificValueCategory fn;
                expect(call(std::reference_wrapper(fn)) == empty);
            };
        };

        given("a callable object with value-category-aware call operators") =
            [=]
        {
            then("the object is called only as an lvalue") = [=]
            { expect(call(EitherValueCategory{}) == lref); };
        };

        static_assert(not std::is_invocable_v<decltype(call), LvalueOnly>,
                      "See also: https://cplusplus.github.io/LWG/issue3680");

        given("an immutable callable object") = [=]
        {
            then("the object is called only as a copy") = [=]
            {
                {
                    expect(call(ImmutableCall<UnspecificValueCategory>{}) ==
                           empty);
                };
            };

            then("reference_wrapper can call either const or non-const") = [=]
            {
                ImmutableCall<UnspecificValueCategory> fn;
                expect(call(std::ref(fn)) == empty);
                expect(call(std::cref(fn)) == const_);
            };
        };
    };

    feature("const-qualified") =
        [call = [](move_only_function<int(T) const> const f)
         { return f(nullptr); }]
    {
        given("a callable object with const call operator") = [=]
        {
            expect(call(ImmutableCall<UnspecificValueCategory>{}) == const_);

            then("reference_wrapper is unaffected by the qualifier") = [=]
            {
                ImmutableCall<UnspecificValueCategory> fn;
                expect(call(std::ref(fn)) == empty);
                expect(call(std::cref(fn)) == const_);
            };
        };

        given("a callable object with value-category-aware call operators") =
            [=]
        {
            then("the object is called only as an lvalue") = [=] {
                expect(call(ImmutableCall<EitherValueCategory>{}) ==
                       const_lref);
            };
        };

        static_assert(
            not std::is_invocable_v<decltype(call), UnspecificValueCategory>,
            "unqual signature is non-const-only");

        given("a callable object with unqual call operator") = [=]
        {
            then("reference_wrapper can make it const-invocable by lying") = [=]
            {
                UnspecificValueCategory fn;
                expect(call(std::reference_wrapper(fn)) == empty);
            };
        };
    };

    feature("&-qualified") =
        [call = [](move_only_function<int(T) &> f) { return f(nullptr); }]
    {
        given("a callable object with lvalue-ref call operator") = [=]
        { expect(call(LvalueOnly{}) == lref); };

        given("a callable object with unqual call operator") = [=]
        { expect(call(UnspecificValueCategory{}) == empty); };

        given("a callable object with value-category-aware call operators") =
            [=]
        {
            then("the object is called only as an lvalue") = [=]
            { expect(call(EitherValueCategory{}) == lref); };
        };

        static_assert(not std::is_invocable_v<decltype(call), RvalueOnly>,
                      "&&-qualified cannot be called as an lvalue");

        given("an immutable callable object") = [=]
        {
            then("the object is called only as a copy") = [=]
            {
                {
                    expect(call(ImmutableCall<UnspecificValueCategory>{}) ==
                           empty);
                };
            };
        };
    };

    feature("&&-qualified") = [call = [](move_only_function<int(T) &&> f)
                               { return std::move(f)(nullptr); }]
    {
        given("a callable object with rvalue-ref call operator") = [=]
        { expect(call(RvalueOnly{}) == rref); };

        given("a callable object with unqual call operator") = [=]
        { expect(call(UnspecificValueCategory{}) == empty); };

        given("a callable object with value-category-aware call operators") =
            [=]
        {
            then("the object is called only as an rvalue") = [=]
            { expect(call(EitherValueCategory{}) == rref); };

            then("reference_wrapper calls it only as an lvalue") = [=]
            {
                EitherValueCategory fn;
                expect(call(std::reference_wrapper(fn)) == lref);
            };
        };

        static_assert(not std::is_invocable_v<decltype(call), LvalueOnly>,
                      "&-qualified cannot be called as an rvalue");

        given("an immutable callable object") = [=]
        {
            then("the object is called only as a copy") = [=]
            {
                {
                    expect(call(ImmutableCall<UnspecificValueCategory>{}) ==
                           empty);
                };
            };
        };
    };

    feature("const &-qualified") =
        [call = [](move_only_function<int(T) const &> const f)
         { return f(nullptr); }]
    {
        given("a callable object with const lvalue-ref call operator") = [=]
        { expect(call(ImmutableCall<LvalueOnly>{}) == const_lref); };

        given("a callable object with const call operator") = [=]
        { expect(call(ImmutableCall<UnspecificValueCategory>{}) == const_); };

        given("a callable object with value-category-aware call operators") =
            [=]
        {
            then("the object is called only as an lvalue") = [=] {
                expect(call(ImmutableCall<EitherValueCategory>{}) ==
                       const_lref);
            };
        };

        static_assert(
            not std::is_invocable_v<decltype(call), ImmutableCall<RvalueOnly>>,
            "&&-qualified cannot be called as an lvalue");

        static_assert(
            not std::is_invocable_v<decltype(call), UnspecificValueCategory>,
            "unqual signature is non-const-only");
    };

    feature("const &&-qualified") =
        [call = [](move_only_function<int(T) const &&> const f)
         { return static_cast<decltype(f) &&>(f)(nullptr); }]
    {
        given("a callable object with const rvalue-ref call operator") = [=]
        { expect(call(ImmutableCall<RvalueOnly>{}) == const_rref); };

        given("a callable object with const call operator") = [=]
        { expect(call(ImmutableCall<UnspecificValueCategory>{}) == const_); };

        given("a callable object with value-category-aware call operators") =
            [=]
        {
            then("the object is called only as an rvalue") = [=] {
                expect(call(ImmutableCall<EitherValueCategory>{}) ==
                       const_rref);
            };

            then("reference_wrapper calls it only as an lvalue") = [=]
            {
                ImmutableCall<EitherValueCategory> fn;
                expect(call(std::ref(fn)) == lref);
                expect(call(std::cref(fn)) == const_lref);
            };
        };

        given("a callable object with const lvalue-ref call operators") = [=]
        {
            then("the object can be called as an rvalue") = [=]
            { expect(call(ImmutableCall<LvalueOnly>{}) == const_lref); };
        };

        static_assert(
            not std::is_invocable_v<decltype(call), UnspecificValueCategory>,
            "unqual signature is non-const-only");
    };
};

static_assert(std::is_invocable_v<move_only_function<int()>>);
static_assert(std::is_invocable_v<move_only_function<int() const>>);
static_assert(not std::is_invocable_v<move_only_function<int()> const>);
static_assert(std::is_invocable_v<move_only_function<int() const> const>);

static_assert(not std::is_invocable_v<move_only_function<int() &>>);
static_assert(std::is_invocable_v<move_only_function<int() const &>>,
              "const & can bind rvalue");
static_assert(not std::is_invocable_v<move_only_function<int() &> const>);
static_assert(std::is_invocable_v<move_only_function<int() const &> const>);

static_assert(not std::is_invocable_v<move_only_function<int() &&> &>);
static_assert(not std::is_invocable_v<move_only_function<int() const &&> &>);
static_assert(not std::is_invocable_v<move_only_function<int() &&> const &>);
static_assert(
    not std::is_invocable_v<move_only_function<int() const &&> const &>);
