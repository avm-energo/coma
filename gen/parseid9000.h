#ifndef PARSEID9000_H
#define PARSEID9000_H

#define MT_ID21         10001 // первый ID осциллограммы по модулю 21
#define MT_ID21E        10016 // последний ID осциллограммы по модулю 21
#define MT_ID80         10020 // ID осциллограммы по модулю 808x
#define MT_ID85         10030 // ID осциллограммы по модулю 8085

#include <QByteArray>
#include <QVector>
#include <QObject>
#include "parsemodule.h"
#include "../models/trendviewmodel.h"
#include "../dialogs/trendviewdialog.h"

class ParseID9000 : public ParseModule
{
public:
    ParseID9000(QByteArray &BA);

    struct OscHeader_Data
    {
        quint64 unixtime; // время первой точки в Unix-формате
        float step; // шаг по времени в мс
        quint32 len; // длина осциллограммы в количестве точек по времени
    };

    struct Point85
    {
        float An[9];    // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
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


    int Parse(int &count);

    int ParseID21(quint32 id, OscHeader_Data &OHD, const QString &fn, QStringList tmpav, TrendViewDialog *dlg, int &count);
    int ParseID8x(quint32 id, OscHeader_Data &OHD, const QString &fn, QStringList tmpav, QStringList tmpdv, TrendViewDialog *dlg, int &count);
    int ParseID85(quint32 id, OscHeader_Data &OHD, const QString &fn, QStringList tmpav, QStringList tmpdv, TrendViewDialog *dlg, int &count);
};

#endif // PARSEID9000_H
