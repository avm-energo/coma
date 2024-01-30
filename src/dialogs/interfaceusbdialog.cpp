#include "interfaceusbdialog.h"

#include "../widgets/wd_func.h"

#include <QMessageBox>
#include <QStandardItem>
#include <QVBoxLayout>
#include <gen/error.h>
#include <interfaces/types/settingstypes.h>
#include <interfaces/types/usbhidportinfo.h>

InterfaceUSBDialog::InterfaceUSBDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
    setWindowTitle("USB соединения");
}

void InterfaceUSBDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    m_tableView = WDFunc::NewQTV(this, "usbtv", nullptr);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // no editable view
    lyout->addWidget(m_tableView);
    connect(m_tableView, &QTableView::doubleClicked, this, &InterfaceUSBDialog::setInterface);
    setLayout(lyout);
    AbstractInterfaceDialog::setupUI();
}

void InterfaceUSBDialog::setInterface(QModelIndex index)
{
    if (!index.isValid())
        return;
    auto *mdl = index.model();
    int row = index.row();

    UsbHidSettings settings;
    settings.vendor_id = mdl->data(mdl->index(row, 0)).toString().toUInt(nullptr, 16);
    settings.product_id = mdl->data(mdl->index(row, 1)).toString().toUInt(nullptr, 16);
    settings.serial = mdl->data(mdl->index(row, 2)).toString();
#ifdef QT_DEBUG
    settings.path = mdl->data(mdl->index(row, 3)).toString();
#endif
    m_settings.native().beginGroup("settings");
    settings.m_timeout = m_settings.get<Settings::ProtocomTimeout>();
    settings.m_reconnectInterval = m_settings.get<Settings::ProtocomReconnect>();
    fill(settings);
    m_settings.native().endGroup();

    ConnectStruct st { QString(), settings };
    emit accepted(st);
}

bool InterfaceUSBDialog::updateModel()
{
    auto usbDevices = UsbHidPortInfo::devicesFound();
    if (usbDevices.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка", "Устройства не найдены");
        qCritical() << Error::Msg::NoDeviceError;
        return false;
    }
    QStringList sl { "VID", "PID", "Serial" };
#ifdef QT_DEBUG
    sl.push_back("Path");
#endif
    QStandardItemModel *mdl = new QStandardItemModel(this);

    mdl->setHorizontalHeaderLabels(sl);
    for (const auto &row : usbDevices)
    {
        QList<QStandardItem *> device {
            new QStandardItem(QString::number(row.vendor_id, 16)),  //
            new QStandardItem(QString::number(row.product_id, 16)), //
            new QStandardItem(row.serial),                          //
#ifdef QT_DEBUG
            new QStandardItem(row.path) //
#endif
        };
        mdl->appendRow(device);
    }
    m_tableView->setModel(mdl);
    m_tableView->resizeColumnsToContents();
    return true;
}
