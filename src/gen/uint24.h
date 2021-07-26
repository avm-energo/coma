#pragma once
#include <bitset>
#include <cstdint>
const int UINT24_MAX = 16777215;

class uint24
{
protected:
    unsigned char value[3];

public:
    uint24()
    {
    }

    uint24(const uint24 &val)
    {
        *this = val;
    }

    uint24(const uint32_t &val)
    {
        *this = val;
    }

    operator uint32_t() const
    {
        /* Sign extend negative quantities */
        if (value[2] & 0x80)
        {
            return (0xff << 24) | (value[2] << 16) | (value[1] << 8) | value[0];
        }
        else
        {
            return (value[2] << 16) | (value[1] << 8) | value[0];
        }
    }

    uint24 &operator=(const uint24 &input)
    {
        value[0] = input.value[0];
        value[1] = input.value[1];
        value[2] = input.value[2];

        return *this;
    }

    uint24 &operator=(const uint32_t input)
    {
        value[0] = ((unsigned char *)&input)[0];
        value[1] = ((unsigned char *)&input)[1];
        value[2] = ((unsigned char *)&input)[2];

        return *this;
    }

    uint24 operator+(const uint24 &val) const
    {
        return uint24((uint32_t) * this + (uint32_t)val);
    }

    uint24 operator-(const uint24 &val) const
    {
        return uint24((uint32_t) * this - (uint32_t)val);
    }

    uint24 operator*(const uint24 &val) const
    {
        return uint24((uint32_t) * this * (uint32_t)val);
    }

    uint24 operator/(const uint24 &val) const
    {
        return uint24((uint32_t) * this / (uint32_t)val);
    }

    uint24 &operator+=(const uint24 &val)
    {
        *this = *this + val;
        return *this;
    }

    uint24 &operator-=(const uint24 &val)
    {
        *this = *this - val;
        return *this;
    }

    uint24 &operator*=(const uint24 &val)
    {
        *this = *this * val;
        return *this;
    }

    uint24 &operator/=(const uint24 &val)
    {
        *this = *this / val;
        return *this;
    }

    operator bool() const
    {
        return (uint32_t) * this != 0;
    }

    bool operator!() const
    {
        return !((int)*this);
    }

    //    Uint24 operator- ()
    //    {
    //        return Uint24( -(uint32_t)*this );
    //    }

    bool operator==(const uint24 &val) const
    {
        return (int)*this == (int)val;
    }

    bool operator!=(const uint24 &val) const
    {
        return (int)*this != (int)val;
    }

    bool operator>=(const uint24 &val) const
    {
        return (int)*this >= (int)val;
    }

    bool operator<=(const uint24 &val) const
    {
        return (int)*this <= (int)val;
    }

    /* Define all operations you need below.. */
};

class ui24
{
    std::bitset<24> m_value;

public:
    ui24() = default;
    constexpr ui24(unsigned int value) noexcept : m_value { value }
    {
    }

    operator uint32_t() const
    {
        //  constexpr std::uint32_t negative_mask = (0xff << 24);
        return /*(m_value[23] ? negative_mask : 0) |*/ m_value.to_ulong();
    }
};
