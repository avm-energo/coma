#pragma once

#include <QDebug>
#include <algorithm>
#include <interfaces/utils/datamanager.h>
#include <typeinfo>

/*! \details That class represents a connection between datamanager and classes (user classes) that usage datamanager.
 *  That class using a simple conception: data manager sending signal with QVariant and hash of stored type in QVariant
 *  instance. Instance of connection class stores hash of registered type (the class user must register type for each
 *  instance of connection class). When receiving a signal from data manager, hashes of the types are compared. If
 *  hashes are equal, then connection class calls signal and passes QVariant to the user class.
 * \see DataManager
 */
class DataTypesProxy : public QObject
{
    Q_OBJECT
private:
    std::size_t hash_type;

public:
    DataTypesProxy(QObject *parent = nullptr) : QObject(parent), hash_type(0)
    {
    }

    /// \brief A function for registering type whose objects will be sent to the user class in QVariant instance.
    template <class T> void RegisterType()
    {
        hash_type = std::hash<std::string> {}(typeid(T).name());
        auto mngr = &DataManager::GetInstance();
        connect(mngr, &DataManager::DataReceived, this, &DataTypesProxy::DataHandled, Qt::QueuedConnection);
    }

public slots:
    /// \brief A slot that compares hash from datamanager with stored hash
    void DataHandled(const std::size_t &hash, const QVariant &var)
    {
        if (hash_type == hash)
            emit DataStorable(var);
    }

signals:
    void DataStorable(const QVariant &);
};
