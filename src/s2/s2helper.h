#pragma once

#include "s2datatypes.h"

#include <gen/std_ext.h>
#include <iostream>

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
