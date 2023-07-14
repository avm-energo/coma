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

Configuration::Configuration(const S2ConfigStorage &storage) : factory(storage)
{
}

bool Configuration::append(const S2::DataRec &record)
{
    const auto [it, success] = data.insert({ record.header.id, factory.create(record) });
    Q_UNUSED(it);
    return success;
}

bool Configuration::append(const quint16 id, const QByteArray &bytes)
{
    const auto [it, success] = data.insert({ id, factory.create(id, bytes) });
    Q_UNUSED(it);
    return success;
}

bool Configuration::append(const quint16 id, const QString &str)
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

Configuration::ConstIter Configuration::cbegin() const noexcept
{
    return data.cbegin();
}

Configuration::ConstIter Configuration::cend() const noexcept
{
    return data.cend();
}

std::vector<quint16> Configuration::checkDiff(const Configuration &rhs) const
{
    std::set<quint16> currentItems, receivedtItems;
    std::transform(data.cbegin(), data.cend(), detail::setInserter(currentItems), //
        [](const auto &iter) { return iter.first; });
    std::transform(rhs.data.cbegin(), rhs.data.cend(), detail::setInserter(receivedtItems), //
        [](const auto &iter) { return iter.first; });

    std::vector<quint16> diffItems;
    std::set_difference(currentItems.cbegin(), currentItems.cend(), //
        receivedtItems.cbegin(), receivedtItems.cend(),             //
        std::back_inserter(diffItems)                               //
    );
    return diffItems;
}

} // namespace S2
