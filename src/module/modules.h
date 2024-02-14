#pragma once
#include <QMap>
#include <gen/pch.h>

/**
 *  Перечисление для хранения списка приборов
 */

namespace Modules
{

struct StartupInfoBlock
{
    quint32 MTypeB;
    quint32 MTypeM;
    quint32 HwverB;
    quint32 Fwver;
    quint32 Rst;
    quint32 RstCount;
    quint32 UIDLow;
    quint32 UIDMid;
    quint32 UIDHigh;
    quint32 SerialNumB;
    quint32 SerialNumM;
    quint32 HwverM;
    quint32 SerialNum;
    quint32 Cfcrc;
    quint32 Hth;

    bool isOutdated(const quint32 &configVersion) const
    {
        return Fwver < configVersion;
    }

    quint16 type() const
    {
        return quint16((MTypeB << 8) + MTypeM);
    }
};

struct StartupInfoBlockExt0
{
    quint32 NamePO;
    quint32 VerLoader;
    quint32 POVDT;
    quint32 POVCRCB;
    quint32 POVCRCM;
};

}

namespace AVM
{

const QStringList Health {
    "ADCI", //
    "FLS",  //
    "TUP",  //
    "ADCB", //
    "1PPS", //
    "ADCM", //
    "REGB", //
    "RCN",  //
    "HWIB", //
    "HWIM", //
    "REGM", //
    "BAT",  //
    "NTP",  //
    "FLS2", //
    "FRM"   //
};

const QStringList HthToolTip {
    "Проблемы со встроенным АЦП ",                                                      //
    "Не работает внешняя flash-память",                                                 //
    "Перегрев",                                                                         //
    "Проблемы с АЦП (нет связи) (базовая)",                                             //
    "Нет сигнала 1PPS с антенны",                                                       //
    "Проблемы с АЦП (нет связи) (мезонин)",                                             //
    "Ошибка регулировочных коэффициентов (базовая)",                                    //
    "Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию", //
    "Некорректная Hardware информация (базовая)",                                       //
    "Некорректная Hardware информация (мезонин)",                                       //
    "Ошибка регулировочных коэффициентов (мезонин)",                                    //
    "Напряжение батареи низко (< 2,5 В)",                                               //
    "Нет связи с NTP-сервером",                                                         //
    "Не работает внешняя flash-память (мезонин)",                                       //
    "Не работает внешняя fram",                                                         //
    "Проблемы с RTC "                                                                   //
};

}

// Oscillogram / SwitchJournal Part

/// ID заголовка осциллограммы
constexpr int MT_HEAD_ID = 9000;
constexpr int MT_SPEC_ID = 9100;

/*constexpr int ID_OSC_CH0 = 10040; // ID осциллограммы канала 0
constexpr int ID_OSC_CH1 = 10041; // ID осциллограммы канала 1
constexpr int ID_OSC_CH2 = 10042; // ID осциллограммы канала 2
constexpr int ID_OSC_CH3 = 10043; // ID осциллограммы канала 3
constexpr int ID_OSC_CH4 = 10044; // ID осциллограммы канала 4
constexpr int ID_OSC_CH5 = 10045; // ID осциллограммы канала 5
constexpr int ID_OSC_CH6 = 10046; // ID осциллограммы канала 6
constexpr int ID_OSC_CH7 = 10047; // ID осциллограммы канала 7

constexpr int ID_SPECTR_CH0 = 10050; // ID спектрограммы канала 0
constexpr int ID_SPECTR_CH1 = 10051; // ID спектрограммы канала 1
constexpr int ID_SPECTR_CH2 = 10052; // ID спектрограммы канала 2
constexpr int ID_SPECTR_CH3 = 10053; // ID спектрограммы канала 3
constexpr int ID_SPECTR_CH4 = 10054; // ID спектрограммы канала 4
constexpr int ID_SPECTR_CH5 = 10055; // ID спектрограммы канала 5
constexpr int ID_SPECTR_CH6 = 10056; // ID спектрограммы канала 6
constexpr int ID_SPECTR_CH7 = 10057; // ID спектрограммы канала 7 */

namespace AVTUK_85
{
constexpr quint32 SWJ_ID = 10031; ///< ID журнала переключений с осциллограммой модуля 8085/8585
constexpr quint32 OSC_ID = 10030; ///< ID осциллограммы по модулю 8085/8585
}

namespace AVTUK_8X
{
constexpr quint32 OSC_ID = 10020; ///< ID осциллограммы по модулю 808x
}

namespace AVTUK_KDV
{
constexpr quint32 OSC_ID = 10040;   ///< ID осциллограммы модуля КДВ
constexpr quint32 OSCV_ID = 10041;  ///< ID осциллограммы модуля КДВ
constexpr quint32 SPEC_ID = 10050;  ///< ID спектрограммы модуля КДВ
constexpr quint32 SPECV_ID = 10051; ///< ID осциллограммы модуля КДВ
}

namespace AVTUK_21
{
constexpr quint32 OSC_ID_MIN = 10001; ///< Первый ID осциллограммы по модулю 21
constexpr quint32 OSC_ID_MAX = 10016; ///< Последний ID осциллограммы по модулю 21
}

constexpr quint32 MAXOSCBUFSIZE = 262144; ///< Максимальный размер буфера для осциллограмм
