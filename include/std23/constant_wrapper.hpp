// This file has been copied from
// https://github.com/tzlaine/constexpr/blob/4df3d5d472f40c80aed70e7997f70c60145470db/include/constant_wrapper.hpp
// with the following changes applied:
// - rename namespace from std to std23
// - add std:: where needed
// - add #include <cstdint>
// - add #pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

#pragma once

namespace std23
{

namespace exposition_only {
  template<typename T>
  struct cw_fixed_value; // exposition only
}

template<exposition_only::cw_fixed_value X,
         typename unspecified = typename decltype(exposition_only::cw_fixed_value(X))::type> // exposition only
struct constant_wrapper;

template<class T>
concept constexpr_param = requires { typename constant_wrapper<T::value>; }; // exposition only

namespace exposition_only {
  template<typename T>
  struct cw_fixed_value { // exposition only
    using type = T;
    constexpr cw_fixed_value(type v) noexcept: data(v) { }
    T data;
  };

  template<typename T, std::size_t Extent> struct cw_fixed_value<T[Extent]>
  { // exposition only
      using type = T[Extent];
      constexpr cw_fixed_value(T (&arr)[Extent]) noexcept
          : cw_fixed_value(arr, std::make_index_sequence<Extent>())
      {}
      T data[Extent];

    private:
      template<std::size_t... Idx>
      constexpr cw_fixed_value(T (&arr)[Extent],
                               std::index_sequence<Idx...>) noexcept
          : data{arr[Idx]...}
      {}
  };

  template<typename T, std::size_t Extent>
  cw_fixed_value(T (&)[Extent]) -> cw_fixed_value<T[Extent]>; // exposition only
  template<typename T>
  cw_fixed_value(T) -> cw_fixed_value<T>;                     // exposition only

  struct cw_operators { // exposition only
    // unary operators
    template<constexpr_param T>
      friend constexpr auto operator+(T) noexcept -> constant_wrapper<(+T::value)> { return {}; }
    template<constexpr_param T>
      friend constexpr auto operator-(T) noexcept -> constant_wrapper<(-T::value)> { return {}; }
    template<constexpr_param T>
      friend constexpr auto operator~(T) noexcept -> constant_wrapper<(~T::value)> { return {}; }
    template<constexpr_param T>
      friend constexpr auto operator!(T) noexcept -> constant_wrapper<(!T::value)> { return {}; }
    template<constexpr_param T>
      friend constexpr auto operator&(T) noexcept -> constant_wrapper<(&T::value)> { return {}; }
    template<constexpr_param T>
      friend constexpr auto operator*(T) noexcept -> constant_wrapper<(*T::value)> { return {}; }

