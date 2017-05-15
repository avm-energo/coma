#ifndef CONFIG3X_H
#define CONFIG3X_H

#define BCI_DIN_NUM     3 // 3 параметра в конфигурации цифровых входов
#define BCI_DOUT_NUM    3 // 3 параметра в конфигурации цифровых выходов

#define DIN_MIN_TYPE    0x30 // минимальный номер дискретной платы
#define DIN_MAX_TYPE    0x3F // максимальный номер дискретной платы

#include <QVector>
#include <QMap>
//#include "config.h"

class Config3x
{
public:
    enum DInTypes
    {
        DIT_NONE,
        DIT_INV,
        DIT_RES
    };

    enum DOutTypes
    {
        DOT_NONE,
        DOT_PFM, // pulse-frequency modulation
        DOT_PWM // pulse-width modulation
    };

    struct ModuleDesc
    {
        quint32 DInSize, DOutSize; // размер векторов в соответствующих структурах
        quint32 DInStart, DOutStart; // начальный номер ID диапазонов аналоговых входов и выходов
    };

    static QMap<int, ModuleDesc> ModTypeMap()
    {
        QMap<int, ModuleDesc> map;
        ModuleDesc MTB31MD = {BCI_DIN_NUM, 0, 201, 0};
        map.insert(MTB_31, MTB31MD);
        ModuleDesc MTM31MD = {BCI_DIN_NUM, 0, 231, 0};
        map.insert(MTM_31, MTM31MD);
        ModuleDesc MTB35MD = {0, BCI_DOUT_NUM, 0, 401};
        map.insert(MTB_35, MTB35MD);
        ModuleDesc MTM35MD = {0, BCI_DOUT_NUM, 0, 421};
        map.insert(MTM_35, MTM35MD);
        return map;
    }

    typedef struct
    {
        QVector<qint8> in_type;     // Тип входа (бит 0 - не исп., бит 1 - инверсия)
        QVector<quint32> dly_time;  // Время задержки для контроля дребезга
        QVector<quint16> pair;      // Номера каналов в парах
    } Bci_DIn;

    typedef struct
    {
        quint8 wd_type; // Тип Watchdog (0 - нормальный)
        quint8 wd_nout; // Номер выхода Watchdog
        quint16 wd_tm;  // Время watchdog
    } Bci_DOut;

    static void SetDInSize(Bci_DIn &Bcii, int mtype);
    static void SetDOutSize(Bci_DOut &Bcio, int mtype);

private:
};
#endif // CONFIG3X_H
