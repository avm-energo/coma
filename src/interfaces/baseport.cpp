#include "baseport.h"

BasePort::BasePort(const QString &logFilename, QObject *parent) : QObject(parent), Log(new LogClass(this))
{
    QString filename(logFilename);
    filename.append(".").append(::logExt);
    Log->Init(filename);
    Log->WriteRaw(::logStart);
}

void BasePort::setState(Interface::State state)
{
    m_state = state;
    emit stateChanged(m_state);
}
