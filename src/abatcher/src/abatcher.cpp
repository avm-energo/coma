#include "abatcher.h"

#include "interfaces/types/usbhid_settings.h"
#include "moduleWorker/moduleWorker.h"
#include <avm-gen/settings.h>
#include <avm-widgets/animatedpopup.h>
#include <avm-widgets/emessagebox.h>
#include <avm-widgets/styleloader.h>
#include <common/names.h>
#include <interfaces/types/usbhidportinfo.h>

#include <QCheckBox>
#include <QCoreApplication>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QVBoxLayout>

Abatcher::Abatcher(QWidget *parent) : QMainWindow(parent)
{
    m_widgets = new QStackedWidget(this);
    setCentralWidget(m_widgets);

    createUSBConnectionsWindow();
    createDryCheckBox();
    createStartPushButton();

    AnimatedPopup::setParent(this);
}
Abatcher::~Abatcher() { }

void Abatcher::setupUI() 
{
    QString caption(QCoreApplication::applicationName());
    caption.append(" v").append(QCoreApplication::applicationVersion());
    setWindowTitle(caption);
    setMinimumSize(QSize(600, 700));

    auto wdgt = new QWidget(this);
    auto lyout = new QVBoxLayout(wdgt);
    auto hlyout = new QHBoxLayout();

    lyout->addWidget(m_usbCons);

    hlyout->addWidget(m_isDry);
    hlyout->addStretch();
    hlyout->addWidget(m_startButton);

    lyout->addLayout(hlyout);

    wdgt->setLayout(lyout);

    m_widgets->addWidget(wdgt);

    updateUSBConnectionWindow();

    StyleLoader::setStyle(Settings::get("theme", "Dark"));
}

void Abatcher::createDryCheckBox()
{
    m_isDry = new QCheckBox("Пробный запуск", this);
}

void Abatcher::run()
{
    setupUI();
    m_updateTicker->start();
    show();
}

void Abatcher::createUSBConnectionsWindow()
{
    m_usbCons = new QTableView(this);
    m_usbCons->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_usbCons->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usbCons->setSelectionMode(QAbstractItemView::SingleSelection);

    m_usbModel = new QStandardItemModel(this);
    m_usbModel->setHorizontalHeaderLabels({"VID", "PID", "Serial"});

    m_usbCons->setModel(m_usbModel);
    m_usbCons->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_updateTicker = new QTimer(this);
    m_updateTicker->setInterval(3000); // 3 секунд

    connect(m_updateTicker, &QTimer::timeout, this, &Abatcher::updateUSBConnectionWindow);
    connect(m_usbCons, &QTableView::doubleClicked, this, &Abatcher::startModuleWorker);
}

void Abatcher::updateUSBConnectionWindow()
{
    m_usbModel->removeRows(0, m_usbModel->rowCount());

    auto usbDevices = UsbHidPortInfo::devicesFound();

    for (const auto &row : std::as_const(usbDevices))
    {
        m_usbModel->appendRow({
            new QStandardItem(QString::number(row->get<u16>(MemKeys::USB::vendor_id), 16)),
            new QStandardItem(QString::number(row->get<u16>(MemKeys::USB::product_id), 16)),
            new QStandardItem(row->get<QString>(MemKeys::USB::serial))
        });
    }
}

void Abatcher::createStartPushButton()
{
    m_startButton = new QPushButton("Запуск", this);
    m_startButton->setFixedSize(m_startButton->sizeHint());

    connect(m_startButton, &QPushButton::clicked, this, &Abatcher::startModuleWorker);
}

void Abatcher::startModuleWorker()
{
    bool isDry = m_isDry->isChecked();

    QItemSelectionModel *selectionModel = m_usbCons->selectionModel();
    QModelIndexList selectedRows = selectionModel->selectedRows();

    if (selectedRows.isEmpty())
    {
        EMessageBox::error(this, "Устройство не выбрано");
        return;
    }

    QModelIndex currentRow = selectedRows.first();
    const QAbstractItemModel *mdl = currentRow.model();
    int row = currentRow.row();

    UsbHidSettings *usb = new UsbHidSettings(this);
    usb->set(MemKeys::USB::vendor_id, mdl->data(mdl->index(row, 0)).toString().toUInt(nullptr, 16));
    usb->set(MemKeys::USB::product_id, mdl->data(mdl->index(row, 1)).toString().toUInt(nullptr, 16));
    usb->set(MemKeys::USB::serial, mdl->data(mdl->index(row, 2)).toString());
    usb->set(MemKeys::timeout, Settings::get(SettingsKeys::USB::protocomTimeout, 5000));
    usb->set(MemKeys::reconnectInterval, Settings::get(SettingsKeys::USB::protocomReconnect, 100));

    ModuleWorker *mw = new ModuleWorker(isDry, usb, this);
    m_widgets->addWidget(mw);
    m_widgets->setCurrentIndex(1);

    connect(mw, &ModuleWorker::finishWork, this,
        [this, mw]
        {
            m_widgets->setCurrentIndex(0);

            m_widgets->removeWidget(mw);
            delete mw;
        });
}