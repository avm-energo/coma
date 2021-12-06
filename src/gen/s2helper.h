#pragma once

#include "std_ext.h"

#include <array>
#include <iostream>

namespace DataTypes
{
using BYTE = uint8_t;
using WORD = uint16_t;
using DWORD = uint32_t;
using INT32 = int32_t;
using BYTE_4t = std::array<BYTE, 4>;
using WORD_4t = std::array<WORD, 4>;
using DWORD_4t = std::array<DWORD, 4>;
using BYTE_8t = std::array<BYTE, 8>;
using WORD_8t = std::array<WORD, 8>;
using DWORD_8t = std::array<DWORD, 8>;
using BYTE_16t = std::array<BYTE, 16>;
using WORD_16t = std::array<WORD, 16>;
using DWORD_16t = std::array<DWORD, 16>;
using BYTE_32t = std::array<BYTE, 32>;
using WORD_32t = std::array<WORD, 32>;
using DWORD_32t = std::array<DWORD, 32>;
using FLOAT_2t = std::array<float, 2>;
using FLOAT_3t = std::array<float, 3>;
using FLOAT_4t = std::array<float, 4>;
using FLOAT_6t = std::array<float, 6>;
using FLOAT_8t = std::array<float, 8>;

static_assert(sizeof(BYTE) != sizeof(WORD), "Broken datatypes");
static_assert(sizeof(BYTE) != sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(INT32) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(float) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(WORD_4t) == sizeof(BYTE_8t), "Broken datatypes");
static_assert(sizeof(DWORD_4t) == sizeof(BYTE_16t), "Broken datatypes");
static_assert(sizeof(FLOAT_2t) == sizeof(BYTE_8t), "Broken datatypes");
namespace detail
{

    template <typename T> void print(const T &value)
    {
        if constexpr (std::is_unsigned<T>())
            std::cout << std::hex << +value << std::dec;
        else if constexpr (std_ext::is_container<T>())
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
using valueType = std::variant<BYTE, WORD, DWORD, INT32, //
    BYTE_4t, WORD_4t, DWORD_4t,                          //
    BYTE_8t, WORD_8t, DWORD_8t,                          //
    BYTE_16t, WORD_16t, DWORD_16t,                       //
    BYTE_32t, WORD_32t, DWORD_32t,                       //
    float, FLOAT_2t, FLOAT_3t, FLOAT_4t, FLOAT_6t, FLOAT_8t>;

}
