#ifndef TYPESPROXY_H
#define TYPESPROXY_H

#include "../datatypes.h"
#include "datamanager.h"

#include <QDebug>
#include <algorithm>
#include <typeinfo>

class DataTypesProxy : public QObject
{
    Q_OBJECT
private:
    std::size_t hash_type;

public:
    DataTypesProxy(QObject *parent = nullptr);

    template <class T> void RegisterType()
    {
        std::string type_name = typeid(T).name();
        hash_type = std::hash<std::string> {}(type_name);
        auto mngr = &DataManager::GetInstance();
        auto conn = connect(mngr, &DataManager::DataReceived, this, &DataTypesProxy::DataHandled, Qt::QueuedConnection);
        Q_ASSERT(bool(conn));
    }

    /*
    /// \see https://stackoverflow.com/questions/8793658/functions-with-variadic-templates
    template <typename H, typename T, typename... R> void RegisterType()
    {
        auto hash = typeid(H).hash_code();
        if (!HasHash(hash))
            hash_types.push_back(hash);
        RegisterType<T, R...>();
    }
    */

public slots:
    void DataHandled(const std::size_t &hash, const QVariant &var)
    {
        if (hash_type == hash)
        {
            emit DataStorable(var);
        }
    }

signals:
    void DataStorable(const QVariant &);
};

#endif // TYPESPROXY_H
