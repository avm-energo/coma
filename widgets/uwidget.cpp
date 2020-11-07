#include "uwidget.h"

UWidget::UWidget(QWidget *parent) : QWidget(parent)
{
    m_updatesEnabled = false;
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

void UWidget::setInterface(BaseInterface *iface)
{
    m_iface = iface;
}

BaseInterface *UWidget::iface()
{
    return m_iface;
}

void UWidget::update()
{
}
