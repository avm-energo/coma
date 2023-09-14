#pragma once

#include <interfaces/modbusprivate.h>

namespace ModbusItem
{

enum DataType : quint8
{
    Data_Uint = 0,
    Data_Int = 1,
    Data_Float = 4
};

enum Parity : quint8
{
    NoParity = 0,
    EvenParity = 1,
    OddParity = 2
};

enum StopBits : quint8
{
    OneStop = 0,
    TwoStop = 1
};

struct INICOM
{
    // Скорость порта
    quint16 baud;
    // Четность
    Parity parity;
    // Стоп биты
    StopBits stop;
    friend bool operator==(const INICOM &lhs, const INICOM &rhs);
    friend bool operator!=(const INICOM &lhs, const INICOM &rhs);
};

bool inline operator==(const INICOM &lhs, const INICOM &rhs)
{
    return (lhs.baud == rhs.baud) && (lhs.parity == rhs.parity) && (lhs.stop == rhs.stop);
}

bool inline operator!=(const INICOM &lhs, const INICOM &rhs)
{
    return !(lhs == rhs);
}

enum SensorType : quint8
{
    SEN_None = 0,      // no sensors
    SEN_Universal = 1, // custom sensor - all parameters are active
    SEN_1 = 2,         // sensor type 1 - all parameters active but Period,Adr,TypeData,Func,StReg & CntReg
    // ....
};

/// \brief Enumeration for type identification.
enum TypeId : quint8
{
    Uint16 = 0, ///< 1 register contains 16 bit uint (WORD)
    Int16 = 1,  ///< 1 register contains 16 bit int
    Bool = 2,   ///< 1 register contains 1 bit
    Uint32 = 3, ///< 2 registers contain uint32 (bitstring)
    Float1 = 4, ///< 2 registers contain float (big-endian, standard byte-order)
    Float2 = 5, ///< 2 registers contain float (little-endian, standard byte-order)
    Float3 = 6, ///< 2 registers contain float (big-endian, reverse byte-order)
    Float4 = 7, ///< 2 registers contain float (little-endian, reverse byte-order)
    // ...
    // Smth else
    // ...
    None = 255
};

#pragma pack(push, 1)
struct Item
{
    SensorType typedat; ///< Тип датчика
    INICOM parport;     ///< Параметры порта
    quint8 per;         ///< Период опроса
    quint8 adr;         ///< Адрес
    TypeId typedata;    ///< Тип данных
    quint8 func;        ///< код функции modbus
    quint16 reg;        ///< Начальный адрес регистра
    quint8 cnt;         ///< Количество регистров

    friend bool operator==(const Item &lhs, const Item &rhs);
    friend bool operator!=(const Item &lhs, const Item &rhs);
};
#pragma pack(pop)

bool inline operator==(const Item &lhs, const Item &rhs)
{
    return (lhs.typedat == rhs.typedat) && (lhs.parport == rhs.parport) && (lhs.per == rhs.per) && (lhs.adr == rhs.adr)
        && (lhs.typedata == rhs.typedata) && (lhs.func == rhs.func) && (lhs.reg == rhs.reg) && (lhs.cnt == rhs.cnt);
}

bool inline operator!=(const Item &lhs, const Item &rhs)
{
    return !(lhs == rhs);
}

}
