#pragma once
#include "../gen/datatypes.h"

#include <QByteArray>
#include <QVariant>

class FileManager
{
public:
    FileManager() = default;
    FileManager(const QByteArray &ba);
    QVariant load(const QByteArray &ba);
    QVariant virtual load(const DataTypes::FileStruct &fs) = 0;

protected:
    QByteArray buffer;
};
