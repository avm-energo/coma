#ifndef MODULE_KXX_H
#define MODULE_KXX_H
#include "../gen/pch.h"
#include "../interfaces/modbusprivate.h"

namespace Bci
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
    CommandsMBS::BaudRate baud : 4;
    // Биты данных
    quint8 bits : 1;
    // Четность
    CommandsMBS::Parity parity : 2;
    // Стоп биты
    CommandsMBS::StopBits stop : 1;
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

struct TypeR
{
    // Функция
    CommandsMBS::Commands reg : 4;
    // Тип данных
    CommandsMBS::TypeId dat : 4;
    friend bool operator==(const TypeR &lhs, const TypeR &rhs);
    friend bool operator!=(const TypeR &lhs, const TypeR &rhs);
};
bool inline operator==(const TypeR &lhs, const TypeR &rhs)
{
    return (lhs.reg == rhs.reg) && (lhs.dat == rhs.dat);
}
bool inline operator!=(const TypeR &lhs, const TypeR &rhs)
{
    return !(lhs == rhs);
}

enum SensorType : quint8
{
    SEN_None,
    SEN_Universal
};

#pragma pack(push, 1)
struct ModbusItem
{
    // Тип датчика
    SensorType typedat;
    // Параметры порта
    INICOM parport;
    // Период опроса
    quint8 per;
    // Адрес
    quint8 adr;
    TypeR type;
    // Начальный адрес регистра
    quint16 reg;
    // Количество регистров
    quint8 cnt;
    friend bool operator==(const ModbusItem &lhs, const ModbusItem &rhs);
    friend bool operator!=(const ModbusItem &lhs, const ModbusItem &rhs);
};
#pragma pack(pop)
bool inline operator==(const ModbusItem &lhs, const ModbusItem &rhs)
{
    return (lhs.typedat == rhs.typedat) && (lhs.parport == rhs.parport) && (lhs.per == rhs.per) && (lhs.adr == rhs.adr)
        && (lhs.type == rhs.type) && (lhs.reg == rhs.reg) && (lhs.cnt == rhs.cnt);
}
bool inline operator!=(const ModbusItem &lhs, const ModbusItem &rhs)
{
    return !(lhs == rhs);
}

}
#endif // MODULE_KXX_H
