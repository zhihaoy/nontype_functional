#include <std23/function_ref.h>

using std23::function_ref;

void parse_ini(function_ref<size_t(char *, size_t)> read_cb);

#include <stdio.h>

int main()
{
    auto fp = ::fopen("my.ini", "r");
    parse_ini([fp](auto ptr, auto n)
              { return ::fread(ptr, 1, n, fp); });
    ::fclose(fp);
}
