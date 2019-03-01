#ifndef PARSEMODULE_H
#define PARSEMODULE_H

#include <QByteArray>
#include <QVector>
#include <QObject>
#include "../models/trendviewmodel.h"
#include "../dialogs/trendviewdialog.h"

class ParseModule
{
public:
    ParseModule(QByteArray &BA);

    QByteArray BArray;
    TrendViewModel *TModel;
    TrendViewDialog *dlg;
    quint32 id;

    struct DataRecHeader
    {
        quint32 id;        // ID
        quint32 numbytes;  // количество байт
    };

    int PosPlusPlus(void *dst, int &count, int size);
    virtual int Parse(int &count) = 0;
//    virtual void Save(quint32 *len) = 0;



};

#endif // PARSEMODULE_H
