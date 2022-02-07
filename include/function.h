#pragma once

#include "__functional_base.h"

#include <memory>
#include <new>

namespace std23
{

template<class Sig> struct _opt_fn_sig;

template<class R, class... Args> struct _opt_fn_sig<R(Args...)>
{
    using function_type = R(Args...);
    static constexpr bool is_variadic = false;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::is_invocable_r_v<R, T..., Args...>;
};

template<class R, class... Args> struct _opt_fn_sig<R(Args..., ...)>
{
    using function_type = R(Args...);
    static constexpr bool is_variadic = true;

    template<class... T>
    static constexpr bool is_invocable_using =
        std::is_invocable_r_v<R, T..., Args..., va_list>;
};

template<class S, class = typename _opt_fn_sig<S>::function_type>
class function;

template<class S, class R, class... Args> class function<S, R(Args...)>
{
    using signature = _opt_fn_sig<S>;

    template<class... T>
    static constexpr bool is_invocable_using =
        signature::template is_invocable_using<T...>;

    struct lvalue_callable
    {
        virtual R operator()(_param_t<Args>...) const = 0;
        virtual ~lvalue_callable() = default;

        void copy_into(std::byte *storage) const { copy_into_(storage); }
        void move_into(std::byte *storage) noexcept { move_into_(storage); }

      protected:
        virtual void copy_into_(void *) const = 0;
        virtual void move_into_(void *) noexcept = 0;
    };

    struct typical_target_object : lvalue_callable
    {
        R operator()(_param_t<Args>...) const override;
        void copy_into_(void *) const override;
        void move_into_(void *) noexcept override;

        union
        {
            void (*fp)() = nullptr;
            void *p;
        };
    };

    struct empty_target_object : lvalue_callable
    {
        [[noreturn]] R operator()(_param_t<Args>...) const override
        {
            throw std::bad_function_call{};
        }

        void copy_into_(void *location) const override
        {
            ::new (location) empty_target_object;
        }

        void move_into_(void *location) noexcept override
        {
            ::new (location) empty_target_object;
        }
    };

    template<class T> class target_object : lvalue_callable
    {
        std::conditional_t<std::is_pointer_v<T>, T, std::unique_ptr<T>> p_;

      public:
        R operator()(_param_t<Args>... args) const override
        {
            return std23::invoke_r<R>(*p_, std::forward<Args>(args)...);
        }

        template<class F>
        explicit target_object(F &&f) requires(
            _is_not_self<F, target_object> and not std::is_pointer_v<T>)
            : p_(std::make_unique<T>(std::forward<F>(f)))
        {}

        explicit target_object(T p) noexcept requires std::is_pointer_v<T>
            : p_(p)
        {}

        void copy_into_(void *location) const override
        {
            if constexpr (std::is_pointer_v<T>)
                ::new (location) auto(*this);
            else
                ::new (location) target_object(*p_);
        }

        void move_into_(void *location) noexcept override
        {
            ::new (location) auto(std::move(*this));
        }
    };

    template<class T> class target_object<T &> : lvalue_callable
    {
        T &target_;

      public:
        R operator()(_param_t<Args>... args) const override
        {
            return std23::invoke_r<R>(target_, std::forward<Args>(args)...);
        }

        explicit target_object(T &target) noexcept : target_(target) {}

        void copy_into_(void *location) const override
        {
            ::new (location) auto(*this);
        }

        void move_into_(void *location) noexcept override
        {
            ::new (location) auto(*this);
        }
    };

    alignas(typical_target_object)
        std::byte storage_[sizeof(typical_target_object)];

    auto storage_location() noexcept -> void * { return &storage_; }
    auto target() const noexcept
    {
        return std::launder(
            reinterpret_cast<lvalue_callable const *>(&storage_));
    }

  public:
    using result_type = R;

    function() noexcept { ::new (storage_location()) empty_target_object; }
    function(std::nullptr_t) noexcept : function() {}

    template<class F>
    function(F &&f) requires _is_not_self<F, function> and is_invocable_using<F>
    {
        using T = target_object<std::unwrap_ref_decay_t<F>>;
        static_assert(sizeof(T) <= sizeof(storage_));

        ::new (storage_location()) T(std::forward<F>(f));
    }

    function(function const &other) { other.target()->copy_into(&storage_); }

    function(function &&other) noexcept
    {
        other.target()->move_into(&storage_);
    }

    ~function() { target()->~lvalue_callable(); }

    explicit operator bool() const noexcept
    {
        return dynamic_cast<empty_target_object const *>(target()) == nullptr;
    }

    friend bool operator==(function const &f, std::nullptr_t) noexcept
    {
        return !f;
    }

    R operator()(Args... args) const
    {
        return (*target())(std::forward<Args>(args)...);
    }
};

// clang-format off

template<class F>
requires std::is_function_v<F>
function(F *) -> function<F>;

// clang-format on

template<class T>
function(T) -> function<_drop_first_arg_to_invoke_t<decltype(&T::operator())>>;

} // namespace std23