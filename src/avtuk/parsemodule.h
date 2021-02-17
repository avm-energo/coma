#ifndef PARSEMODULE_H
#define PARSEMODULE_H

#include "trendviewdialog.h"
#include "trendviewmodel.h"

#include <QByteArray>
#include <QObject>
#include <QVector>

class ParseModule
{
public:
    ParseModule(QByteArray &BA);

    QByteArray BArray;
    TrendViewModel *TModel;
    TrendViewDialog *dlg;
    quint32 id;

    bool PosPlusPlus(void *dst, int &count, int size);
    virtual bool Parse(int &count) = 0;
    //    virtual void Save(quint32 *len) = 0;
};

#endif // PARSEMODULE_H
