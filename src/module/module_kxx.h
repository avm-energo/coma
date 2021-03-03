#ifndef MODULE_KXX_H
#define MODULE_KXX_H
#include "../gen/pch.h"

#include <QDebug>
#include <array>
#include <vector>
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

constexpr int DEF_RTERM = 100;
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

struct INICOM
{
    quint8 baud : 4;
    quint8 bits : 1;
    quint8 parity : 2;
    quint8 stop : 1;
};

struct TypeR
{
    quint8 reg : 4;
    quint8 dat : 4;
};

#pragma pack(push, 1)
struct ABMAST
{
    quint8 typedat;
    INICOM parport;
    quint8 per;
    quint8 adr;
    TypeR type;
    quint16 reg;
    quint8 cnt;
};
#pragma pack(pop)

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
        Parity = DEF_PARITY;
        Stopbit = DEF_STOPBIT;
        adrMB = DEF_ADRMB;
    }
    quint8 IP[4];
    quint8 Mask[4];
    quint8 GateWay[4];
    quint16 Port[4];
    quint8 SNTP[4];
    quint32 Baud;
    quint8 Parity;
    quint8 Stopbit;
    quint8 adrMB;
};
}
#endif // MODULE_KXX_H
