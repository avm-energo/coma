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
    static void StoreDataMem(QByteArray &mem, std::vector<DataTypes::FileStruct> &dr, int fname);
    static void StoreDataMem(QByteArray &mem, const QVector<S2DataTypes::DataRec> &dr, int fname);
    // restore IDs and contents in ConfParameters list
    static bool RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist);
    static bool RestoreData(QByteArray bain, QList<DataTypes::DataRecV> &outlist);
    static S2DataTypes::S2ConfigType ParseHexToS2(QByteArray &ba);
    static void tester(S2DataTypes::S2ConfigType &buffer);
    static quint16 GetIdByName(QString name);
    static quint64 GetFileSize(const QByteArray &data);

    static QMap<QString, quint16> NameIdMap;

private:
    // S2: Поиск элемента в массиве описаний
    // static bool RestoreDataMem(void *mem, quint32 memsize, const QVector<S2DataTypes::DataRec> &dr);
    // static void findElemAndWriteIt(QVector<S2DataTypes::DataRec> *s2config, const DataTypes::S2Record &cfp);
    // static bool findElemAndWriteIt(S2DataTypes::DataRec *record, const DataTypes::S2Record &cfp);
    // static const S2DataTypes::DataRec *FindElem(const QVector<S2DataTypes::DataRec> *, quint32);
    static quint32 getTime32();
};
