#pragma once

#include <functional>
#include <utility>

namespace std23
{

template<auto V> struct nontype_t
{
    explicit nontype_t() = default;
};

template<auto V> inline constexpr nontype_t<V> nontype{};

template<class R, class F, class... Args>
requires std::is_invocable_r_v<R, F, Args...>
constexpr R invoke_r(F &&f, Args &&...args) noexcept(
    std::is_nothrow_invocable_r_v<R, F, Args...>)
{
    if constexpr (std::is_void_v<R>)
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    else
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

template<class Sig> struct _qual_fn_sig_common;

template<class R, class... Args> struct _qual_fn_sig_common<R(Args...)>
{
    template<class T> using prepend = R(T, Args...);
    static constexpr bool is_noexcept = false;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::is_invocable_r_v<R, T..., Args...>;
};

template<class R, class... Args> struct _qual_fn_sig_common<R(Args...) noexcept>
{
    template<class T> using prepend = R(T, Args...);
    static constexpr bool is_noexcept = true;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::is_nothrow_invocable_r_v<R, T..., Args...>;
};

template<class T>
inline constexpr bool _is_function_pointer =
    std::is_pointer_v<T> and std::is_function_v<std::remove_pointer_t<T>>;

template<class T, class Self>
inline constexpr bool _is_not_self =
    not std::is_same_v<std::remove_cv_t<T>, Self>;

template<class Sig> struct _qual_fn_sig : _qual_fn_sig_common<Sig>
{
    template<class F, class... T>
    static constexpr bool is_memfn_invocable_using =
        _qual_fn_sig_common<Sig>::template is_invocable_using<F, T...>
            and std::is_member_pointer_v<F>;

    template<class T>
    static constexpr bool is_lvalue_invocable_using =
        _qual_fn_sig_common<Sig>::template is_invocable_using<T &> and
        not _is_function_pointer<T> and not std::is_member_pointer_v<T>;
};

struct _function_ref_base
{
    union storage {
        void *p_ = nullptr;
        void const *cp_;
        void (*fp_)();

        constexpr storage() noexcept = default;

        template<class T>
        requires std::is_object_v<T>
        constexpr explicit storage(T *p) noexcept : p_(p)
        {
        }

        template<class T>
        requires std::is_object_v<T>
        constexpr explicit storage(T const *p) noexcept : cp_(p)
        {
        }

        template<class T>
        requires std::is_function_v<T>
        constexpr explicit storage(T *p) noexcept
            : fp_(reinterpret_cast<decltype(fp_)>(p))
        {
        }
    };

    template<class T> constexpr static auto get(storage obj)
    {
        if constexpr (std::is_const_v<T>)
            return static_cast<T *>(obj.cp_);
        else if constexpr (std::is_object_v<T>)
            return static_cast<T *>(obj.p_);
        else
            return reinterpret_cast<T *>(obj.fp_);
    }
};

template<class Sig> class function_ref;

#define _FUNCTION_REF_SPECIALIZATION(Sig, T_cv)                                \
    template<class R, class... Args>                                           \
    class function_ref<Sig> : _function_ref_base                               \
    {                                                                          \
        using signature = _qual_fn_sig<Sig>;                                   \
        storage obj_;                                                          \
        typename signature::template prepend<storage> *fptr_ = nullptr;        \
                                                                               \
      public:                                                                  \
        function_ref() = default;                                              \
        template<class F, class T = std::remove_reference_t<F>>                \
        function_ref(F &&f) noexcept                                           \
            requires(_is_not_self<T, function_ref> and                         \
                     signature::template is_lvalue_invocable_using<T_cv &>)    \
            : obj_(std::addressof(f)), fptr_([](storage fn_, Args... args) {   \
                  return std23::invoke_r<R, T_cv &>(                           \
                      *get<T>(fn_), std::forward<Args>(args)...);              \
              })                                                               \
        {                                                                      \
        }                                                                      \
                                                                               \
        template<auto F>                                                       \
        constexpr function_ref(nontype_t<F>) noexcept requires                 \
            signature::template is_invocable_using<decltype(F)>                \
            : fptr_([](storage, Args... args) {                                \
                return std23::invoke_r<R>(F, std::forward<Args>(args)...);     \
            })                                                                 \
        {                                                                      \
        }                                                                      \
                                                                               \
        template<auto F, class T>                                              \
        function_ref(nontype_t<F>, T_cv &obj) noexcept requires                \
            signature::template is_invocable_using<decltype(F), decltype(obj)> \
            : obj_(std::addressof(obj)),                                       \
              fptr_([](storage this_, Args... args) {                          \
                  return std23::invoke_r<R>(F, *(get<T_cv>(this_)),            \
                                            std::forward<Args>(args)...);      \
              })                                                               \
        {                                                                      \
        }                                                                      \
                                                                               \
        template<auto F, class T>                                              \
        function_ref(nontype_t<F>, T_cv *obj) noexcept requires                \
            signature::template is_memfn_invocable_using<decltype(F),          \
                                                         decltype(obj)>        \
            : obj_(obj), fptr_([](storage this_, Args... args) {               \
                return std23::invoke_r<R>(F, get<T_cv>(this_),                 \
                                          std::forward<Args>(args)...);        \
            })                                                                 \
        {                                                                      \
        }                                                                      \
                                                                               \
        template<auto F, class T>                                              \
        function_ref(nontype_t<F> f,                                           \
                     std::reference_wrapper<T> obj) noexcept requires          \
            signature::template is_memfn_invocable_using<decltype(F),          \
                                                         decltype(obj)>        \
            : function_ref(f, obj.get())                                       \
        {                                                                      \
        }                                                                      \
                                                                               \
        constexpr R operator()(Args... args) const                             \
            noexcept(signature::is_noexcept)                                   \
        {                                                                      \
            return fptr_(obj_, std::forward<Args>(args)...);                   \
        }                                                                      \
    };

_FUNCTION_REF_SPECIALIZATION(R(Args...), T);
_FUNCTION_REF_SPECIALIZATION(R(Args...) noexcept, T);
_FUNCTION_REF_SPECIALIZATION(R(Args...) const, T const);
_FUNCTION_REF_SPECIALIZATION(R(Args...) const noexcept, T const);

#undef _FUNCTION_REF_SPECIALIZATION

} // namespace std23
