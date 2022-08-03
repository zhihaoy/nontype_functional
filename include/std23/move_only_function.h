#ifndef INCLUDE_STD23_MOVE__ONLY__FUNCTION
#define INCLUDE_STD23_MOVE__ONLY__FUNCTION

#include "__functional_base.h"

#include <new>
#include <utility>

namespace std23
{

template<class Sig> struct _cv_fn_sig
{};

template<class R, class... Args> struct _cv_fn_sig<R(Args...)>
{
    using function = R(Args...);
    template<class T> using cv = T;
};

template<class R, class... Args> struct _cv_fn_sig<R(Args...) const>
{
    using function = R(Args...);
    template<class T> using cv = T const;
};

template<class Sig> struct _ref_quals_fn_sig : _cv_fn_sig<Sig>
{
    template<class T> using ref = T;
};

template<class R, class... Args>
struct _ref_quals_fn_sig<R(Args...) &> : _cv_fn_sig<R(Args...)>
{
    template<class T> using ref = T &;
};

template<class R, class... Args>
struct _ref_quals_fn_sig<R(Args...) const &> : _cv_fn_sig<R(Args...) const>
{
    template<class T> using ref = T &;
};

template<class R, class... Args>
struct _ref_quals_fn_sig<R(Args...) &&> : _cv_fn_sig<R(Args...)>
{
    template<class T> using ref = T &&;
};

template<class R, class... Args>
struct _ref_quals_fn_sig<R(Args...) const &&> : _cv_fn_sig<R(Args...) const>
{
    template<class T> using ref = T &&;
};

template<bool V> struct _noex_traits
{
    static constexpr bool is_noexcept = V;
};

template<class Sig>
struct _full_fn_sig : _ref_quals_fn_sig<Sig>, _noex_traits<false>
{};

template<class R, class... Args>
struct _full_fn_sig<R(Args...) noexcept> : _ref_quals_fn_sig<R(Args...)>,
                                           _noex_traits<true>
{};

template<class R, class... Args>
struct _full_fn_sig<R(Args...) & noexcept> : _ref_quals_fn_sig<R(Args...) &>,
                                             _noex_traits<true>
{};

template<class R, class... Args>
struct _full_fn_sig<R(Args...) && noexcept> : _ref_quals_fn_sig<R(Args...) &&>,
                                              _noex_traits<true>
{};

template<class R, class... Args>
struct _full_fn_sig<R(Args...) const noexcept>
    : _ref_quals_fn_sig<R(Args...) const>, _noex_traits<true>
{};

template<class R, class... Args>
struct _full_fn_sig<R(Args...) const & noexcept>
    : _ref_quals_fn_sig<R(Args...) const &>, _noex_traits<true>
{};

template<class R, class... Args>
struct _full_fn_sig<R(Args...) const && noexcept>
    : _ref_quals_fn_sig<R(Args...) const &&>, _noex_traits<true>
{};

constexpr auto _take_reference(auto &&rhs)
{
    return new auto(decltype(rhs)(rhs));
}

constexpr auto _take_reference(auto *rhs) noexcept
{
    return rhs;
}

template<class T>
constexpr auto _take_reference(std::reference_wrapper<T> rhs) noexcept
{
    return std::addressof(rhs.get());
}

struct _move_only_pointer
{
    union value_type
    {
        void *p_ = nullptr;
        void const *cp_;
        void (*fp_)();
    } val;

    _move_only_pointer() = default;
    _move_only_pointer(_move_only_pointer const &) = delete;
    _move_only_pointer &operator=(_move_only_pointer const &) = delete;

    constexpr _move_only_pointer(_move_only_pointer &&other) noexcept
        : val(std::exchange(other.val, {}))
    {}

    template<class T> requires std::is_object_v<T>
    constexpr explicit _move_only_pointer(T *p) noexcept : val{.p_ = p}
    {}

    template<class T> requires std::is_object_v<T>
    constexpr explicit _move_only_pointer(T const *p) noexcept : val{.cp_ = p}
    {}

    template<class T> requires std::is_function_v<T>
    constexpr explicit _move_only_pointer(T *p) noexcept
        : val{.fp_ = reinterpret_cast<decltype(val.fp_)>(p)}
    {}

    template<class T> requires std::is_object_v<T>
    constexpr _move_only_pointer &operator=(T *p) noexcept
    {
        val.p_ = p;
        return *this;
    }

    template<class T> requires std::is_object_v<T>
    constexpr _move_only_pointer &operator=(T const *p) noexcept
    {
        val.cp_ = p;
        return *this;
    }

    template<class T> requires std::is_function_v<T>
    constexpr _move_only_pointer &operator=(T *p) noexcept
    {
        val.fp_ = reinterpret_cast<decltype(val.fp_)>(p);
        return *this;
    }

