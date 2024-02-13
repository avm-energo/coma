#include "board.h"

#include "modules.h"

#include <QDebug>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <interfaces/conn/async_connection.h>

using namespace Interface;

Board::Board(Singleton::token)
{
}

quint16 Board::typeB() const
{
    return m_startupInfoBlock.MTypeB;
}

quint16 Board::typeM() const
{
    return m_startupInfoBlock.MTypeM;
}

quint32 Board::serialNumber(Board::Types type) const
{
    switch (type)
    {
    case Base:
        return m_startupInfoBlock.SerialNumB;
    case Mezzanine:
        return m_startupInfoBlock.SerialNumM;
    case None:
        return 0;
    default:
        return m_startupInfoBlock.SerialNum;
    }
}

QString Board::UID() const
{
    return QString::number(m_startupInfoBlock.UIDHigh, 16) + QString::number(m_startupInfoBlock.UIDMid, 16)
        + QString::number(m_startupInfoBlock.UIDLow, 16);
}

bool Board::noConfig() const
{
    return (m_startupInfoBlock.Hth & HTH_CONFIG);
}

void Board::updateExt(const DataTypes::BitStringStruct &bs)
{
    constexpr auto minCount = sizeof(Modules::StartupInfoBlockExt0) / sizeof(quint32);
    constexpr auto lastExt0Reg = addr::bsiExtStartReg + minCount;
    // BsiExt0
    if ((bs.sigAdr >= addr::bsiExtStartReg) && (bs.sigAdr <= lastExt0Reg))
    {
        quint32 &item = *(reinterpret_cast<quint32 *>(&m_startupInfoBlockExt) + (bs.sigAdr - addr::bsiExtStartReg));
        item = bs.sigVal;
        ++m_updateCounterExt;
    }
    if (m_updateCounterExt == minCount)
        m_updateCounterExt = 0;
}
