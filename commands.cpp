// commands.cpp
#include <QCoreApplication>
#include "canal.h"

int GetBsi(void *ptr, quint32 size)
{
    cn.Send(CN_GBsi, Canal::BT_NONE, ptr, size);
    while (cn.Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    return cn.result;
}
