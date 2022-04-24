#include "common_callables.h"

namespace
{

static C c;
constinit function_ref<int()> cb{c};

static A a;
constinit function_ref<int()> cb_memref{nontype<&A::k>, a};
constinit function_ref<int()> cb_memptr{nontype<&A::k>, &a};

suite constant_initialized = []
{
    using namespace bdd;

    feature("constant-initialized global callback") = []
    {
        given("a global callback") = []
        {
            then("you can use it without capturing") = []
            { expect(cb() == non_const); };

            when("rebinding it to a different function") = []
            {
                cb = f;

                then("its functionality is replaced") = []
                { expect(cb() == free_function); };
            };

            when("rebinding it to a different global callback") = []
            {
                cb = cb_memptr;

                then("its functionality is replaced") = []
                { expect(cb() == cb_memref()); };
            };
        };
    };
};

} // namespace
