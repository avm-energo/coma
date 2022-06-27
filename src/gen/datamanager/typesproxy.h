#ifndef TYPESPROXY_H
#define TYPESPROXY_H

#include "../datatypes.h"
#include "datamanager.h"

#include <algorithm>
#include <typeinfo>
#include <vector>

class DataTypesProxy : public QObject
{
    Q_OBJECT
private:
    DataManager *manager;
    std::vector<std::size_t> hash_types;

    bool HasHash(const std::size_t &hash)
    {
        auto iter = std::find(hash_types.begin(), hash_types.end(), hash);
        return (iter != hash_types.end());
    }

public:
    DataTypesProxy(DataManager *mngr, QObject *parent = nullptr) : QObject(parent), manager(mngr)
    {
        connect(manager, &DataManager::dataReceived, this, &DataTypesProxy::DataReceived);
        hash_types.reserve(16);
    }

    template <class T> void RegisterType()
    {
        auto hash = typeid(T).hash_code();
        if (!HasHash(hash))
            hash_types.push_back(hash);
    }

    /// \see https://stackoverflow.com/questions/8793658/functions-with-variadic-templates
    template <typename H, typename T, typename... R> void RegisterType()
    {
        auto hash = typeid(H).hash_code();
        if (!HasHash(hash))
            hash_types.push_back(hash);
        RegisterType<T, R...>();
    }

public slots:
    void DataReceived(const std::size_t &hash, const QVariant &var)
    {
        if (HasHash(hash))
        {
            emit DataStorable(var);
        }
    }

signals:
    void DataStorable(const QVariant &);
};

#endif // TYPESPROXY_H
