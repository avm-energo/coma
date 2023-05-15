#ifndef MODULE_KXX_H
#define MODULE_KXX_H

#include "../interfaces/modbusprivate.h"

#include <gen/pch.h>

namespace ModbusItem
{

enum DataType : quint8
{
    Data_Uint = 0,
    Data_Int = 1,
    Data_Float = 4
};

struct INICOM
{
    // Скорость порта
    MBS::BaudRate baud : 4;
    // Биты данных
    quint8 bits : 1;
    // Четность
    MBS::Parity parity : 2;
    // Стоп биты
    MBS::StopBits stop : 1;
    friend bool operator==(const INICOM &lhs, const INICOM &rhs);
    friend bool operator!=(const INICOM &lhs, const INICOM &rhs);
};

bool inline operator==(const INICOM &lhs, const INICOM &rhs)
{
    return (lhs.baud == rhs.baud) && (lhs.bits && rhs.bits) && (lhs.parity == rhs.parity) && (lhs.stop == rhs.stop);
}

bool inline operator!=(const INICOM &lhs, const INICOM &rhs)
{
    return !(lhs == rhs);
}

enum SensorType : quint8
{
    SEN_None,
    SEN_Universal
};

/// \brief Enumeration for type identification.
enum TypeId : quint8
{
    Uint16 = 0, ///< 1 register contains 16 bit uint (WORD)
    Int16 = 1,  ///< 1 register contains 16 bit int
    Bool,       ///< 1 register contains 1 bit
    Uint32,     ///< 2 registers contain uint32 (bitstring)
    Float,      ///< 2 registers contain float
    // ...
    // Smth else
    // ...
    None = 255
};

struct TypeR
{
    // Функция
    MBS::Commands reg : 4;
    // Тип данных
    TypeId dat : 4;
    friend bool operator==(const TypeR &lhs, const TypeR &rhs);
    friend bool operator!=(const TypeR &lhs, const TypeR &rhs);
};

#pragma pack(push, 1)
struct Item
{
    SensorType typedat; ///< Тип датчика
    INICOM parport;     ///< Параметры порта
    quint8 per;         ///< Период опроса
    quint8 adr;         ///< Адрес
    TypeR type;
    quint16 reg; ///< Начальный адрес регистра
    quint8 cnt;  ///< Количество регистров

    friend bool operator==(const Item &lhs, const Item &rhs);
    friend bool operator!=(const Item &lhs, const Item &rhs);
};
#pragma pack(pop)

bool inline operator==(const Item &lhs, const Item &rhs)
{
    return (lhs.typedat == rhs.typedat) && (lhs.parport == rhs.parport) && (lhs.per == rhs.per) && (lhs.adr == rhs.adr)
        && (lhs.reg == rhs.reg) && (lhs.cnt == rhs.cnt);
}

bool inline operator!=(const Item &lhs, const Item &rhs)
{
    return !(lhs == rhs);
}

}
#endif // MODULE_KXX_H
