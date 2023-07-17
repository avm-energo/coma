#include "s2configuration.h"

#include <set>

namespace detail
{

// thanx to P0471R0
template <class Set> //
auto setInserter(Set &set)
{
    return std::inserter(set, std::end(set));
}

}

namespace S2
{

Configuration::Configuration(const S2ConfigStorage &storage) : factory(storage), util(storage)
{
}

const Configuration &Configuration::operator=(const Configuration &rhs)
{
    // Не копируем data factory
    data = rhs.data;
    return *this;
}

bool Configuration::append(const S2::DataRec &record)
{
    const auto [it, success] = data.insert({ record.header.id, factory.create(record) });
    Q_UNUSED(it);
    return success;
}

bool Configuration::append(const quint32 id, const QByteArray &bytes)
{
    const auto [it, success] = data.insert({ id, factory.create(id, bytes) });
    Q_UNUSED(it);
    return success;
}

bool Configuration::append(const quint32 id, const QString &str)
{
    const auto [it, success] = data.insert({ id, factory.create(id, str) });
    Q_UNUSED(it);
    return success;
}

Configuration::Iter Configuration::begin() noexcept
{
    return data.begin();
}

Configuration::Iter Configuration::end() noexcept
{
    return data.end();
}

Configuration::ConstIter Configuration::begin() const noexcept
{
    return data.cbegin();
}

Configuration::ConstIter Configuration::end() const noexcept
{
    return data.cend();
}

void Configuration::setRecord(const quint32 id, const DataItem &record)
{
    data.insert_or_assign(id, record);
}

void Configuration::setRecord(const quint32 id, const valueType &value)
{
    data.insert_or_assign(id, DataItem { id, value });
}

const DataItem &Configuration::getRecord(const quint32 id) const
{
    return data.at(id);
}

const DataItem &Configuration::getRecord(const QString &name) const
{
    auto id = util.getIdByName(name);
    return data.at(id);
}

std::vector<quint32> Configuration::checkDiff(const Configuration &rhs) const
{
    std::set<quint32> currentItems, receivedtItems;
    std::transform(data.cbegin(), data.cend(), detail::setInserter(currentItems), //
        [](const auto &iter) { return iter.first; });
    std::transform(rhs.data.cbegin(), rhs.data.cend(), detail::setInserter(receivedtItems), //
        [](const auto &iter) { return iter.first; });

    std::vector<quint32> diffItems;
    std::set_difference(currentItems.cbegin(), currentItems.cend(), //
        receivedtItems.cbegin(), receivedtItems.cend(),             //
        std::back_inserter(diffItems)                               //
    );
    return diffItems;
}

} // namespace S2
