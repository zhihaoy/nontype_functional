#include "common_callables.h"

#include <cmath>
#include <cstdio>
#include <string_view>

suite va_args = []
{
    using namespace bdd;
    using namespace std::string_view_literals;

    feature("ellipsis signature") = []
    {
        "free function with va_list parameter"_test = []
        {
            function<void(char *, size_t, char const *, ...)> snpf = vsnprintf;

            char buf[100];

            snpf(buf, sizeof(buf), "no replacement");
            expect(buf == "no replacement"sv);

            snpf(buf, sizeof(buf), "The answer = %d", 42);
            expect(buf == "The answer = 42"sv);

            snpf(buf, sizeof(buf), "nice %.*s", 4, "boating");
            expect(buf == "nice boat"sv);
        };

        "closure with va_list parameter"_test = []
        {
            function<double(int, ...)> stddev = [](int count, va_list args)
            {
                double sum = 0;
                double sum_sq = 0;

                for (int i = 0; i < count; ++i)
                {
                    double num = va_arg(args, double);
                    sum += num;
                    sum_sq += num * num;
                }

                return std::sqrt(sum_sq / count -
                                 (sum / count) * (sum / count));
            };

            expect(stddev(4, 25.0, 27.3, 26.9, 25.7) == 0.920258_d);
        };
    };
};