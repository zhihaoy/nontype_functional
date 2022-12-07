# nontype\<functional\>

[![GitHub tag](https://img.shields.io/github/v/tag/zhihaoy/nontype_functional?sort=semver)](https://github.com/zhihaoy/nontype_functional/tags)
[![GitHub license](https://img.shields.io/github/license/zhihaoy/nontype_functional)](https://github.com/zhihaoy/nontype_functional/blob/main/LICENSE)
[![CMake](https://github.com/zhihaoy/nontype_functional/actions/workflows/cmake.yml/badge.svg)](https://github.com/zhihaoy/nontype_functional/actions/workflows/cmake.yml)


Provide complete implementation of `std::function`, `std::function_ref`, and `std::move_only_function` equivalent to those in the C++23 `<functional>` header.

## Highlights

- Macro-free implementation
- The size of each specialization is two pointers
- Not require RTTI
- Support classes without `operator()`

The implementation does not guarantee the best performance under all use cases but provides adequate code size & quality while maintaining full conformance.[^1]


## Supported toolchains

| Toolset              | Standard Library | Test Environment   |
| -------------------- | ---------------- | ------------------ |
| GCC >= 11.1.0        | libstdc++        | Ubuntu 20.04       |
| MSVC >= 14.30        | Microsoft STL    | Visual Studio 2022 |


## Installation

It's a header-only library. You may also install and consume its CMake targets:

```cmake
find_package(nontype_functional CONFIG REQUIRED)
target_link_libraries("main" PRIVATE std23::nontype_functional)
```


## Getting started

```cpp
#include <std23/function_ref.h>

using std23::function_ref;

void parse_ini(function_ref<size_t(char *, size_t)> read_cb);

...

#include <stdio.h>

int main()
{
    auto fp = ::fopen("my.ini", "r");
    parse_ini([fp](auto ptr, auto n)
              { return ::fread(ptr, 1, n, fp); });
    ::fclose(fp);
}
```

Ignore the fact that the code has no error handling or resource-safety; the callable wrappers, `function_ref` in the example, generalized the idea of *callbacks*. You can pass anything with a matching call pattern to another function, `parse_ini` in here, without turning the latter into a template.

Now, what if you have an existing class that can read data, but it's not a function object?

```cpp
class data_source
{
    ...

  public:
    auto read(char *, size_t) -> size_t;
};
```

Then you may designate a named member function, `read` in this example, to serve the role of an `operator()`:

```cpp
using std23::nontype;

int main()
{
    data_source input;
    parse_ini({nontype<&data_source::read>, input});
}
```

The `nontype` tag generalized the idea of *delegates* from other languages, like C&sharp;. What replaces `operator()` doesn't have to be a member function. You can also use a free function or even a lambda:

```cpp
int main()
{
    auto fp = ::fopen("my.ini", "r");
    parse_ini({nontype<[](FILE *fh, auto ptr, auto n)
                       { return ::fread(ptr, 1, n, fh); }>,
               fp});
    ::fclose(fp);
}
```

Feels like creating a member function for `FILE` on the fly, isn't it?


## Roadmap

- [x] 0.8 – `std::function_ref` & `std::function`
- [x] 0.9 – `std::move_only_function`
- [x] 1.0 – `nontype_t` constructors for `move_only_function`
- [ ] 1.1 – `copyable_function` from P2548
- [ ] 1.2 – Support C++20 modules


## See also

cppreference page for [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)
<br/>
cppreference page for [`std::move_only_function`](https://en.cppreference.com/w/cpp/utility/functional/move_only_function)
<br/>
[`std::function_ref`](https://wg21.link/p0792r12#Wording) specification


[^1]: Except for `std::function`'s `target()` member function, which is unimplemented because it requires RTTI.