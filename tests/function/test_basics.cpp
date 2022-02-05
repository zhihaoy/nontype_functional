#include "common_callables.h"

suite basics = []
{
    using namespace bdd;

    "basics"_test = []
    {
        function<int()> fn;
        expect(!fn);
    };
};