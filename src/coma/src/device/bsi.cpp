#include "device/bsi.h"

#include <avm-gen/stdfunc.h>
Device::Bsi::Bsi()
{
    clearBsi();
}

Device::Bsi::Bsi(const Bsi &other)
{
    *this = other;
}

void Device::Bsi::clearBsi()
{
    for (auto &arr : m_bsi)
        arr = 0;
    clearBsiFill();
}

void Device::Bsi::clearBsiFill()
{
    m_fillBits.reset();
}

void Device::Bsi::setData(BsiIndexes index, u32 value)
{
    m_bsi[index] = value;
    m_fillBits.set(index);
    if (m_fillBits.all())
        emit bsiIsFilled();
}

void Device::Bsi::setData(u32 index, u32 value)
{
    if (index < bsiCountRegs)
        setData(static_cast<BsiIndexes>(index), value);
}

u32 Device::Bsi::data(BsiIndexes index) const
{
    return m_bsi[index];
}

u32 Device::Bsi::data(u32 index) const
{
    return data(static_cast<BsiIndexes>(index));
}

bool Device::Bsi::isFilled()
{
    return m_fillBits.all();
}

Device::Bsi &Device::Bsi::operator=(const Bsi &other)
{
    for (u32 i = 0; i < bsiCountRegs; ++i)
        setData(i, other.data(i));
    return *this;
}

QByteArray Device::Bsi::toByteArray() const
{
    return StdFunc::toByteArray(m_bsi);
}

void Device::Bsi::fromByteArray(const QByteArray &ba)
{
    u32 endOffset = ba.size();
    u32 offset = 0;
    u32 count = 0;
    while (offset < endOffset)
    {
        m_bsi[count++] = StdFunc::getFromByteArray<u32>(ba, offset);
        offset += 4;
    }
}

u32 Device::Bsi::size() const
{
    return sizeof(m_bsi);
}
