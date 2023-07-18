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

Configuration::Configuration(const S2ConfigStorage &storage) : s2confStorage(storage), factory(storage)
{
}

quint32 Configuration::getIdByName(const QString &name) const noexcept
{
    const auto &nameMap = s2confStorage.getIdByNameMap();
    auto search = nameMap.find(name);
    if (search == nameMap.cend())
        return 0;
    else
        return search->second;
}

const Configuration &Configuration::operator=(const Configuration &rhs)
{
    // Не копируем data factory
    data = rhs.data;
    return *this;
}

const DataItem &Configuration::operator[](const quint32 id) const
{
    return data.at(id);
}

DataItem &Configuration::operator[](const quint32 id)
{
    return data.at(id);
}

const DataItem &Configuration::operator[](const QString &name) const
{
    auto id = getIdByName(name);
    return data.at(id);
}

DataItem &Configuration::operator[](const QString &name)
{
    auto id = getIdByName(name);
    return data.at(id);
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

const S2ConfigStorage &Configuration::getConfigStorage() const noexcept
{
    return s2confStorage;
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

void Configuration::setRecord(const quint32 id, const DataItem &record)
{
    data.insert_or_assign(id, record);
}

void Configuration::setRecord(const quint32 id, const valueType &value)
{
    data.insert_or_assign(id, DataItem { id, value });
}

// const DataItem &Configuration::getRecord(const quint32 id) const
//{
//    return data.at(id);
//}

// const DataItem &Configuration::getRecord(const QString &name) const
//{
//    auto id = util.getIdByName(name);
//    return data.at(id);
//}

QByteArray Configuration::toByteArray() const
{
    return util.convert(*this, std_ext::to_underlying(FilesEnum::Config));
}

bool Configuration::updateByRawData(const QByteArray &rawData)
{
    std::map<quint32, DataItem> dataFromFile;
    auto result = util.convert(rawData, factory, dataFromFile);
    switch (result)
    {
    case Error::Msg::NoError:
        data = dataFromFile;
        return true;
    case Error::Msg::HeaderSizeError:
        qWarning() << "Размер файла меньше установленного размера заголовка.";
        break;
    case Error::Msg::WrongFileError:
        qWarning() << "Передан файл, не являющийся конфигурацией.";
        break;
    case Error::Msg::SizeError:
        qWarning() << "Ошибка размера: выход за границу принятых байт.";
        break;
    case Error::Msg::CrcError:
        qWarning() << "Получена некорректная контрольная сумма.";
        break;
    default:
        break;
    }
    return false;
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
