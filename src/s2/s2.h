#pragma once

#include "datarecv.h"
#include "s2datatypes.h"

#include <QVector>
#include <gen/datatypes.h>

constexpr int MAXSIZE = 200000;

class BaseInterface;
class S2
{
    // friend BaseInterface;

public:
    S2();
    // S2: Сборщик в память:
    // 0 - успешно, иначе код ошибки S2: получение размера:
    static void StoreDataMem(QByteArray &mem, const QList<DataTypes::DataRecV> &dr, int fname);
    static void StoreDataMem(QByteArray &mem, const std::vector<DataTypes::FileStruct> &dr, int fname);
    static void StoreDataMem(QByteArray &mem, const QVector<S2DataTypes::DataRec> &dr, int fname);
    // restore IDs and contents in ConfParameters list
    static bool RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist);
    static bool RestoreData(QByteArray bain, QList<DataTypes::DataRecV> &outlist);
    static S2DataTypes::S2ConfigType ParseHexToS2(QByteArray &ba);
    static void tester(S2DataTypes::S2ConfigType &buffer);
    static quint16 GetIdByName(QString name);

    static QMap<QString, quint16> NameIdMap;

private:
    static quint32 getTime32();
};
