#pragma once

#include "dataitem.h"

#include <QVector>
#include <gen/datatypes.h>

constexpr int MAXSIZE = 200000;

namespace S2
{
class ConfigStorage;
class Configuration;
}

class S2Util
{
private:
    const S2::ConfigStorage &s2confStorage;

public:
    S2Util(const S2::ConfigStorage &s2confStorage);

    quint32 getIdByName(const QString &name) const;
    QByteArray toByteArray(const quint32 id, const S2::DataItem &item) const;
    QByteArray toByteArray(const S2::Configuration &config, quint32 fileType) const;

    // S2: Сборщик в память:
    // 0 - успешно, иначе код ошибки S2: получение размера:
    static void StoreDataMem(QByteArray &mem, const QList<S2::DataItem> &dr, int fname);
    static void StoreDataMem(QByteArray &mem, const std::vector<S2::FileStruct> &dr, int fname);
    static void StoreDataMem(QByteArray &mem, const std::vector<S2::DataRec> &dr, int fname);
    // restore IDs and contents in ConfParameters list
    static bool RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist);
    static bool RestoreData(QByteArray bain, QList<S2::DataItem> &outlist);

    static void tester(const S2::S2ConfigType &buffer);

    static quint16 GetIdByName(const QString &name);

    static quint64 GetFileSize(const QByteArray &data);

    static S2::S2BFile emulateS2B(const S2::FileStruct &journal, //
        quint16 fname, quint16 typeB, quint16 typeM);
    static S2::S2BFile parseS2B(const QByteArray &file);

private:
    static quint32 getTime32();
};
