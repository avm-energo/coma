#pragma once
#include "../gen/datatypes.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"
#include "trendviewmodel.h"

#include <QAbstractItemModel>
#include <QByteArray>
#include <QVariant>
#include <memory>
class QAbstractItemModel;

struct SwjModel
{
    std::unique_ptr<QAbstractItemModel> commonModel;
    std::unique_ptr<QAbstractItemModel> detailModel;
};

namespace File
{
using Variant = std::variant<S2DataTypes::OscHeader, std::unique_ptr<TrendViewModel>, SwjModel>;
using Vector = std::vector<Variant>;
}

template <typename T> class FileManager
{
protected:
    using OscHeader = S2DataTypes::OscHeader;
    using FileStruct = DataTypes::FileStruct;
    using Record = T;

public:
    FileManager() = default;

    QString generateFilename(quint32 id, quint64 timestamp) const;

    File::Vector virtual loadFromFile(const QString &filename) = 0;
    bool virtual loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) = 0;
    void clearBuffer()
    {
        files.clear();
    }

    const static inline auto isOscHeader = [](const DataTypes::S2Record &record) { return (record.ID == MT_HEAD_ID); };

protected:
    DataTypes::S2FilePack files;
};

template <typename T> QString FileManager<T>::generateFilename(quint32 id, quint64 timestamp) const
{
    // составляем имя файла осциллограммы
    QString filename = TimeFunc::UnixTime64ToString(timestamp);
    filename.replace("/", "-");
    filename.replace(":", "_");
    filename.insert(0, "_");
    filename.insert(0, QString::number(id));
    return filename;
}
