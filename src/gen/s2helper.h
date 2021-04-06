#pragma once

#include <array>
#include <iostream>
#include <variant>

namespace std
{
template <typename...> using to_void = void; // maps everything to void, used in non-evaluated contexts

template <typename T, typename = void> struct is_container : false_type
{
};

template <typename T>
struct is_container<T,
    to_void<decltype(declval<T>().begin()), decltype(declval<T>().end()),
        typename T::value_type>> : true_type // will  be enabled for iterable objects
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
    std::detail::for_constexpr_impl(std::forward<F>(func), std::make_index_sequence<C> {});
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
}

namespace DataTypes
{
using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned int;
using BYTE_4t = std::array<BYTE, 4>;
using WORD_4t = std::array<WORD, 4>;
using BYTE_8t = std::array<BYTE, 8>;
using WORD_8t = std::array<WORD, 8>;
using BYTE_16t = std::array<BYTE, 16>;
using WORD_16t = std::array<WORD, 16>;
using FLOAT_2t = std::array<float, 2>;
using FLOAT_2t_2t = std::array<FLOAT_2t, 2>;
using FLOAT_3t = std::array<float, 3>;
using FLOAT_6t = std::array<float, 6>;
using FLOAT_8t = std::array<float, 8>;

namespace detail
{

    template <typename T> void print(const T &value)
    {
        if constexpr (std::is_unsigned<T>())
            std::cout << std::hex << +value << std::dec;
        else if constexpr (std::is_container<T>())
        {
            std::cout << "[ ";
            size_t counter = 0;
            for (const auto i : value)
            {
                print(i);
                if (++counter < value.size())
                {
                    std::cout << ",";
                }
            }
            std::cout << " ]";
        }
        else
            std::cout << value;
    }

}
using valueType = std::variant<BYTE, WORD, DWORD, BYTE_4t, WORD_4t, BYTE_8t, WORD_8t, BYTE_16t, WORD_16t, float,
    FLOAT_2t, FLOAT_2t_2t, FLOAT_3t, FLOAT_6t, FLOAT_8t>;

}
