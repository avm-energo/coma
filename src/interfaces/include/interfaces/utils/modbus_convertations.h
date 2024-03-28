#pragma once

#include <QByteArray>

namespace Modbus
{

template <typename T, std::size_t N = sizeof(T)> //
inline QByteArray packRegister(T value)
{
    static_assert(N % 2 == 0, "The size of type T must be even");
    static_assert(N >= 2, "The size of type T must be greater than or equal to 2");
    QByteArray ba;
    ba.reserve(N);
    auto srcBegin = reinterpret_cast<std::uint8_t *>(&value);
    auto srcEnd = srcBegin + N;
    for (auto it = srcBegin; it != srcEnd; it = it + 2)
    {
        ba.push_back(*(it + 1));
        ba.push_back(*it);
    }
    return ba;
}

inline QByteArray packRegister(const QByteArray &value)
{
    const auto N = value.size();
    Q_ASSERT(N % 2 == 0);
    Q_ASSERT(N >= 2);
    QByteArray ba;
    ba.reserve(value.size());
    for (auto it = value.cbegin(); it != value.cend(); it = it + 2)
    {
        ba.push_back(*(it + 1));
        ba.push_back(*it);
    }
    return ba;
}

template <typename T, std::size_t N = sizeof(T)> //
inline T unpackRegister(const QByteArray &ba)
{
    assert(N == ba.size());
    T value;
    auto dstBegin = reinterpret_cast<std::uint8_t *>(&value);
    std::copy(ba.cbegin(), ba.cend(), dstBegin);
    for (std::size_t i = 0; i < N; i += 2)
        std::swap(dstBegin[i], dstBegin[i + 1]);
    return value;
}

} // namespace Modbus
