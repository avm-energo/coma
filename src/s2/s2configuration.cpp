#include "s2configuration.h"

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

} // namespace S2
