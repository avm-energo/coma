#ifndef PARSEID9050_H
#define PARSEID9050_H

#define ID_OSC_CH0         10040 // ID осциллограммы канала 0
#define ID_OSC_CH1         10041 // ID осциллограммы канала 1
#define ID_OSC_CH2         10042 // ID осциллограммы канала 2
#define ID_OSC_CH3         10043 // ID осциллограммы канала 3
#define ID_OSC_CH4         10044 // ID осциллограммы канала 4
#define ID_OSC_CH5         10045 // ID осциллограммы канала 5
#define ID_OSC_CH6         10046 // ID осциллограммы канала 6
#define ID_OSC_CH7         10047 // ID осциллограммы канала 7

#define ID_SPECTR_CH0      10050 // ID спектрограммы канала 0
#define ID_SPECTR_CH1      10051 // ID спектрограммы канала 1
#define ID_SPECTR_CH2      10052 // ID спектрограммы канала 2
#define ID_SPECTR_CH3      10053 // ID спектрограммы канала 3
#define ID_SPECTR_CH4      10054 // ID спектрограммы канала 4
#define ID_SPECTR_CH5      10055 // ID спектрограммы канала 5
#define ID_SPECTR_CH6      10056 // ID спектрограммы канала 6
#define ID_SPECTR_CH7      10057 // ID спектрограммы канала 7

#include <QByteArray>
#include <QVector>
#include <QObject>
#include "parsemodule.h"
#include "../models/trendviewmodel.h"
#include "../dialogs/trendviewdialog.h"

class ParseID9050 : public ParseModule
{
public:
    ParseID9050(QByteArray &BA);

    QMultiMap<QString, int> Channel;

    struct OscHeader_Data
    {
        quint64 unixtime;     // время первой точки в Unix-формате
        float step;           // шаг по времени в мс
        quint32 len;          // длина осциллограммы в количестве точек по времени
    };

    struct SpectHeader_Data
    {
        quint64 t1;           // Время начала события
        quint64 t2;           // Время максимума события
        quint64 t3;           // Время конца события
        float Umax;           // Значение максимума события
        float Fmax;           // Максимальная частотная составляющая
        float step;           // шаг по частоте
        float len;          // длина спектрограммы в количестве точек по частоте
    };

    struct Point87
    {
        float An;
    };

    int Parse(int &count);
    void Save(quint32 &len);
    int ParseOsc(quint32 id, OscHeader_Data &OHD, const QString &fn, QStringList tmpav, TrendViewDialog *dlg, int &count);
    int ParseSpectr(quint32 id, SpectHeader_Data &SHD, const QString &fn, QStringList tmpav, TrendViewDialog *dlg, int &count);
};

#endif // PARSEID9050_H
