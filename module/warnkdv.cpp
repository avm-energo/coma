#include "warnkdv.h"

#include "modules.h"

WarnKDV::WarnKDV(QWidget *parent) : Warn(parent)
{
    m_warnBdNum = 11;
    m_startWarnAddress = 3011;
    m_warnFlags = std::bitset<32>(AVM_KDV::warnFlags);

    setupUI(AVM_KDV::warnEvents);
}
