#pragma once

#include <QByteArray>

namespace utils
{

class CRC32
{
private:
    static const quint32 table[256];
    quint32 checksum;

    void updateChecksum(const quint8 byte);

public:
    explicit CRC32() noexcept;
    explicit CRC32(const quint32 value) noexcept;
    explicit CRC32(const QByteArray &data);

    void update(const QByteArray &data);
    void update(const quint8 *data, const quint32 size);
    void reset() noexcept;

    operator quint32() const noexcept;
    friend bool operator==(const CRC32 lhs, const CRC32 rhs) noexcept;
    friend bool operator==(const quint32 lhs, const CRC32 rhs) noexcept;
    friend bool operator==(const CRC32 lhs, const quint32 rhs) noexcept;
    friend bool operator!=(const CRC32 lhs, const CRC32 rhs) noexcept;
    friend bool operator!=(const quint32 lhs, const CRC32 rhs) noexcept;
    friend bool operator!=(const CRC32 lhs, const quint32 rhs) noexcept;
};

bool operator==(const CRC32 lhs, const CRC32 rhs) noexcept;
bool operator==(const quint32 lhs, const CRC32 rhs) noexcept;
bool operator==(const CRC32 lhs, const quint32 rhs) noexcept;
bool operator!=(const CRC32 lhs, const CRC32 rhs) noexcept;
bool operator!=(const quint32 lhs, const CRC32 rhs) noexcept;
bool operator!=(const CRC32 lhs, const quint32 rhs) noexcept;

}
