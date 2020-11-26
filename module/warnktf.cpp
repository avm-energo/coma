#include "warnktf.h"

#include "modules.h"

WarnKTF::WarnKTF(QWidget *parent) : Warn(parent)
{
    //    m_warnBdNum = 16;
    m_startWarnAddress = 5011;
    m_warnFlags = std::bitset<32>(AVM_KTF::warnFlags);

    setupUI(AVM_KTF::warnEvents);
}
