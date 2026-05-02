module;

import std;

#include <cassert>

#include <std23/function.h>
#include <std23/function_ref.h>
#include <std23/move_only_function.h>

export module nontype_functional;

export namespace std23
{
    using std23::nontype;
    using std23::nontype_t;
    using std23::function;
    using std23::function_ref;
    using std23::move_only_function;
}
