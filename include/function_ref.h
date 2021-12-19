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

template<class Sig> struct _qual_fn_sig;

template<class R, class... Args> struct _qual_fn_sig<R(Args...)>
{
    using function = R(Args...);
    static constexpr bool is_noexcept = false;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::is_invocable_r_v<R, T..., Args...>;

    template<class T> using cv = T;
};

template<class R, class... Args> struct _qual_fn_sig<R(Args...) noexcept>
{
    using function = R(Args...);
    static constexpr bool is_noexcept = true;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::is_nothrow_invocable_r_v<R, T..., Args...>;

    template<class T> using cv = T;
};

template<class R, class... Args>
struct _qual_fn_sig<R(Args...) const> : _qual_fn_sig<R(Args...)>
{
    template<class T> using cv = T const;
};

template<class R, class... Args>
struct _qual_fn_sig<R(Args...) const noexcept>
    : _qual_fn_sig<R(Args...) noexcept>
{
    template<class T> using cv = T const;
};

template<class T, class Self>
inline constexpr bool _is_not_self =
    not std::is_same_v<std::remove_cvref_t<T>, Self>;

template<class T> struct _unwrap_reference
{
    using type = T;
};

template<class U> struct _unwrap_reference<std::reference_wrapper<U>>
{
    using type = U;
};

template<class T>
using _remove_and_unwrap_reference_t =
    _unwrap_reference<std::remove_reference_t<T>>::type;

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

template<class Sig, class = typename _qual_fn_sig<Sig>::function>
struct function_ref;

template<class Sig, class R, class... Args>
struct function_ref<Sig, R(Args...)> : _function_ref_base
{
    storage obj_;
    typedef R fwd_t(storage, Args...);
    fwd_t *fptr_ = nullptr;

    using signature = _qual_fn_sig<Sig>;
    template<class T> using cv = signature::template cv<T>;

    template<class... T>
    static constexpr bool is_invocable_using =
        signature::template is_invocable_using<T...>;

    template<class F, class... T>
    static constexpr bool is_memfn_invocable_using =
        is_invocable_using<F, T...> and std::is_member_pointer_v<F>;

    template<class T>
    static constexpr bool is_lvalue_invocable_using = is_invocable_using<T &>;

  public:
    function_ref() = default;

    template<class F>
    function_ref(F *f) noexcept requires std::is_function_v<F> and
        is_invocable_using<F>
        : obj_(f), fptr_([](storage fn_, Args... args) {
            return std23::invoke_r<R>(get<F>(fn_), std::forward<Args>(args)...);
        })
    {
    }

    template<class F, class T = _remove_and_unwrap_reference_t<F>>
    function_ref(F &&f) noexcept requires _is_not_self<F, function_ref> and
        is_lvalue_invocable_using<cv<T>>
        : obj_(std::addressof(static_cast<T &>(f))),
          fptr_([](storage fn_, Args... args) {
              return std23::invoke_r<R, cv<T> &>(*get<T>(fn_),
                                                 std::forward<Args>(args)...);
          })
    {
    }

    template<auto F>
    constexpr function_ref(nontype_t<F>) noexcept requires
        is_invocable_using<decltype(F)> : fptr_([](storage, Args... args) {
            return std23::invoke_r<R>(F, std::forward<Args>(args)...);
        })
    {
    }

    template<auto F, class T>
    function_ref(nontype_t<F>, cv<T> &obj) noexcept requires
        is_invocable_using<decltype(F), decltype(obj)>
        : obj_(std::addressof(obj)), fptr_([](storage this_, Args... args) {
            return std23::invoke_r<R>(F, *(get<cv<T>>(this_)),
                                      std::forward<Args>(args)...);
        })
    {
    }

    template<auto F, class T>
    function_ref(nontype_t<F>, cv<T> *obj) noexcept requires
        is_memfn_invocable_using<decltype(F), decltype(obj)>
        : obj_(obj), fptr_([](storage this_, Args... args) {
            return std23::invoke_r<R>(F, get<cv<T>>(this_),
                                      std::forward<Args>(args)...);
        })
    {
    }

    template<auto F, class T>
    function_ref(nontype_t<F> f, std::reference_wrapper<T> obj) noexcept
        requires is_memfn_invocable_using<decltype(F), decltype(obj)>
        : function_ref(f, obj.get())
    {
    }

    constexpr R operator()(Args... args) const noexcept(signature::is_noexcept)
    {
        return fptr_(obj_, std::forward<Args>(args)...);
    }
};

template<class T> struct _adapt_signature;

template<class F>
requires std::is_function_v<F>
struct _adapt_signature<F *>
{
    using type = F;
};

template<class Fp> using _adapt_signature_t = _adapt_signature<Fp>::type;

template<class T> struct _drop_first_arg_to_invoke;

template<class R, class T, class... Args>
struct _drop_first_arg_to_invoke<R (*)(T, Args...)>
{
    using type = R(Args...);
};

template<class R, class T, class... Args>
struct _drop_first_arg_to_invoke<R (*)(T, Args...) noexcept>
{
    using type = R(Args...);
};

template<class T, class Cls>
requires std::is_object_v<T>
struct _drop_first_arg_to_invoke<T Cls::*>
{
    using type = T();
};

template<class T, class Cls>
requires std::is_function_v<T>
struct _drop_first_arg_to_invoke<T Cls::*>
{
    using type = T;
};

template<class Fp>
using _drop_first_arg_to_invoke_t = _drop_first_arg_to_invoke<Fp>::type;

// clang-format off

template<class F>
requires std::is_function_v<F>
function_ref(F *) -> function_ref<F>;

// clang-format on

template<auto V>
function_ref(nontype_t<V>) -> function_ref<_adapt_signature_t<decltype(V)>>;

template<auto V>
function_ref(nontype_t<V>, auto)
    -> function_ref<_drop_first_arg_to_invoke_t<decltype(V)>>;

} // namespace std23
