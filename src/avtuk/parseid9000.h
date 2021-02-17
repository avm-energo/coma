#ifndef PARSEID9000_H
#define PARSEID9000_H

#include "parsemodule.h"
#include "trendviewdialog.h"
#include "trendviewmodel.h"

#include <QByteArray>
#include <QObject>
#include <QVector>

class ParseID9000 : public ParseModule
{
public:
    ParseID9000(QByteArray &BA);

    struct OscHeader_Data
    {
        quint64 unixtime; // время первой точки в Unix-формате
        float step;       // шаг по времени в мс
        quint32 len; // длина осциллограммы в количестве точек по времени
    };

    struct Point85
    {
        float An[9]; // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
        quint32 Dis;
    };
    // Dis:
    // 31-29:	nOx – НО блок-контакты полюсов А, В, С (нефильтр.)
    // 28-26:	nCx – НЗ блок-контакты полюсов А, В, С (нефильтр.)
    // 25-23:	nFx – оперативное управление отключением полюсов А, В, С (нефильтр.)
    // 22-20:	nNx – оперативное управление включением полюсов А, В, С (нефильтр.)
    // 19-17:	CFx – выходные команды на отключение полюсов А, В, С
    // 16-14:	CNx – выходные команды на включение полюсов А, В, С
    // 13:      CSC – контроль цепи соленоида отключения
    // 12:      CSO – контроль цепи соленоида включения
    // 11-9:	BKOx – НО блок-контакты полюсов А, В, С
    // 8-6:     BKCx – НЗ блок-контакты полюсов А, В, С
    // 5-3:     OCFx – оперативное управление отключением полюсов А, В, С
    // 2-0:     OCNx – оперативное управление включением полюсов А, В, С

    struct Point21
    {
        float An;
    };

    struct Point8x
    {
        float An[6];
    };

    bool Parse(int &count);
    //    void Save(quint32 *len);
    bool ParseID21(quint32 id, OscHeader_Data &OHD, const QString &fn, TrendViewDialog *dlg, int &count);
    bool ParseID8x(quint32 id, OscHeader_Data &OHD, const QString &fn, TrendViewDialog *dlg, int &count);
    bool ParseID85(quint32 id, OscHeader_Data &OHD, const QString &fn, TrendViewDialog *dlg, int &count);
};

#endif // PARSEID9000_H
