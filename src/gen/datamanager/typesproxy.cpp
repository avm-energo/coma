#include "typesproxy.h"

DataTypesProxy::DataTypesProxy(QObject *parent) : QObject(parent), hash_type(0)
{
    auto mngr = &DataManager::GetInstance();
    connect(mngr, &DataManager::DataReceived, this, &DataTypesProxy::DataHandled);
}
