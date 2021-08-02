#include "recovery.h"

#include "../gen/modules.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

constexpr char eth0path[] = "/etc/network/interfaces.d/eth0";
constexpr char eth2path[] = "/etc/network/interfaces.d/eth2";

Recovery::Recovery(QObject *parent) : QObject(parent)
{
}

void Recovery::eth0()
{
    if (!QFile::exists(":/network/eth0"))
    {
        qCritical() << "No eth0 recovery";
        return;
    }
    if (QFile::exists(eth0path))
    {
        QFile::remove(eth0path);
    }

    if (!QFile::copy(":/network/eth0", eth0path))
    {
        qCritical() << "Couldn't copy eth0";
        return;
    }
    if (!QFile(eth0path).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser
            | QFileDevice::ReadGroup | QFileDevice::ReadOther))
    {
        qCritical() << "Couldn't change perm for " << eth0path;
    }
}

void Recovery::eth2()
{
    if (!QFile::exists(":/network/eth2"))
    {
        qCritical() << "No eth0 recovery";
        return;
    }
    if (QFile::exists(eth2path))
    {
        QFile::remove(eth2path);
    }

    if (!QFile::copy(":/network/eth2", eth2path))
    {
        qCritical() << "Couldn't copy eth2";
        return;
    }
    if (!QFile(eth2path).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser
            | QFileDevice::ReadGroup | QFileDevice::ReadOther))
    {
        qCritical() << "Couldn't change perm for " << eth2path;
    }
}

void Recovery::sync()
{
    QString program = "sync";
    QProcess *myProcess = new QProcess(this);
    myProcess->setProgram(program);
    myProcess->start();
    myProcess->waitForFinished();
}

void Recovery::restartNetwork()
{
    QString program = "/etc/init.d/networking";
    QStringList arguments { "restart" };
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
    myProcess->waitForFinished();
}

void Recovery::receiveBlock(const DataTypes::BlockStruct blk)
{
    switch (blk.data.size())
    {
    case sizeof(AVTUK_14::Main):
    {
        AVTUK_14::Main mainBlock;
        memcpy(&mainBlock, blk.data.data(), sizeof(mainBlock));
        if (mainBlock.resetReq && (!resetInit))
        {
            eth0();
            eth2();
            sync();
            restartNetwork();
            sync();
            emit rebootReq();
            resetInit = true;
        }
    }
    }
}
