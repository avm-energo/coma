#pragma once

#include <QByteArray>

namespace utils
{

class CRC16
{
private:
    static const quint8 hiTable[256];
    static const quint8 loTable[256];
    quint8 hiByte;
    quint8 loByte;

    void updateChecksum(const quint8 byte);

public:
    explicit CRC16() noexcept;
    explicit CRC16(const quint16 value) noexcept;
    explicit CRC16(const QByteArray &data);

    void update(const QByteArray &data);
    void update(const quint8 *data, const quint32 size);
    void reset() noexcept;
    void appendTo(QByteArray &array) const;

    operator quint16() const noexcept;
    friend bool operator==(const CRC16 lhs, const CRC16 rhs) noexcept;
    friend bool operator==(const quint16 lhs, const CRC16 rhs) noexcept;
    friend bool operator==(const CRC16 lhs, const quint16 rhs) noexcept;
    friend bool operator!=(const CRC16 lhs, const CRC16 rhs) noexcept;
    friend bool operator!=(const quint16 lhs, const CRC16 rhs) noexcept;
    friend bool operator!=(const CRC16 lhs, const quint16 rhs) noexcept;
};

bool operator==(const CRC16 lhs, const CRC16 rhs) noexcept;
bool operator==(const quint16 lhs, const CRC16 rhs) noexcept;
bool operator==(const CRC16 lhs, const quint16 rhs) noexcept;
bool operator!=(const CRC16 lhs, const CRC16 rhs) noexcept;
bool operator!=(const quint16 lhs, const CRC16 rhs) noexcept;
bool operator!=(const CRC16 lhs, const quint16 rhs) noexcept;

}
