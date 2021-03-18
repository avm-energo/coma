#ifndef MODULE_KXX_H
#define MODULE_KXX_H
#include "../gen/pch.h"
#include "../interfaces/modbusprivate.h"

#include <array>
namespace AVM_KXX
{

}

namespace Bci
{
// constexpr int DEF_IP 172, 16, 29, 12
// constexpr int DEF_MASK 255, 255, 252, 0
// constexpr int DEF_GW 172, 16, 29, 1
// constexpr int DEF_PORT 2404, 2405, 502, 502
// constexpr int DEF_SNTP 172, 16, 31, 220
constexpr int DEF_BAUD = 9600;
constexpr int DEF_PARITY = 0;
constexpr int DEF_STOPBIT = 0;
constexpr int DEF_ADRMB = 0;
// constexpr int DEF_ISNTP 0
// constexpr int DEF_ISPPS 0

constexpr float DEF_RTERM = 100.0f;
constexpr float DEF_W100 = 1.385f;

// constexpr int DEF_TRELE_PRED = 0;
// constexpr int DEF_TRELE_ALARM = 0;

constexpr int DEF_MBMASTER = 0;

struct TempConfStruct
{
    TempConfStruct()
    {
        RTerm = DEF_RTERM;
        W100 = DEF_W100;
    }
    float RTerm;
    float W100;
};
struct StructTrele
{
    quint32 Trele_pred;
    quint32 Trele_alarm;
};

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
struct ABMAST
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
    friend bool operator==(const ABMAST &lhs, const ABMAST &rhs);
    friend bool operator!=(const ABMAST &lhs, const ABMAST &rhs);
};
#pragma pack(pop)
bool inline operator==(const ABMAST &lhs, const ABMAST &rhs)
{
    return (lhs.typedat == rhs.typedat) && (lhs.parport == rhs.parport) && (lhs.per == rhs.per) && (lhs.adr == rhs.adr)
        && (lhs.type == rhs.type) && (lhs.reg == rhs.reg) && (lhs.cnt == rhs.cnt);
}
bool inline operator!=(const ABMAST &lhs, const ABMAST &rhs)
{
    return !(lhs == rhs);
}

struct StructModBus
{
    quint8 MBMaster;
    std::array<quint8, 8> MBMab1;
    std::array<quint8, 8> MBMab2;
    std::array<quint8, 8> MBMab3;
    std::array<quint8, 8> MBMab4;
};
struct Com
{
    Com()
    {
        IP[0] = 172;
        IP[1] = 16;
        IP[2] = 29;
        IP[3] = 12;

        Mask[0] = 255;
        Mask[1] = 255;
        Mask[2] = 252;
        Mask[3] = 0;

        GateWay[0] = 172;
        GateWay[1] = 16;
        GateWay[2] = 29;
        GateWay[3] = 1;

        Port[0] = 2404;
        Port[1] = 2405;
        Port[2] = 502;
        Port[3] = 502;

        SNTP[0] = 172;
        SNTP[1] = 16;
        SNTP[2] = 31;
        SNTP[3] = 220;

        Baud = DEF_BAUD;
        Parity = CommandsMBS::Parity::NoParity;
        Stopbit = CommandsMBS::StopBits::OneStop;
        adrMB = DEF_ADRMB;
    }
    std::array<quint8, 4> IP;
    std::array<quint8, 4> Mask;
    std::array<quint8, 4> GateWay;
    std::array<quint16, 4> Port;
    std::array<quint8, 4> SNTP;
    quint32 Baud;
    CommandsMBS::Parity Parity;
    CommandsMBS::StopBits Stopbit;
    quint8 adrMB;
};
}
#endif // MODULE_KXX_H
