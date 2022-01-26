#include "common_callables.h"

#include <cstdlib>
#include <string>

constexpr char some_str[] = "if you see this, then we're fine";

static void return_type_differs(function_ref<std::string()> fr)
{
    auto s = fr();
    if (s != some_str)
        std::abort();
}

static void calling_convension_differs(function_ref<void(std::string)> fr)
{
    fr(some_str);
}

struct pure_if
{
    virtual int f() noexcept = 0;
    virtual int g()
    {
        return -1;
    }
};

void test_pmf_cast()
{
    {
        struct TR
        {
            char const *f()
            {
                return some_str;
            }
        } a;
        return_type_differs({nontype<&TR::f>, &a});
    }

    {
        struct TP
        {
            void f(std::string s)
            {
                if (s != some_str)
                    std::abort();
            }
        } b;
        calling_convension_differs({nontype<&TP::f>, &b});
    }

    {
        struct base1
        {
            int g()
            {
                std::printf("got: %c; ", nope);
                return BODY();
            }

            char nope = 'a';
        };

        struct base2
        {
            int f() const
            {
                std::printf("got: %d; ", yep);
                return BODY();
            }

            int yep = 99;
        };

        struct multiple_inheritance : base1, base2
        {
        } m;
        foo({nontype<&multiple_inheritance::f>, &m});
    }

    {
        struct derived : pure_if
        {
            int f() noexcept override
            {
                std::printf("got: %c; ", nope);
                return BODY();
            }

            char nope = 'a';
        };

        struct most_derived : derived
        {
            int f() noexcept override
            {
                std::printf("got: %d; ", yep);
                return BODY();
            }

            int yep = 99;
        } md;

        foo({nontype<&derived::f>, &md});
        foo({nontype<&pure_if::f>, &md});
    }

    {
        struct base
        {
            float h = .3f;
        };

        struct base1 : virtual base
        {
            int g()
            {
                std::printf("got: c=%c; h=%g; ", c, h);
                return BODY();
            }

            char c = 'a';
        };

        struct base2 : virtual base
        {
            int f() const
            {
                std::printf("got: %d; h=%g; ", d, h);
                return BODY();
            }

            int d = 99;
        };

        struct virtual_inheritance : base1, base2
        {
        } vm;
        foo({nontype<&virtual_inheritance::f>, &vm});
        foo({nontype<&virtual_inheritance::g>, &vm});
    }

    {
        struct non_pure_if : pure_if
        {
            float h = .3f;
        };

        struct base1 : virtual non_pure_if
        {
            int f() noexcept override
            {
                std::printf("got: c=%c; h=%g; ", c, h);
                return BODY();
            }

            char c = 'a';
        };

        struct base2 : virtual non_pure_if
        {
            int f() noexcept override
            {
                std::printf("got: %d; h=%g; ", d, h);
                return BODY();
            }

            int d = 99;
        };

        struct virtual_x_virtual : base1, base2
        {
            int f() noexcept override
            {
                std::printf("got: c=%c; d=%d h=%g; ", c, d, h);
                return BODY();
            }
        } vv;

        foo({nontype<&base1::f>, &vv});
        foo({nontype<&base2::f>, &vv});
        foo({nontype<&non_pure_if::f>, &vv});
    }
}