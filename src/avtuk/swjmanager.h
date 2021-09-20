#pragma once
#include "filemanager.h"

class SwjManager : public FileManager
{
public:
    SwjManager(const QByteArray &ba);
    std::unique_ptr<TrendViewModel> load(
        const S2DataTypes::OscHeader &header, const DataTypes::FileStruct &fs) override;
};
