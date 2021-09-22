#pragma once
#include "../gen/datatypes.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"

#include <QByteArray>
#include <QVariant>

template <typename T> class FileManager
{
protected:
    using OscHeader = S2DataTypes::OscHeader;
    using FileStruct = DataTypes::FileStruct;
    using SwitchJourRecord = S2DataTypes::SwitchJourRecord;

public:
    using Record = T;
    FileManager() = default;

    QString generateFilename(quint32 id, quint64 timestamp) const;

    void virtual loadFromFile(const QString &filename) = 0;
    void clearBuffer()
    {
        files.clear();
    }

protected:
    const static inline auto isOscHeader = [](const DataTypes::S2Record &record) { return (record.ID == MT_HEAD_ID); };

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
