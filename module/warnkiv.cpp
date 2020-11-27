#include "warnkiv.h"

#include "module_kiv.h"
#include "modules.h"

WarnKIV::WarnKIV(QWidget *parent) : Warn(parent)
{
    //    m_warnBdNum = 11;
    m_startWarnAddress = 3011;
    m_warnFlags = std::bitset<32>(AVM_KIV::warnFlags);

    setupUI(AVM_KIV::warnEvents);
}
