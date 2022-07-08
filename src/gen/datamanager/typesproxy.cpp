#include "typesproxy.h"

DataTypesProxy::DataTypesProxy(QObject *parent) : QObject(parent), hash_type(0)
{
}

void DataTypesProxy::DataHandled(const std::size_t &hash, const QVariant &var)
{
    if (hash_type == hash)
    {
        emit DataStorable(var);
    }
}
