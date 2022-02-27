#pragma once

#include "std23/function.h"

#include <boost/ut.hpp>

using namespace boost::ut;

using std23::function;
using std23::nontype;
using std23::nontype_t;

#ifdef _MSC_VER
#define BODYN(n) ((::boost::ut::log << __FUNCSIG__ << '\n'), n)
#else
#define BODYN(n) ((::boost::ut::log << __PRETTY_FUNCTION__ << '\n'), n)
#endif

int f();
int f_good() noexcept;
