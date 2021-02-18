#ifndef PARSEID10031_H
#define PARSEID10031_H

#include "parsemodule.h"
#include "trendviewmodel.h"

#include <QByteArray>
#include <QObject>
#include <QVector>

class ParseID10031 : public ParseModule
{
public:
    ParseID10031(QByteArray &BA);

    //    static quint32 len2;

    struct Point85
    {
        float An[9]; // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
        quint32 Dis;
    };

    bool Parse(int &count);
    //    void Save(quint32 *len);

    bool ParseID85(S2DataTypes::OscHeader &OHD, const QString &fn, int &count);

private:
    //     quint32 Len;
};

#endif // PARSEID10031_H
