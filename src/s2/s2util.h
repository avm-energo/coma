#pragma once

#include "dataitem.h"

#include <gen/datatypes.h>
#include <gen/error.h>

constexpr int MAXSIZE = 200000;

namespace S2
{
class ConfigStorage;
class Configuration;
class DataFactory;
}

class S2Util
{
public:
    /// \brief Конуструктор класса.
    S2Util() = default;

    QByteArray convert(const quint32 id, const S2::DataItem &item) const;
    QByteArray convert(const S2::Configuration &config, quint32 fileType) const;
    Error::Msg convert(const QByteArray &rawFile, const S2::DataFactory &factory, //
        std::map<quint32, S2::DataItem> &result) const;

    // S2: Сборщик в память:
    // 0 - успешно, иначе код ошибки S2: получение размера:
    static void StoreDataMem(QByteArray &mem, const std::vector<S2::FileStruct> &dr, int fname);
    static void StoreDataMem(QByteArray &mem, const std::vector<S2::DataRec> &dr, int fname);
    // restore IDs and contents in ConfParameters list
    static bool RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist);

    [[deprecated]] static bool RestoreData(QByteArray bain, QList<S2::DataItem> &outlist);
    [[deprecated]] static void tester(const S2::S2ConfigType &buffer);
    [[deprecated]] static quint16 GetIdByName(const QString &name);

    static quint64 GetFileSize(const QByteArray &data);

    static S2::S2BFile emulateS2B(const S2::FileStruct &journal, //
        quint16 fname, quint16 typeB, quint16 typeM);
    static S2::S2BFile parseS2B(const QByteArray &file);

private:
    static quint32 getTime32();
};
