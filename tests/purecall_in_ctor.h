#pragma once

#include "common_callables.h"

struct courier;

struct abstract_base
{
    explicit abstract_base(courier &c);
    virtual int f() = 0;
};

using callback_type = function_ref<int()>;

struct courier
{
    callback_type cb = +[] { return std::abort(), -1; };
};

struct derived : abstract_base
{
    using abstract_base::abstract_base;
    int f() override;

    int data = 42;
};