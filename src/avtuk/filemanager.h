#pragma once
#include "../gen/datatypes.h"
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
    FileManager(const QByteArray &ba) : buffer(ba)
    {
    }
    QString generateFilename(quint32 id, quint64 timestamp);

protected:
    QByteArray buffer;
};

template <typename T> QString FileManager<T>::generateFilename(quint32 id, quint64 timestamp)
{
    // составляем имя файла осциллограммы
    QString filename = TimeFunc::UnixTime64ToString(timestamp);
    filename.replace("/", "-");
    filename.replace(":", "_");
    filename.insert(0, "_");
    filename.insert(0, QString::number(id));
    return filename;
}
