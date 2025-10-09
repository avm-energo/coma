#include "dialogs/interfaceusbdialog.h"

#include <common/names.h>
#include <gen/error.h>
#include <gen/settings.h>
#include <interfaces/types/serial_settings.h>
#include <interfaces/types/usbhidportinfo.h>
#include <widgets/tvfunc.h>

#include <QMessageBox>
#include <QStandardItem>
#include <QVBoxLayout>

InterfaceUSBDialog::InterfaceUSBDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
    setWindowTitle("USB соединения");
}

void InterfaceUSBDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    m_tableView = TVFunc::New(this, "", nullptr);
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

    UsbHidSettings *settings = new UsbHidSettings;
    settings->set(MemKeys::USB::vendor_id, mdl->data(mdl->index(row, 0)).toString().toUInt(nullptr, 16));
    settings->set(MemKeys::USB::product_id, mdl->data(mdl->index(row, 1)).toString().toUInt(nullptr, 16));
    settings->set(MemKeys::USB::serial, mdl->data(mdl->index(row, 2)).toString());
#ifdef QT_DEBUG
    settings->set(MemKeys::USB::path, mdl->data(mdl->index(row, 3)).toString());
#endif
    settings->set(MemKeys::timeout, Settings::get(SettingsKeys::USB::protocomTimeout, 5000));
    settings->set(MemKeys::reconnectInterval, Settings::get(SettingsKeys::USB::protocomReconnect, 100));
    apply(settings);

    ConnectionSettings st { QString(), settings };
    emit accepted(st);
}

bool InterfaceUSBDialog::updateModel()
{
    auto usbDevices = UsbHidPortInfo::devicesFound();
    if (usbDevices.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка", "Устройства не найдены");
        qWarning() << "Устройства не найдены";
        return false;
    }
    QStringList sl { "VID", "PID", "Serial" };
#ifdef QT_DEBUG
    sl.push_back("Path");
#endif
    QStandardItemModel *mdl = new QStandardItemModel(this);

    mdl->setHorizontalHeaderLabels(sl);
    for (auto row : usbDevices)
    {
        QList<QStandardItem *> device {
            new QStandardItem(QString::number(row->get<u16>(MemKeys::USB::vendor_id), 16)),  //
            new QStandardItem(QString::number(row->get<u16>(MemKeys::USB::product_id), 16)), //
            new QStandardItem(row->get<QString>(MemKeys::USB::serial)),                      //
#ifdef QT_DEBUG
            new QStandardItem(row->get<QString>(MemKeys::USB::path))                         //
#endif
        };
        mdl->appendRow(device);
    }
    m_tableView->setModel(mdl);
    m_tableView->resizeColumnsToContents();
    return true;
}
