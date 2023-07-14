#include "s2datamanager.h"

namespace S2
{

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , storage(S2ConfigStorage::GetInstance()) //
    , defaultConfig(storage)
    , workingConfig(storage)
{
}

S2ConfigStorage &DataManager::getStorage() noexcept
{
    return storage;
}

const S2ConfigStorage &DataManager::getStorage() const noexcept
{
    return storage;
}

} // namespace S2
