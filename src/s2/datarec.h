#ifndef DATAREC_H
#define DATAREC_H

#include <QVector>

namespace S2DataTypes
{
struct DataRecHeader
{
    quint32 id;      // ID
    quint32 numByte; // количество байт в TypeTheData
};

struct DataRec
{
    DataRecHeader header;
    void *thedata;
};

typedef QVector<DataRec> S2ConfigType;

}

#endif // DATAREC_H
