#include "uwidget.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"
#include "wd_func.h"
//#define UWIDGET_DEBUG
#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
UWidget::UWidget(QWidget *parent) : QWidget(parent)
{
    m_updatesEnabled = false;
    // Отключим обновление виджета по умолчанию
    //    QWidget::setUpdatesEnabled(false);
}

void UWidget::setUpdatesEnabled()
{
    //    Q_ASSERT(!updatesEnabled());
    //    QWidget::setUpdatesEnabled(true);
    m_updatesEnabled = true;
}

void UWidget::setUpdatesDisabled()
{
    //    Q_ASSERT(updatesEnabled());
    //    QWidget::setUpdatesEnabled(false);
    m_updatesEnabled = false;
}

const QString UWidget::getCaption()
{
    return QWidget::windowTitle();
}

void UWidget::setCaption(const QString &caption)
{
    QWidget::setWindowTitle(caption);
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

void UWidget::setBsBdQuery(const QList<UWidget::BdQuery> &list)
{
    m_bsBdQueryList = list;
}

void UWidget::updateFloatData(const DataTypes::FloatStruct &fl)
{
    //    if ((updatesEnabled()) && m_withGUI)
    if (m_updatesEnabled)
    {
        bool result = WDFunc::SetLBLText(this, QString::number(fl.sigAdr), WDFunc::StringValueWithCheck(fl.sigVal, 3));
#ifdef UWIDGET_DEBUG
        if (!result)
            qDebug() << Error::DescError << QString::number(fl.sigAdr) << WDFunc::StringValueWithCheck(fl.sigVal, 3);
#else
        Q_UNUSED(result)
#endif
    }
}

void UWidget::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    //    if ((updatesEnabled()) && m_withGUI)
    if (m_updatesEnabled)
    {
        QList<HighlightWarnAlarmStruct> hstlist = m_highlightMap.value(sp.sigAdr);
        for (const auto &hst : hstlist)
            WDFunc::SetLBLTColor(
                this, QString::number(hst.fieldnum), (sp.sigVal == 1) ? Colors::TABCOLORA1 : hst.color);
    }
}

void UWidget::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    Q_UNUSED(bs)
}

bool UWidget::updatesEnabled()
{
    //    return QWidget::updatesEnabled();
    return m_updatesEnabled;
}

void UWidget::reqUpdate()
{
    // NOTE: POS (Piece Of Shit)
    if (!updatesEnabled())
        return;
    for (const auto &query : qAsConst(m_floatBdQueryList))
        BaseInterface::iface()->reqFloats(query.sigAdr, query.sigQuantity);
    for (const auto &query : qAsConst(m_spBdQueryList))
        BaseInterface::iface()->reqAlarms(query.sigAdr, query.sigQuantity);
    for (const auto &query : qAsConst(m_bsBdQueryList))
        BaseInterface::iface()->reqBitStrings(query.sigAdr, query.sigQuantity);
}

void UWidget::uponInterfaceSetting()
{
}

bool UWidget::checkPassword()
{
    Q_ASSERT(!m_hash.isEmpty());
    KeyPressDialog *dlg = new KeyPressDialog;
    return dlg->CheckPassword(m_hash);
}
