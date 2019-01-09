#ifndef CONFIG3X_H
#define CONFIG3X_H

#define D_MIN_TYPE    0x30 // минимальный номер дискретной платы
#define D_MAX_TYPE    0x3F // максимальный номер дискретной платы

#define D_INTYPE_NORM   0x00
#define D_INTYPE_NONE   0x01
#define D_INTYPE_INV    0x02

#define D_OUTTYPE_NORM  0x00
#define D_OUTTYPE_NONE  0x01
#define D_OUTTYPE_PFM   0x02
#define D_OUTTYPE_PWM   0x04

#define SIZEMAX     128 // максимальный размер массивов конфигурационных параметров

#include <QVector>
#include <QMap>
#include "config.h"
#include "../gen/modulebsi.h"

class Config3x
{
public:
    struct ModuleDesc
    {
        quint32 DInSize, DOutSize; // размер векторов в соответствующих структурах
        quint32 DInStart, DOutStart; // начальный номер ID диапазонов аналоговых входов и выходов
    };

    static QMap<int, ModuleDesc> ModTypeMap()
    {
        QMap<int, ModuleDesc> map;
        ModuleDesc MTB31MD = {11, 0, 201, 0}; // InputsNum, OutputsNum, DInStart, DOutStart
        map.insert(MTB_31, MTB31MD);
        ModuleDesc MTM31MD = {11, 0, 231, 0};
        map.insert(MTM_31, MTM31MD);
        ModuleDesc MTB35MD = {0, 4, 0, 401};
        map.insert(MTB_35, MTB35MD);
        ModuleDesc MTM35MD = {0, 4, 0, 421};
        map.insert(MTM_35, MTM35MD);
        return map;
    }

    typedef struct
    {
        qint8 in_type[16];     // Тип входа (бит 0 - не исп., бит 1 - инверсия)
        quint32 dly_time[16];  // Время задержки для контроля дребезга
        quint16 pair[8];      // Номера каналов в парах
        quint16 SignalType;   // Тип сигнала
    } Bci_DIn;

    typedef struct
    {
        quint8 wd_type; // Тип Watchdog (0 - нормальный)
        quint8 wd_nout; // Номер выхода Watchdog
        quint16 wd_tm;  // Время watchdog
        float pulse_short; // ширина короткого импульса в мс
        float pulse_long; // ширина длинного импульса в мс
        qint8 out_type[16]; // тип выхода (бит 0 - не исп., бит 1 - ЧИМ, бит 2 - ШИМ)
    } Bci_DOut;

/*    static void SetDInSize(Bci_DIn &Bcii, int mtype);
    static void SetDOutSize(Bci_DOut &Bcio, int mtype);
    static void SetDOutRelaySize(Bci_DOut_Relay &Bcio, int mtype); */

private:
};
#endif // CONFIG3X_H