    // binary operators
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator+(L, R) noexcept -> constant_wrapper<(L::value + R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator-(L, R) noexcept -> constant_wrapper<(L::value - R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator*(L, R) noexcept -> constant_wrapper<(L::value * R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator/(L, R) noexcept -> constant_wrapper<(L::value / R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator%(L, R) noexcept -> constant_wrapper<(L::value % R::value)> { return {}; }

    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator<<(L, R) noexcept -> constant_wrapper<(L::value << R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator>>(L, R) noexcept -> constant_wrapper<(L::value >> R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator&(L, R) noexcept -> constant_wrapper<(L::value & R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator|(L, R) noexcept -> constant_wrapper<(L::value | R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator^(L, R) noexcept -> constant_wrapper<(L::value ^ R::value)> { return {}; }

    template<constexpr_param L, constexpr_param R>
    requires(!std::is_constructible_v<bool, decltype(L::value)> ||
             !std::is_constructible_v<bool, decltype(R::value)>)
    friend constexpr auto operator&&(L, R) noexcept
        -> constant_wrapper<(L::value && R::value)>
    {
        return {};
    }
    template<constexpr_param L, constexpr_param R>
    requires(!std::is_constructible_v<bool, decltype(L::value)> ||
             !std::is_constructible_v<bool, decltype(R::value)>)
    friend constexpr auto operator||(L, R) noexcept
        -> constant_wrapper<(L::value || R::value)>
    {
        return {};
    }

    // comparisons
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator<=>(L, R) noexcept -> constant_wrapper<(L::value <=> R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator<(L, R) noexcept -> constant_wrapper<(L::value < R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator<=(L, R) noexcept -> constant_wrapper<(L::value <= R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator==(L, R) noexcept -> constant_wrapper<(L::value == R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator!=(L, R) noexcept -> constant_wrapper<(L::value != R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator>(L, R) noexcept -> constant_wrapper<(L::value > R::value)> { return {}; }
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator>=(L, R) noexcept -> constant_wrapper<(L::value >= R::value)> { return {}; }

    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator,(L, R) noexcept = delete;
    template<constexpr_param L, constexpr_param R>
      friend constexpr auto operator->*(L, R) noexcept -> constant_wrapper<L::value->*R::value> { return {}; }

    // call and index
    template<constexpr_param T, constexpr_param... Args>
      constexpr auto operator()(this T, Args...) noexcept
        requires requires(Args...) { constant_wrapper<T::value(Args::value...)>(); }
          { return constant_wrapper<T::value(Args::value...)>{}; }
    template<constexpr_param T, constexpr_param... Args>
      constexpr auto operator[](this T, Args...) noexcept -> constant_wrapper<(T::value[Args::value...])>
        { return {}; }

    // pseudo-mutators
    template<constexpr_param T>
      constexpr auto operator++(this T) noexcept requires requires(T::value_type x) { ++x; }
        { return constant_wrapper<[] { auto c = T::value; return ++c; }()>{}; }
    template<constexpr_param T>
      constexpr auto operator++(this T, int) noexcept requires requires(T::value_type x) { x++; }
        { return constant_wrapper<[] { auto c = T::value; return c++; }()>{}; }

    template<constexpr_param T>
      constexpr auto operator--(this T) noexcept requires requires(T::value_type x) { --x; }
        { return constant_wrapper<[] { auto c = T::value; return --c; }()>{}; }
    template<constexpr_param T>
      constexpr auto operator--(this T, int) noexcept requires requires(T::value_type x) { x--; }
        { return constant_wrapper<[] { auto c = T::value; return c--; }()>{}; }

    template<constexpr_param T, constexpr_param R>
      constexpr auto operator+=(this T, R) noexcept requires requires(T::value_type x) { x += R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v += R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator-=(this T, R) noexcept requires requires(T::value_type x) { x -= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v -= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator*=(this T, R) noexcept requires requires(T::value_type x) { x *= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v *= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator/=(this T, R) noexcept requires requires(T::value_type x) { x /= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v /= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator%=(this T, R) noexcept requires requires(T::value_type x) { x %= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v %= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator&=(this T, R) noexcept requires requires(T::value_type x) { x &= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v &= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator|=(this T, R) noexcept requires requires(T::value_type x) { x |= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v |= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator^=(this T, R) noexcept requires requires(T::value_type x) { x ^= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v ^= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator<<=(this T, R) noexcept requires requires(T::value_type x) { x <<= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v <<= R::value; }()>{}; }
    template<constexpr_param T, constexpr_param R>
      constexpr auto operator>>=(this T, R) noexcept requires requires(T::value_type x) { x >>= R::value; }
        { return constant_wrapper<[] { auto v = T::value; return v >>= R::value; }()>{}; }
  };
}

template<exposition_only::cw_fixed_value X, typename>
struct constant_wrapper: exposition_only::cw_operators {
  static constexpr const auto & value = X.data;
  using type = constant_wrapper;
  using value_type = typename decltype(X)::type;

  template<constexpr_param R>
    constexpr auto operator=(R) const noexcept requires requires(value_type x) { x = R::value; }
      { return constant_wrapper<[] { auto v = value; return v = R::value; }()>{}; }

  constexpr operator decltype(auto)() const noexcept { return value; }
};

template<exposition_only::cw_fixed_value X>
  constinit auto cw = constant_wrapper<X>{};

} // namespace std23
