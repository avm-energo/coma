#pragma once

#include "datarecv.h"
#include "s2datatypes.h"

#include <QVector>
#include <gen/datatypes.h>
#include <gen/error.h>

constexpr int MAXSIZE = 200000;

class S2
{
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
    static void tester(S2DataTypes::S2ConfigType &buffer);
    static quint16 GetIdByName(QString name);
    static quint64 GetFileSize(const QByteArray &data);
    static S2DataTypes::S2BFile emulateS2B(const QByteArray &data, quint16 fname, quint16 typeB, quint16 typeM);
    static Error::Msg parseS2B(const QByteArray &file, S2DataTypes::S2BFile &result);

    static QMap<QString, quint16> NameIdMap;

private:
    static quint32 getTime32();
};
