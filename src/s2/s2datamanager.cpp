#include "s2datamanager.h"

#include <QDebug>

namespace S2
{

DataManager::DataManager(QObject *parent) : QObject(parent), storage(S2ConfigStorage::GetInstance())
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

void DataManager::clear() noexcept
{
    data.clear();
    storage.clearDetailData();
}

bool DataManager::isOneBoard() const noexcept
{
    // Если в парсинге использовался один XML-файл, то модуль
    // состоит из одной платы и имеет один диалог для конфигурации.
    return currentParseTarget == BoardConfig::Base;
}

void DataManager::startNewConfig()
{
    constexpr std::array<BoardConfig, 2> boardTypes { BoardConfig::Base, BoardConfig::Mezz };
    for (const auto &boardType : boardTypes)
    {
        auto search = data.find(boardType);
        if (search == data.end())
        {
            data.insert({ boardType, BoardConfiguration { storage, storage } });
            currentParseTarget = boardType;
            break;
        }
    }
}

void DataManager::configDataReceive(const quint16 id, const QString &defVal, const bool visib, const quint16 count)
{
    // Перед вызовом configDataReceive гарантированно будет вызван startNewConfig,
    // поэтому currentParseTarget будет гарантированно инициализирован значением.
    data.at(currentParseTarget).defaultConfig.append(id, defVal);
    storage.widgetDetailsReceive(id, visib, count);
}

} // namespace S2
