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

BoardConfiguration &DataManager::getConfiguration(const BoardType boardType) noexcept
{
    return m_data.at(boardType);
}

const BoardConfiguration &DataManager::getConfiguration(const BoardType boardType) const noexcept
{
    return m_data.at(boardType);
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
    return m_currentParseTarget == BoardType::Base;
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

void DataManager::parseS2File(const QByteArray &rawFile)
{
    S2DataFactory factory { m_storage };
    std::map<quint32, DataItem> dataFromFile;
    auto result = m_util.convert(rawFile, factory, dataFromFile);
    if (result == Error::Msg::NoError)
    {
        // Для конфигурации каждой платы находим соответствие
        for (const auto &[id, value] : dataFromFile)
            for (auto &[_, boardConfig] : m_data)
                if (boardConfig.m_defaultConfig.contains(id))
                    boardConfig.m_workingConfig.setRecord(id, value);
    }
    emit parseStatus(result);
}

QByteArray DataManager::getBinaryConfiguration() const
{
    // В новую конфигурацию копируем данные из всех имеющихся
    S2Configuration all { m_storage };
    for (auto &[_, boardConfig] : m_data)
        all.merge(boardConfig.m_workingConfig);
    return all.toByteArray();
}

void DataManager::startNewConfig()
{
    constexpr std::array<BoardType, 2> boardTypes { BoardType::Base, BoardType::Mezz };
    for (const auto boardType : boardTypes)
    {
        auto search = m_data.find(boardType);
        if (search == m_data.end())
        {
            m_data.insert(
                { boardType, BoardConfiguration { "", S2Configuration(m_storage), S2Configuration(m_storage) } });
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

void DataManager::configNameReceive(const QString &tabName)
{
    auto &currentName = m_data.at(m_currentParseTarget).m_tabName;
    if (!tabName.isEmpty())
        currentName = tabName;
    else
        currentName = "Конфигурация";
}

} // namespace S2
