#include "s2datamanager.h"

#include <QDebug>

namespace S2
{

DataManager::DataManager(QObject *parent) : QObject(parent), m_storage(S2ConfigStorage::GetInstance())
{
}

S2ConfigStorage &DataManager::getStorage() noexcept
{
    return m_storage;
}

const S2ConfigStorage &DataManager::getStorage() const noexcept
{
    return m_storage;
}

BoardConfiguration &DataManager::getCurrentConfiguration() noexcept
{
    return m_data.at(m_currentParseTarget);
}

const BoardConfiguration &DataManager::getCurrentConfiguration() const noexcept
{
    return m_data.at(m_currentParseTarget);
}

void DataManager::clear() noexcept
{
    m_data.clear();
    m_storage.clearDetailData();
}

bool DataManager::isOneBoard() const noexcept
{
    // Если в парсинге использовался один XML-файл, то модуль
    // состоит из одной платы и имеет один диалог для конфигурации.
    return m_currentParseTarget == BoardConfig::Base;
}

DataManager::Iter DataManager::begin() noexcept
{
    return m_data.begin();
}

DataManager::Iter DataManager::end() noexcept
{
    return m_data.end();
}

DataManager::ConstIter DataManager::begin() const noexcept
{
    return m_data.cbegin();
}

DataManager::ConstIter DataManager::end() const noexcept
{
    return m_data.cend();
}

void DataManager::startNewConfig()
{
    constexpr std::array<BoardConfig, 2> boardTypes { BoardConfig::Base, BoardConfig::Mezz };
    for (const auto &boardType : boardTypes)
    {
        auto search = m_data.find(boardType);
        if (search == m_data.end())
        {
            m_data.insert({ boardType, BoardConfiguration { S2Configuration(m_storage), S2Configuration(m_storage) } });
            m_currentParseTarget = boardType;
            break;
        }
    }
}

void DataManager::configDataReceive(const quint16 id, const QString &defVal, const bool visib, const quint16 count)
{
    // Перед вызовом configDataReceive гарантированно будет вызван startNewConfig,
    // поэтому currentParseTarget будет гарантированно инициализирован значением.
    m_data.at(m_currentParseTarget).m_defaultConfig.append(id, defVal);
    m_storage.widgetDetailsReceive(id, visib, count);
}

} // namespace S2
