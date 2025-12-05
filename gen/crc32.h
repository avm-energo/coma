#pragma once

#include <QByteArray>
#include <gen/gen_export.h>

namespace utils
{

/// \brief Класс, реализующий подсчёт контрольной суммы CRC32.
class GENLIB_EXPORT CRC32
{
private:
    static const quint32 table[256];
    quint32 checksum;

    /// \brief Обновление хранящейся контрольной суммы входящим байтом данных.
    void updateChecksum(const quint8 byte);

public:
    /// \brief Конструктор по умолчанию.
    explicit CRC32() noexcept;
    /// \brief Конструктор, инициализирующий объект класса контрольной суммой из переменной value.
    explicit CRC32(const quint32 value) noexcept;
    /// \brief Конструктор, рассчитывающий контрольную сумму переданного массива байт data.
    explicit CRC32(const QByteArray &data);

    /// \brief Обновляет контрольную сумму переданным массивом байт data.
    void update(const QByteArray &data);
    /// \brief Обновляет контрольную сумму переданным указателем на данные и размером данных.
    void update(const quint8 *data, const quint32 size);
    /// \brief Сбрасывает контрольную сумму до начального значения.
    void reset() noexcept;

    /// \brief Оператор приведения к типу quint32.
    operator quint32() const noexcept;
    friend bool GENLIB_EXPORT operator==(const CRC32 lhs, const CRC32 rhs) noexcept;
    friend bool GENLIB_EXPORT operator==(const quint32 lhs, const CRC32 rhs) noexcept;
    friend bool GENLIB_EXPORT operator==(const CRC32 lhs, const quint32 rhs) noexcept;
    friend bool GENLIB_EXPORT operator!=(const CRC32 lhs, const CRC32 rhs) noexcept;
    friend bool GENLIB_EXPORT operator!=(const quint32 lhs, const CRC32 rhs) noexcept;
    friend bool GENLIB_EXPORT operator!=(const CRC32 lhs, const quint32 rhs) noexcept;
};

bool GENLIB_EXPORT operator==(const CRC32 lhs, const CRC32 rhs) noexcept;
bool GENLIB_EXPORT operator==(const quint32 lhs, const CRC32 rhs) noexcept;
bool GENLIB_EXPORT operator==(const CRC32 lhs, const quint32 rhs) noexcept;
bool GENLIB_EXPORT operator!=(const CRC32 lhs, const CRC32 rhs) noexcept;
bool GENLIB_EXPORT operator!=(const quint32 lhs, const CRC32 rhs) noexcept;
bool GENLIB_EXPORT operator!=(const CRC32 lhs, const quint32 rhs) noexcept;

}
