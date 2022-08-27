#pragma once

#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>

// idea of https://stackoverflow.com/a/24687161/8524139

template <bool... Bs> using bool_sequence = std::integer_sequence<bool, Bs...>;

template <bool... Bs> using bool_and = std::is_same<bool_sequence<Bs...>, bool_sequence<(Bs || true)...>>;

template <bool... Bs> using bool_or = std::integral_constant<bool, !bool_and<!Bs...>::value>;

namespace std_ext
{
template <typename R, bool... Bs> // note: R first, no default :(
using enable_if_any = std::enable_if<bool_or<Bs...>::value, R>;

template <typename R, bool... Bs> // note: R first, no default :(
using enable_if_all = std::enable_if<bool_and<Bs...>::value, R>;

template <typename T, typename... Ts> using are_same = bool_and<std::is_same<T, Ts>::value...>;

template <typename Enum> constexpr std::underlying_type_t<Enum> to_underlying(Enum value) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(value);
};

template <typename...> using to_void = void; // maps everything to void, used in non-evaluated contexts

template <typename T, typename = void> struct is_container : std::false_type
{
};

namespace detail
{
    template <typename... Ts> struct is_container_helper
    {
    };
}

template <typename T>
struct is_container<T,
    std::conditional_t<false,
        detail::is_container_helper<typename T::value_type, typename T::size_type, typename T::iterator,
            typename T::const_iterator, decltype(std::declval<T>().size()), decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end()), decltype(std::declval<T>().cbegin()),
            decltype(std::declval<T>().cend())>,
        void>> : public std::true_type
{
};

namespace detail
{
    template <class F, size_t... I> constexpr void for_constexpr_impl(F &&func, std::index_sequence<I...>)
    {
        (func(std::integral_constant<std::size_t, I> {}), ...);
    }
}

template <size_t C, class F> constexpr void for_constexpr(F &&func)
{
    // check in runtime without std::forward
    std_ext::detail::for_constexpr_impl(std::forward<F>(func), std::make_index_sequence<C> {});
}

template <typename T, typename F> static constexpr bool is_variant_alternative()
{
    constexpr auto size = std::variant_size_v<F>;
    bool state = false;
    for_constexpr<size>([&](auto index) {
        if constexpr (std::is_same_v<T, std::variant_alternative_t<index, F>>)
        {
            state = true;
        }
    });
    return state;
}

template <class Variant> std::type_info const &variant_type(Variant const &v)
{
    return std::visit([](auto &&x) -> decltype(auto) { return typeid(x); }, v);
}

template <class T> struct remove_cvref
{
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

#if defined(Q_CC_MSVC)
__forceinline
#elif defined(Q_CC_GNU) || defined(Q_CC_CLANG)
__attribute__((always_inline)) inline
#else
inline
#endif
    unsigned int
    clp2(unsigned int x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

}

// helper type for the visitor
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};

// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
