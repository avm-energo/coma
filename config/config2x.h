#ifndef CONFIG2X_H
#define CONFIG2X_H

/*#define INTYPENA    0
#define INTYPEMA    1
#define INTYPEV     2
#define INTYPERES   3 */

#define BCI_AIN_NUM 15 // 15 параметров в конфигурации аналоговых входов
#define BCI_AOUT_NUM    5 // 5 параметров в конфигурации аналоговых выходов

#include <QVector>
#include <QMap>
#include "../config.h"`

class config2x
{
    explicit config2x();

public:
    enum AInTypes
    {
        AIT_NONE,
        AIT_MA,
        AIT_V,
        AIT_RES
    };

    enum AOutTypes
    {
        AOT_NONE,
        AOT_PROP,
        AOT_PFM,
        AOT_PWM
    };

/*    enum AModuleTypes
    {
        MT_21,
        MT_22
    }; */

    struct ModuleDesc
    {
        int AInSize, AOutSize; // размер векторов в соответствующих структурах
        int AInStart, AOutStart; // начальный номер ID диапазонов аналоговых входов и выходов
    };

    static QMap<int, ModuleDesc> ModTypeMap()
    {
        QMap<int, ModuleDesc> map;
        ModuleDesc MTB21MD = {8, 8, 101, 151};
        map.insert(MTB_21, MTB21MD);
        ModuleDesc MTM21MD = {8, 8, 121, 171};
        map.insert(MTM_21, MTM21MD);
        ModuleDesc MTB22MD = {8, 8, 301, 351};
        map.insert(MTB_22, MTB22MD);
        ModuleDesc MTM22MD = {8, 8, 321, 371};
        map.insert(MTM_22, MTM22MD);
        return map;
    }

    typedef struct
    {
        QVector<qint8> in_type;     // Тип входа (0 - не исп., 1 - мА, 2 - В, 3 - резерв)
        QVector<float> in_min;      // Минимальное значение диапазона изменения сигнала
        QVector<float> in_max;      // Максимальное значение диапазона
        QVector<float> in_vmin;     // Минимальное значение сигнала в инженерных единицах
        QVector<float> in_vmax;     // Максимальное значение сигнала в инженерных единицах
        QVector<float> setminmin;   // Значение аварийной уставки по минимальному значению
        QVector<float> setmin;      // Значение предупредительной уставки по минимальному значению
        QVector<float> setmax;      // Значение предупредительной уставки по максимальному значению
        QVector<float> setmaxmax;	// Значение аварийной уставки по максимальному значению
        QVector<quint8> osc;        // Признаки источников запуска осциллограмм
        quint16 oscdly;             // Задержка в мс начала фиксации максимумов
        float oscthr;               // Пороговое значение, при превышении которого начинается запись осциллограмм
        float filterthr;            // Пороговое значение, при превышении которого между замерами фильтр не работает
        float filtern;              // Степень фильтрации (0 - отсутствует)
        float hysteresis;           // Значение гистерезиса на сравнение с уставками (доля от диапазона)
    } Bci_AIn;

    typedef struct
    {
        QVector<qint8> out_type;    // Тип выхода (0 - не исп., 1 - пропорциональный, 2 - ЧИМ, 3 - ШИМ)
        QVector<float> out_min;     // Минимальное значение диапазона изменения сигнала (N-значение ЦАП, F-частота, Q-скважность)
        QVector<float> out_max;     // Максимальное значение диапазона
        QVector<float> out_vmin;    // Минимальное значение сигнала в инженерных единицах (чтобы посылать команду: установить ... ед.)
        QVector<float> out_vmax;    // Максимальное значение сигнала в инженерных единицах
    } Bci_AOut;

    struct Bci
    {
        Bci_Main mainblk;   // Основной блок (см. config.h)
        Bci_AIn inblk;      // Блок с определениями входов
        Bci_AOut outblk;    // Блок с определениями выходов
    };

    Bci Bci_block, Bci_defblock;
    publicclass::DataRec Config[CONF_NUM+BCI_AIN_NUM+BCI_AOUT_NUM+1]; // +1 на "пустой" элемент

    bool Setup(int MType);

private:
    bool SetConfig();
    bool SetAInSize(Bci_AIn &Bcii, int size);
    bool SetAOutSize(Bci_AOut &Bcio, int size);
};
#endif // CONFIG2X_H
