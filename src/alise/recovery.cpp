#include "recovery.h"

#include "../gen/modules.h"

#include <QDir>

constexpr char eth0path[] = "/etc/network/interfaces.d/eth0";
constexpr char eth1path[] = "/etc/network/interfaces.d/eth1";

Recovery::Recovery(QObject *parent) : QObject(parent)
{
}

void Recovery::eth0()
{
    if (QFile::exists(eth0path))
    {
        QFile::remove(eth0path);
    }

    QFile::copy(":/network/eth0", eth0path);
}

void Recovery::eth1()
{
    if (QFile::exists(eth1path))
    {
        QFile::remove(eth1path);
    }

    QFile::copy(":/network/eth1", eth1path);
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
            eth1();
            emit rebootReq();
        }
    }
    }
}
