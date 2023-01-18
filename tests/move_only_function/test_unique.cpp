#include "common_callables.h"

#include <cstring>
#include <iterator>
#include <memory>
#include <string_view>

class inlined_fixed_string
{
  public:
    inlined_fixed_string() = delete;

    auto size() const { return size_; }
    auto data() const
    {
        return reinterpret_cast<const char *>(std::next(this));
    }

    auto slice(size_t beg, size_t ed) const &
    {
        return std::string_view(data(), size()).substr(beg, ed - beg);
    }

    static auto make(std::string_view s)
    {
        size_t full_size = sizeof(inlined_fixed_string) + s.size();
        return new (::operator new(full_size))
            inlined_fixed_string(s.data(), s.size());
    }

    static auto make_unique(std::string_view s)
    {
        return std::unique_ptr<inlined_fixed_string>(make(s));
    }

  private:
    inlined_fixed_string(char const *p, size_t sz) : size_(sz)
    {
        std::memcpy(std::next(this), p, sz);
    }

    ~inlined_fixed_string() = default;

    void operator delete(inlined_fixed_string *s, std::destroying_delete_t)
    {
        size_t full_size = sizeof(inlined_fixed_string) + s->size();
        s->~inlined_fixed_string();
        ::operator delete(s, full_size);
    }

    friend std::default_delete<inlined_fixed_string>;

    size_t size_;
};

static_assert(not std::is_default_constructible_v<inlined_fixed_string>);
static_assert(not std::is_move_constructible_v<inlined_fixed_string>);
static_assert(not std::is_destructible_v<inlined_fixed_string>);

using T = move_only_function<std::string_view(size_t, size_t) const>;

suite unique_callable = []
{
    using namespace bdd;
    using namespace std::literals;

    feature("type-erase a boxed bound instance method") = []
    {
        given("a move_only_function storing a unique_ptr") = []
        {
            T fn(nontype<&inlined_fixed_string::slice>,
                 inlined_fixed_string::make_unique("coffee engineering"sv));

            when("calling the wrapper") = [&]
            {
                then("the object works as an lvalue") = [&]
                { expect(fn(0, 6) == "coffee"sv); };
            };

            when("moving the wrapper") = [fn2 = std::move(fn)]
            {
                then("the object behaves as if it is boxed") = [&]
                { expect(fn2(7, 10) == "eng"sv); };
            };
        };

        given("a move_only_function in-place constructed a unique_ptr") = []
        {
            T fn(nontype<&inlined_fixed_string::slice>,
                 std::in_place_type<std::unique_ptr<inlined_fixed_string>>,
                 inlined_fixed_string::make("destroying delete"sv));

            when("moving the wrapper") = [&]
            {
                T fn2 = std::move(fn);

                then("the object behaves as if it is boxed") = [&]
                { expect(fn2(11, 17) == "delete"sv); };
            };
        };
    };
};

static_assert(
    std::is_constructible_v<T, nontype_t<&inlined_fixed_string::slice>,
                            std::unique_ptr<inlined_fixed_string>>);
static_assert(
    not std::is_constructible_v<T, nontype_t<&inlined_fixed_string::slice>,
                                std::unique_ptr<inlined_fixed_string> &>,
    "users cannot construct the same wrapper from an lvalue");

static_assert(std::is_constructible_v<
              T, nontype_t<&inlined_fixed_string::slice>,
              std::in_place_type_t<std::unique_ptr<inlined_fixed_string>>,
              inlined_fixed_string *>);
static_assert(not std::is_constructible_v<
                  T, nontype_t<&inlined_fixed_string::slice>,
                  std::in_place_type_t<std::unique_ptr<inlined_fixed_string>>,
                  std::unique_ptr<inlined_fixed_string> &>,
              "move-only type cannot be in-place constructed from lvalue");
