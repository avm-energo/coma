#ifndef PARSEID9050_H
#define PARSEID9050_H

#include "parsemodule.h"
#include "trendviewdialog.h"
#include "trendviewmodel.h"

#include <QByteArray>
#include <QObject>
#include <QVector>

constexpr int ID_OSC_CH0 = 10040; // ID осциллограммы канала 0
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
constexpr int ID_SPECTR_CH7 = 10057; // ID спектрограммы канала 7

class ParseID9050 : public ParseModule
{
public:
    ParseID9050(QByteArray &BA);

    QMultiMap<QString, int> Channel;

    struct OscHeader_Data
    {
        quint64 unixtime; // время первой точки в Unix-формате
        float step;       // шаг по времени в мс
        quint32 len; // длина осциллограммы в количестве точек по времени
    };

    struct SpectHeader_Data
    {
        quint64 t1; // Время начала события
        quint64 t2; // Время максимума события
        quint64 t3; // Время конца события
        float Umax; // Значение максимума события
        float Fmax; // Максимальная частотная составляющая
        float step; // шаг по частоте
        float len;  // длина спектрограммы в количестве точек по частоте
    };

    struct Point87
    {
        float An;
    };

    bool Parse(int &count);
    //    void Save(quint32 *len);
    bool ParseOsc(
        quint32 id, OscHeader_Data &OHD, const QString &fn, QStringList tmpav, TrendViewDialog *dlg, int &count);
    bool ParseSpectr(
        quint32 id, SpectHeader_Data &SHD, const QString &fn, QStringList tmpav, TrendViewDialog *dlg, int &count);
};

#endif // PARSEID9050_H
