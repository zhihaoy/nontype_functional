#include <std23/constant_wrapper.hpp>
#include <std23/function_ref.h>

using std23::cw;
using std23::function_ref;

void parse_ini(function_ref<size_t(char *, size_t)> read_cb);

#include <stdio.h>

class data_source
{
    /* ... */

  public:
    auto read(char *, size_t) -> size_t;
};

int main()
{
    data_source input;
    parse_ini({cw<&data_source::read>, input});
}
