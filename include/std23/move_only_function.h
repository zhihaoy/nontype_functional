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
    using inv_quals =
        std::conditional_t<is_lvalue_only or is_rvalue_only, cvref<T>, cv<T> &>;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::conditional_t<noex, std::is_nothrow_invocable_r<R, T..., Args...>,
                           std::is_invocable_r<R, T..., Args...>>::value;

    template<class VT>
    static constexpr bool is_callable_from =
        is_invocable_using<cvref<VT>> and is_invocable_using<inv_quals<VT>>;

    struct pointer
    {
        union value_type
        {
            void *p_ = nullptr;
            void const *cp_;
            void (*fp_)();
        } val;

        pointer() = default;
        pointer(pointer const &) = delete;
        pointer &operator=(pointer const &) = delete;

        pointer(pointer &&other) noexcept : val(std::exchange(other.val, {})) {}

        pointer &operator=(pointer &&other) noexcept
        {
            val = std::exchange(other.val, {});
        }

        template<class T> constexpr auto get() const noexcept
        {
            if constexpr (std::is_const_v<T>)
                return static_cast<T *>(val.cp_);
            else if constexpr (std::is_object_v<T>)
                return static_cast<T *>(val.p_);
            else
                return reinterpret_cast<T *>(val.fp_);
        }
    };

    struct vtable
    {
        typedef R call_t(pointer::value_type, _param_t<Args>...) noexcept(noex);
        typedef void destroy_t(pointer::value_type) noexcept;

        call_t *call = 0;
        destroy_t *destroy = [](auto) noexcept {};
    };

    static inline constinit vtable abstract_base;

    std::reference_wrapper<vtable const> vtbl_ = abstract_base;
    pointer obj_;

  public:
    using result_type = R;

    move_only_function() = default;
    move_only_function(std::nullptr_t) noexcept : move_only_function() {}

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
        return &vtbl_.get() != &abstract_base;
    }

    friend bool operator==(move_only_function const &f, std::nullptr_t) noexcept
    {
        return !f;
    }
};

} // namespace std23

#endif
