#pragma once

#ifdef NONTYPE_FUNCTIONAL_MODULE
extern "C++" {
#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>
}

import std;
import nontype_functional;
#else
#include "std23/function.h"
#include "std23/function_ref.h"
#include "std23/move_only_function.h"
#include <boost/ut.hpp>
#endif

#ifdef _MSC_VER
#define BODYN(n) ((::boost::ut::log << __FUNCSIG__ << '\n'), n)
#else
#define BODYN(n) ((::boost::ut::log << __PRETTY_FUNCTION__ << '\n'), n)
#endif

