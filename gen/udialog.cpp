#include "udialog.h"

UDialog::UDialog(QWidget *parent) : QDialog(parent)
{
    m_updatesEnabled = false;
}

void UDialog::setUpdatesEnabled()
{
    m_updatesEnabled = true;
}

void UDialog::setUpdatesDisabled()
{
    m_updatesEnabled = false;
}

const QString UDialog::getCaption()
{
    return m_caption;
}

void UDialog::setCaption(const QString &caption)
{
    m_caption = caption;
}

void UDialog::setInterface(BaseInterface *iface)
{
    m_iface = iface;
}

BaseInterface *UDialog::iface()
{
    return m_iface;
}

void UDialog::update()
{
}
