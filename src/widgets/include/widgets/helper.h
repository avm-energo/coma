#pragma once

namespace detail
{
constexpr int defaultRatio = 3;
constexpr int maxRatio = 5;

inline int goldenRatio(int value)
{
    for (auto i = maxRatio; i != defaultRatio; --i)
    {
        if (!(value % i))
            return i;
    }
    return defaultRatio;
}
}
