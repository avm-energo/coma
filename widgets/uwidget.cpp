#include "uwidget.h"

#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"
#include "wd_func.h"

#include <QCoreApplication>
#include <QEventLoop>

UWidget::UWidget(QWidget *parent) : QWidget(parent)
{
    m_updatesEnabled = false;
    m_timerCounter = 0;
    m_timerMax = 2; // 2 seconds by default
    m_floatBdQueryList.clear();
    m_spBdQueryList.clear();
    m_highlightMap.clear();
}

void UWidget::setUpdatesEnabled()
{
    m_updatesEnabled = true;
}

void UWidget::setUpdatesDisabled()
{
    m_updatesEnabled = false;
}

const QString UWidget::getCaption()
{
    return m_caption;
}

void UWidget::setCaption(const QString &caption)
{
    m_caption = caption;
}

void UWidget::setUpdateTimerPeriod(quint32 period)
{
    m_timerMax = period;
}

void UWidget::setHighlightMap(const QMap<int, QList<UWidget::HighlightWarnAlarmStruct>> &map)
{
    m_highlightMap = map;
}

void UWidget::setFloatBdQuery(const QList<UWidget::BdQuery> &list)
{
    m_floatBdQueryList = list;
}

void UWidget::setSpBdQuery(const QList<UWidget::BdQuery> &list)
{
    m_spBdQueryList = list;
}

void UWidget::updateFloatData(const DataTypes::FloatStruct &fl)
{
    ++m_timerCounter;
    if ((m_updatesEnabled) && (m_timerCounter >= m_timerMax)) // every second tick of the timer
    {
        WDFunc::SetLBLText(this, QString::number(fl.sigAdr), WDFunc::StringValueWithCheck(fl.sigVal, 3));
        m_timerCounter = 0;
    }
}

void UWidget::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    QList<HighlightWarnAlarmStruct> hstlist = m_highlightMap.value(sp.sigAdr);
    for (const auto &hst : hstlist)
        WDFunc::SetLBLTColor(this, QString::number(hst.fieldnum), (sp.sigVal == 1) ? Colors::TABCOLORA1 : hst.color);
}

void UWidget::reqUpdate()
{
    // NOTE: POS (Piece Of Shit)
    if (!m_updatesEnabled)
        return;
    for (const auto &query : m_floatBdQueryList)
        iface()->reqFloats(query.sigAdr, query.sigQuantity);
    for (const auto &query : m_spBdQueryList)
        iface()->reqAlarms(query.sigAdr, query.sigQuantity);
}

void UWidget::setInterface(BaseInterface *iface)
{
    m_iface = iface;
    uponInterfaceSetting();
}

void UWidget::uponInterfaceSetting()
{
}

BaseInterface *UWidget::iface()
{
    return m_iface;
}
