#pragma once
#include "filemanager.h"

class SwjManager : public FileManager
{
public:
    SwjManager(const QByteArray &ba);
    QVariant load(const DataTypes::FileStruct &fs) override;
};
