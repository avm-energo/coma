#include "swjmanager.h"

SwjManager::SwjManager(const QByteArray &ba) : FileManager(ba)
{
}

std::unique_ptr<TrendViewModel> SwjManager::load(const S2DataTypes::OscHeader &header, const DataTypes::FileStruct &fs)
{
    return {};
}
