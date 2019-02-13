#ifndef PARSEMODULE_H
#define PARSEMODULE_H

#include <QByteArray>
#include <QVector>
#include <QObject>

class ParseModule
{
public:
    ParseModule(QByteArray &BA);

    QByteArray BArray;


    struct DataRecHeader
    {
        quint32 id;        // ID
        quint32 numbytes;  // количество байт
    };

    int PosPlusPlus(void *dst, int &count, int size);
    virtual int Parse(int &count) = 0;



};

#endif // PARSEMODULE_H
