#pragma once

#include <QByteArray>

namespace utils
{

/// \brief Класс, реализующий подсчёт контрольной суммы CRC16.
class CRC16
{
private:
    static const quint8 hiTable[256];
    static const quint8 loTable[256];
    quint8 hiByte;
    quint8 loByte;

    /// \brief Обновление хранящейся контрольной суммы входящим байтом данных.
    void updateChecksum(const quint8 byte);

public:
    /// \brief Конструктор по умолчанию.
    explicit CRC16() noexcept;
    /// \brief Конструктор, инициализирующий объект класса контрольной суммой из переменной value.
    explicit CRC16(const quint16 value) noexcept;
    /// \brief Конструктор, рассчитывающий контрольную сумму переданного массива байт data.
    explicit CRC16(const QByteArray &data);

    /// \brief Обновляет контрольную сумму переданным массивом байт data.
    void update(const QByteArray &data);
    /// \brief Обновляет контрольную сумму переданным указателем на данные и размером данных.
    void update(const quint8 *data, const quint32 size);
    /// \brief Сбрасывает контрольную сумму до начального значения.
    void reset() noexcept;
    /// \brief Добавляет контрольную сумму в конец переданного массива данных array.
    void appendTo(QByteArray &array) const;

    /// \brief Оператор приведения к типу quint16.
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
