#pragma once

#include "function.h"

#include <boost/ut.hpp>

using namespace boost::ut;

using std23::function;

#ifdef _MSC_VER
#define BODYN(n) ((::boost::ut::log << __FUNCSIG__ << '\n'), n)
#else
#define BODYN(n) ((::boost::ut::log << __PRETTY_FUNCTION__ << '\n'), n)
#endif

int f();
int f_good() noexcept;
