#include "waitthread.h"

#include <QTimer>
#include <QCoreApplication>

WaitThread::WaitThread(QObject *parent) : QObject(parent)
{
    StopRequest = false;
}

void WaitThread::Run()
{
    QTimer *tmr = new QTimer;
    tmr->setInterval(10);
    connect(tmr,SIGNAL(timeout()),this,SIGNAL(TenMsPassed()));
    tmr->setSingleShot(false);
    tmr->start();
    while (!StopRequest)
        qApp->processEvents();
}

void WaitThread::Stop()
{
    StopRequest = true;
}
