#include "s2/s2configuration.h"

#include <set>

namespace detail
{

// thanx to P0471R0
template <class Set> //
auto setInserter(Set &set)
{
    return std::inserter(set, std::end(set));
}

} // namespace detail

namespace S2
{

Configuration::Configuration(const S2ConfigStorage &storage) : m_storage(storage), m_factory(storage)
{
}

Configuration::Configuration(const Configuration &rhs)
    : m_storage(rhs.m_storage), m_factory(m_storage), m_data(rhs.m_data)
{
}

quint32 Configuration::getIdByName(const QString &name) const noexcept
{
    const auto &nameMap = m_storage.getIdByNameMap();
    auto search = nameMap.find(name);
    Q_ASSERT(search != nameMap.cend());
    if (search != nameMap.cend())
        return search->second;
    else
        return 0;
}

const Configuration &Configuration::operator=(const Configuration &rhs)
{
    // Не копируем data factory
    m_data = rhs.m_data;
    return *this;
}

const DataItem &Configuration::operator[](const quint32 id) const
{
    return m_data.at(id);
}

DataItem &Configuration::operator[](const quint32 id)
{
    return m_data.at(id);
}

const DataItem &Configuration::operator[](const QString &name) const
{
    auto id = getIdByName(name);
    return m_data.at(id);
}

DataItem &Configuration::operator[](const QString &name)
{
    auto id = getIdByName(name);
    return m_data.at(id);
}

Configuration::Iter Configuration::begin() noexcept
{
    return m_data.begin();
}

Configuration::Iter Configuration::end() noexcept
{
    return m_data.end();
}

Configuration::ConstIter Configuration::begin() const noexcept
{
    return m_data.cbegin();
}

Configuration::ConstIter Configuration::end() const noexcept
{
    return m_data.cend();
}

const S2ConfigStorage &Configuration::getConfigStorage() const noexcept
{
    return m_storage;
}

bool Configuration::append(const S2::DataRec &record)
{
    const auto [_, success] = m_data.insert({ record.header.id, m_factory.create(record) });
    Q_UNUSED(_);
    return success;
}

bool Configuration::append(const quint32 id, const QByteArray &bytes)
{
    const auto [_, success] = m_data.insert({ id, m_factory.create(id, bytes) });
    Q_UNUSED(_);
    return success;
}

bool Configuration::append(const quint32 id, const QString &str)
{
    const auto [_, success] = m_data.insert({ id, m_factory.create(id, str) });
    Q_UNUSED(_);
    return success;
}

void Configuration::setRecord(const quint32 id, const DataItem &record)
{
    m_data.insert_or_assign(id, record);
}

void Configuration::setRecord(const quint32 id, const valueType &value)
{
    m_data.insert_or_assign(id, DataItem { value });
}

void Configuration::setRecord(const QString &name, const valueType &value)
{
    auto id = getIdByName(name);
    m_data.insert_or_assign(id, DataItem { value });
}

bool Configuration::contains(const quint32 id) const noexcept
{
    const auto search = m_data.find(id);
    return (search != m_data.cend());
}

bool Configuration::contains(const QString &name) const noexcept
{
    auto id = getIdByName(name);
    return contains(id);
}

QByteArray Configuration::toByteArray() const
{
    return m_util.convert(*this, std_ext::to_underlying(FilesEnum::Config));
}

void Configuration::merge(const Configuration &other)
{
    // Copying data from rhs container
    auto rhs_data_copy { other.m_data };
    // Extract nodes from rhs_data_copy
    // and merge them to current container
    m_data.merge(rhs_data_copy);
}

std::vector<quint32> Configuration::checkDiff(const Configuration &rhs) const
{
    std::set<quint32> currentItems, receivedtItems;
    std::transform(m_data.cbegin(), m_data.cend(), detail::setInserter(currentItems), //
        [](const auto &iter) { return iter.first; });
    std::transform(rhs.m_data.cbegin(), rhs.m_data.cend(), detail::setInserter(receivedtItems), //
        [](const auto &iter) { return iter.first; });

    std::vector<quint32> diffItems;
    std::set_difference(currentItems.cbegin(), currentItems.cend(), //
        receivedtItems.cbegin(), receivedtItems.cend(),             //
        std::back_inserter(diffItems)                               //
    );
    return diffItems;
}

} // namespace S2
