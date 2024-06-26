#include "widgets/uwidget.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <device/current_device.h>
#include <gen/colors.h>
#include <gen/stdfunc.h>
#include <widgets/epopup.h>
#include <widgets/wd_func.h>

UWidget::UWidget(Device::CurrentDevice *device, QWidget *parent)
    : QWidget(parent), m_dataUpdater(new ModuleDataUpdater(device->async(), this)), m_device(device)
{
    Q_ASSERT(m_device != nullptr);
    // Отключим обновление виджета по умолчанию
    m_dataUpdater->setUpdatesEnabled(false);
    connect(m_dataUpdater, &ModuleDataUpdater::itsTimeToUpdateSinglePointSignal, this, &UWidget::updateSPData);
    connect(m_dataUpdater, &ModuleDataUpdater::itsTimeToUpdateFloatSignal, this, &UWidget::updateFloatData);
    connect(m_dataUpdater, &ModuleDataUpdater::itsTimeToUpdateBitStringSignal, this, &UWidget::updateBitStringData);
}

const QString UWidget::getCaption()
{
    return QWidget::windowTitle();
}

void UWidget::setCaption(const QString &caption)
{
    QWidget::setWindowTitle(caption);
}

void UWidget::updateFloatData(const DataTypes::FloatStruct &fl)
{
    bool result;
    if (fl.sigQuality != DataTypes::Quality::Good)
        result = WDFunc::SetLBLText(this, QString::number(fl.sigAdr), "***");
    else
        result = WDFunc::SetLBLText(this, QString::number(fl.sigAdr), WDFunc::StringFloatValueWithCheck(fl.sigVal, 3));
#ifdef UWIDGET_DEBUG
    if (!result)
        qDebug() << Error::DescError << QString::number(fl.sigAdr) << WDFunc::StringValueWithCheck(fl.sigVal, 3);
#else
    Q_UNUSED(result)
#endif
}

void UWidget::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    Q_UNUSED(sp);
}

void UWidget::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    Q_UNUSED(bs)
}

ModuleDataUpdater *UWidget::engine()
{
    return m_dataUpdater;
}

Device::CurrentDevice *UWidget::device()
{
    return m_device;
}

bool UWidget::updatesEnabled()
{
    return m_dataUpdater->updatesEnabled();
}

void UWidget::reqUpdate()
{
    m_dataUpdater->requestUpdates();
}

void UWidget::uponInterfaceSetting()
{
}

bool UWidget::checkPassword()
{
    return EMessageBox::password(this);
}
