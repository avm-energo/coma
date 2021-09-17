#ifndef PARSEID9050_H
#define PARSEID9050_H

#include "../gen/datatypes.h"
#include "parsemodule.h"

class ParseID9050 : public ParseModule
{
public:
    ParseID9050(const QByteArray &BA);

    QMultiMap<QString, int> Channel;

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

    bool ParseOsc(quint32 id, S2DataTypes::OscHeader &OHD, const QString &fn, QStringList tmpav, TrendViewDialog *dlg,
        int &count);
    bool ParseSpectr(
        quint32 id, SpectHeader_Data &SHD, const QString &fn, QStringList tmpav, TrendViewDialog *dlg, int &count);
    bool Parse(quint32 id, S2DataTypes::OscHeader &header, TrendViewModel *model) override
    {
        return true;
    }
};

#endif // PARSEID9050_H
