#include "typesproxy.h"

DataTypesProxy::DataTypesProxy(QObject *parent) : QObject(parent), hash_type(0)
{
    //    auto mngr = &DataManager::GetInstance();
    //    auto conn = connect(mngr, &DataManager::DataReceived, this, &DataTypesProxy::DataHandled);
    //    Q_ASSERT(bool(conn));
}