    constexpr _move_only_pointer &operator=(_move_only_pointer &&other) noexcept
    {
        val = std::exchange(other.val, {});
        return *this;
    }
};

template<bool noex, class R, class... Args> struct _callable_trait
{
    using handle = _move_only_pointer::value_type;

    typedef auto call_t(handle, Args...) noexcept(noex) -> R;
    typedef void destroy_t(handle) noexcept;

    struct vtable
    {
        call_t *call = 0;
        destroy_t *destroy = [](handle) noexcept {};
    };

    static inline constinit vtable abstract_base;

    template<class T> constexpr static auto get(handle val)
    {
        if constexpr (std::is_const_v<T>)
            return static_cast<T *>(val.cp_);
        else if constexpr (std::is_object_v<T>)
            return static_cast<T *>(val.p_);
        else
            return reinterpret_cast<T *>(val.fp_);
    }

    // See also: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71954
    template<class T, template<class> class quals>
    static inline constinit vtable callable_target{
        .call =
            [](handle this_, Args... args) noexcept(noex)
        {
            if constexpr (std::is_lvalue_reference_v<T> or std::is_pointer_v<T>)
            {
                using Tp = std::remove_reference_t<std::remove_pointer_t<T>>;
                return std23::invoke_r<R>(*get<Tp>(this_),
                                          static_cast<Args>(args)...);
            }
            else
            {
                using Fp = quals<T>::type;
                return std23::invoke_r<R>(static_cast<Fp>(*get<T>(this_)),
                                          static_cast<Args>(args)...);
            }
        },
        .destroy =
            [](handle this_) noexcept
        {
            if constexpr (not std::is_lvalue_reference_v<T> and
                          not std::is_pointer_v<T>)
                delete get<T>(this_);
        },
    };
};

template<class T, template<class...> class Primary>
inline constexpr bool _is_specialization_of = false;

template<template<class...> class Primary, class... Args>
inline constexpr bool _is_specialization_of<Primary<Args...>, Primary> = true;

template<class T, template<class...> class Primary>
inline constexpr bool _does_not_specialize =
    not _is_specialization_of<std::remove_cvref_t<T>, Primary>;

template<class S, class = typename _full_fn_sig<S>::function>
class move_only_function;

template<class S, class R, class... Args>
class move_only_function<S, R(Args...)>
{
    using signature = _full_fn_sig<S>;

    template<class T> using cv = signature::template cv<T>;
    template<class T> using ref = signature::template ref<T>;

    static constexpr bool noex = signature::is_noexcept;
    static constexpr bool is_const = std::is_same_v<cv<void>, void const>;
    static constexpr bool is_lvalue_only = std::is_same_v<ref<int>, int &>;
    static constexpr bool is_rvalue_only = std::is_same_v<ref<int>, int &&>;

    template<class T> using cvref = ref<cv<T>>;
    template<class T>
    struct inv_quals_f
        : std::conditional<is_lvalue_only or is_rvalue_only, cvref<T>, cv<T> &>
    {};
    template<class T> using inv_quals = inv_quals_f<T>::type;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::conditional_t<noex, std::is_nothrow_invocable_r<R, T..., Args...>,
                           std::is_invocable_r<R, T..., Args...>>::value;

    template<class VT>
    static constexpr bool is_callable_from =
        is_invocable_using<cvref<VT>> and is_invocable_using<inv_quals<VT>>;

    template<class F, class FD = std::decay_t<F>>
    static bool constexpr is_viable_initializer =
        std::is_constructible_v<FD, F>;

    using trait = _callable_trait<noex, R, _param_t<Args>...>;
    using vtable = trait::vtable;

    std::reference_wrapper<vtable const> vtbl_ = trait::abstract_base;
    _move_only_pointer obj_;

  public:
    using result_type = R;

    move_only_function() = default;
    move_only_function(std::nullptr_t) noexcept : move_only_function() {}

    template<class F>
    move_only_function(F &&f)
        requires _is_not_self<F, move_only_function> and
                 _does_not_specialize<F, std::in_place_type_t> and
                 is_callable_from<std::decay_t<F>> and is_viable_initializer<F>
    {
        if constexpr (_looks_nullable_to<F, move_only_function>)
        {
            if (f == nullptr)
                return;
        }

        vtbl_ = trait::template callable_target<std::unwrap_ref_decay_t<F>,
                                                inv_quals_f>;
        obj_ = std23::_take_reference(std::forward<F>(f));
    }

    move_only_function(move_only_function &&) = default;
    move_only_function &operator=(move_only_function &&) = default;

    void swap(move_only_function &other) noexcept
    {
        std::swap<move_only_function>(*this, other);
    }

    friend void swap(move_only_function &lhs, move_only_function &rhs) noexcept
    {
        lhs.swap(rhs);
    }

    ~move_only_function() { vtbl_.get().destroy(obj_.val); }

    explicit operator bool() const noexcept
    {
        return &vtbl_.get() != &trait::abstract_base;
    }

    friend bool operator==(move_only_function const &f, std::nullptr_t) noexcept
    {
        return !f;
    }

    R operator()(Args... args) noexcept(noex)
        requires(!is_const and !is_lvalue_only and !is_rvalue_only)
    {
        return vtbl_.get().call(obj_.val, std::forward<Args>(args)...);
    }

    R operator()(Args... args) const noexcept(noex)
        requires(is_const and !is_lvalue_only and !is_rvalue_only)
    {
        return vtbl_.get().call(obj_.val, std::forward<Args>(args)...);
    }

    R operator()(Args... args) &noexcept(noex)
        requires(!is_const and is_lvalue_only and !is_rvalue_only)
    {
        return vtbl_.get().call(obj_.val, std::forward<Args>(args)...);
    }

    R operator()(Args... args) const &noexcept(noex)
        requires(is_const and is_lvalue_only and !is_rvalue_only)
    {
        return vtbl_.get().call(obj_.val, std::forward<Args>(args)...);
    }

    R operator()(Args... args) &&noexcept(noex)
        requires(!is_const and !is_lvalue_only and is_rvalue_only)
    {
        return vtbl_.get().call(obj_.val, std::forward<Args>(args)...);
    }

    R operator()(Args... args) const &&noexcept(noex)
        requires(is_const and !is_lvalue_only and is_rvalue_only)
    {
        return vtbl_.get().call(obj_.val, std::forward<Args>(args)...);
    }
};

} // namespace std23

#endif
