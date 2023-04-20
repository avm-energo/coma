#pragma once

#include "s2datatypes.h"

#include <array>
#include <gen/std_ext.h>
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
using BYTE_6t = std::array<BYTE, 6>;
using WORD_6t = std::array<WORD, 6>;
using DWORD_6t = std::array<DWORD, 6>;
using BYTE_8t = std::array<BYTE, 8>;
using WORD_8t = std::array<WORD, 8>;
using DWORD_8t = std::array<DWORD, 8>;
using BYTE_16t = std::array<BYTE, 16>;
using WORD_16t = std::array<WORD, 16>;
using DWORD_16t = std::array<DWORD, 16>;
using BYTE_32t = std::array<BYTE, 32>;
using WORD_32t = std::array<WORD, 32>;
using DWORD_32t = std::array<DWORD, 32>;
using FLOAT = float;
using FLOAT_2t = std::array<FLOAT, 2>;
using FLOAT_3t = std::array<FLOAT, 3>;
using FLOAT_4t = std::array<FLOAT, 4>;
using FLOAT_6t = std::array<FLOAT, 6>;
using FLOAT_8t = std::array<FLOAT, 8>;

static_assert(sizeof(BYTE) != sizeof(WORD), "Broken datatypes");
static_assert(sizeof(BYTE) != sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(INT32) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(FLOAT) == sizeof(DWORD), "Broken datatypes");
static_assert(sizeof(WORD_4t) == sizeof(BYTE_8t), "Broken datatypes");
static_assert(sizeof(DWORD_4t) == sizeof(BYTE_16t), "Broken datatypes");
static_assert(sizeof(DWORD_8t) == sizeof(BYTE_32t), "Broken datatypes");
static_assert(sizeof(FLOAT_2t) == sizeof(BYTE_8t), "Broken datatypes");
static_assert(sizeof(FLOAT_4t) == sizeof(BYTE_16t), "Broken datatypes");
static_assert(sizeof(FLOAT_8t) == sizeof(BYTE_32t), "Broken datatypes");

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
    BYTE_6t, WORD_6t, DWORD_6t,                          //
    BYTE_8t, WORD_8t, DWORD_8t,                          //
    BYTE_16t, WORD_16t, DWORD_16t,                       //
    BYTE_32t, WORD_32t, DWORD_32t,                       //
    FLOAT, FLOAT_2t, FLOAT_3t, FLOAT_4t, FLOAT_6t, FLOAT_8t>;
}
