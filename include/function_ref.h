#pragma once

#include <functional>
#include <utility>

namespace std23
{

template <auto V> struct in_place_value_t
{
    explicit in_place_value_t() = default;
};

template <auto V> inline constexpr in_place_value_t<V> in_place_value{};

template <class R, class F, class... Args>
requires std::is_invocable_r_v<R, F, Args...>
constexpr R std_invoke_r(F &&f, Args &&...args) noexcept(
    std::is_nothrow_invocable_r_v<R, F, Args...>)
{
    if constexpr (std::is_void_v<R>)
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    else
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

template <class Sig> class function_ref;

template <class R, class... Args> class function_ref<R(Args...)>
{
    union storage {
        void *p_ = nullptr;
        void const *cp_;
        void (*fp_)();

        constexpr storage() noexcept = default;

        template <class T>
        requires std::is_object_v<T>
        constexpr explicit storage(T *p) noexcept : p_(p)
        {
        }

        template <class T>
        requires std::is_object_v<T>
        constexpr explicit storage(T const *p) noexcept : cp_(p)
        {
        }

        template <class T>
        requires std::is_function_v<T>
        constexpr explicit storage(T *p) noexcept
            : fp_(reinterpret_cast<decltype(fp_)>(p))
        {
        }

    } obj_;

    template <class T> constexpr static auto get(storage obj)
    {
        if constexpr (std::is_const_v<T>)
            return static_cast<T *>(obj.cp_);
        else if constexpr (std::is_object_v<T>)
            return static_cast<T *>(obj.p_);
        else
            return reinterpret_cast<T *>(obj.fp_);
    }

    typedef R fn_t(storage, Args...);
    fn_t *fptr_ = nullptr;

  public:
    function_ref() = default;

    template <class F, class T = std::remove_reference_t<F>>
    function_ref(F &&f) noexcept
        requires(!std::is_same_v<std::remove_cv_t<T>, function_ref> and
                 std::is_invocable_r_v<R, F, Args...> and
                 !(std::is_pointer_v<T> and
                       std::is_function_v<std::remove_pointer_t<T>> or
                   std::is_member_pointer_v<T>))
        : obj_(std::addressof(f)), fptr_([](storage fn_, Args... args) {
              return std_invoke_r<R>(*get<T>(fn_), std::forward<Args>(args)...);
          })
    {
    }

    template <auto F>
    constexpr function_ref(in_place_value_t<F>) noexcept requires
        std::is_invocable_r_v<R, decltype(F), Args...>
        : fptr_([](storage, Args... args) {
            return std_invoke_r<R>(F, std::forward<Args>(args)...);
        })
    {
    }

    template <auto F, class T>
    function_ref(in_place_value_t<F>, T &obj) noexcept requires
        std::is_invocable_r_v<R, decltype(F), decltype(obj), Args...>
        : obj_(std::addressof(obj)), fptr_([](storage this_, Args... args) {
            return std_invoke_r<R>(F, *(get<T>(this_)),
                                   std::forward<Args>(args)...);
        })
    {
    }

    template <auto F, class T>
    function_ref(in_place_value_t<F>, T *obj) noexcept requires
        std::is_invocable_r_v<R, decltype(F), decltype(obj), Args...> and
        std::is_member_pointer_v<decltype(F)>
        : obj_(obj), fptr_([](storage this_, Args... args) {
            return std_invoke_r<R>(F, get<T>(this_),
                                   std::forward<Args>(args)...);
        })
    {
    }

    template <auto F, class T>
    function_ref(in_place_value_t<F> f,
                 std::reference_wrapper<T> obj) noexcept requires
        std::is_invocable_r_v<R, decltype(F), decltype(obj), Args...> and
        std::is_member_pointer_v<decltype(F)> : function_ref(f, obj.get())
    {
    }

    constexpr R operator()(Args... args) const
    {
        return fptr_(obj_, std::forward<Args>(args)...);
    }
};

} // namespace std23
