#pragma once

#include <array>
#include <iostream>
#include <variant>
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
    template <typename...> using to_void = void; // maps everything to void, used in non-evaluated contexts

    template <typename T, typename = void> struct is_container : std::false_type
    {
    };

    template <typename T>
    struct is_container<T,
        to_void<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()),
            typename T::value_type>> : std::true_type // will  be enabled for iterable objects
    {
    };

    template <typename T> void print(const T &value)
    {
        if constexpr (std::is_unsigned<T>())
            std::cout << std::hex << +value;
        else if constexpr (is_container<T>())
            for (const auto i : value)
                print(i);
        else
            std::cout << value;
    }

}
using valueType = std::variant<BYTE, WORD, DWORD, BYTE_4t, WORD_4t, BYTE_8t, WORD_8t, BYTE_16t, WORD_16t, float,
    FLOAT_2t, FLOAT_2t_2t, FLOAT_3t, FLOAT_6t, FLOAT_8t>;

}
