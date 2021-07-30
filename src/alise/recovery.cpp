#include "recovery.h"

#include "../gen/modules.h"

#include <QDebug>
#include <QDir>

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

    QFile::copy(":/network/eth0", eth0path);
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

    QFile::copy(":/network/eth2", eth2path);
}

void Recovery::receiveBlock(const DataTypes::BlockStruct blk)
{
    switch (blk.data.size())
    {
    case sizeof(AVTUK_14::Main):
    {
        AVTUK_14::Main mainBlock;
        memcpy(&mainBlock, blk.data.data(), sizeof(mainBlock));
        if (mainBlock.resetReq)
        {
            eth0();
            eth2();
            emit rebootReq();
        }
    }
    }
}
