#include "connectdialog.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../interfaces/usbhidportinfo.h"
#include "../widgets/wd_func.h"
#include "interfaceethernetdialog.h"
#include "interfaceserialdialog.h"
#include "interfaceusbdialog.h"

#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSerialPortInfo>
#include <QSettings>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QtNetwork/QHostAddress>

ConnectDialog::ConnectDialog(QWidget *parent) : QDialog(parent)
{
    QStringList intersl;
    if (QCoreApplication::applicationName().contains("service", Qt::CaseInsensitive))
        intersl = QStringList { "USB", "Ethernet", "RS485" };
    if (QCoreApplication::applicationName().contains("debug", Qt::CaseInsensitive))
        intersl = QStringList { "USB" };
    setMinimumWidth(150);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL2(this, "Выберите интерфейс связи"));
    lyout->addWidget(WDFunc::NewCB2(this, "intercb", intersl));
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, &QPushButton::clicked, this, &ConnectDialog::setInterface);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, &QAbstractButton::clicked, this, &QDialog::close);
    // connect(pb, &QPushButton::clicked, this, &ConnectDialog::Cancelled);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

void ConnectDialog::setInterface()
{
    auto comboBox = this->findChild<QComboBox *>();
    Board::GetInstance().setProperty("interface", comboBox->currentText());

    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        m_idialog = new InterfaceUSBDialog(this);
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
        m_idialog = new InterfaceEthernetDialog(this);
        break;
    }
    case Board::InterfaceType::RS485:
    {
        m_idialog = new InterfaceSerialDialog(this);
        break;
    }
    default:
    {
        return;
    }
    }
    connect(m_idialog, &AbstractInterfaceDialog::accepted, this, &ConnectDialog::accepted);
    //      connect(m_idialog, &AbstractInterfaceDialog::accepted, this, &QDialog::close);
    m_idialog->setupUI();
    if (m_idialog->updateModel(m_idialog))
        m_idialog->exec();
    else
        m_idialog->deleteLater();
}

void ConnectDialog::addEth()
{
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("ethdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE2(dlg, "namele"), count++, 1, 1, 7);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "IP:"), count, 0, 1, 1, Qt::AlignLeft);
    for (int i = 0; i < 4; ++i)
    {
        lyout->addWidget(WDFunc::NewLE2(dlg, "iple." + QString::number(i)), count, (i * 2 + 1), 1, 1);
        if (i != 3)
            lyout->addWidget(WDFunc::NewLBL2(dlg, "."), count, (i * 2 + 2), 1, 1);
    }
    ++count;
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Адрес БС:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewSPB2(dlg, "bsadrspb", 1, 255, 0), count++, 1, 1, 7);
    lyout->addWidget(WDFunc::NewPB(dlg, "acceptpb", "Сохранить", this, &ConnectDialog::ethAccepted), count, 0, 1, 4);
    lyout->addWidget(WDFunc::NewPB(dlg, "cancelpb", "Отмена", [=] { dlg->close(); }), count, 4, 1, 3);
    dlg->setLayout(lyout);
    dlg->exec();
}

void ConnectDialog::ethAccepted()
{
    QDialog *dlg = this->findChild<QDialog *>("ethdlg");
    if (dlg == nullptr)
        return;
    QString name = WDFunc::LEData(dlg, "namele");
    // check if there's such name in registry
    if (isKeyExist("Ethernet-", name))
    {
        QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
        return;
    }
    QString ipstr = WDFunc::LEData(dlg, "iple.0") + "." + WDFunc::LEData(dlg, "iple.1") + "."
        + WDFunc::LEData(dlg, "iple.2") + "." + WDFunc::LEData(dlg, "iple.3");
    qDebug() << name;
    rotateSettings("Ethernet-", name);
    QString key = QCoreApplication::applicationName();
    key += "\\" + name;
    auto settings = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));
    settings->setValue("ip", ipstr);
    int spbdata;
    WDFunc::SPBData(dlg, "bsadrspb", spbdata);
    settings->setValue("bs", QString::number(spbdata));
    QDialog *dlg2 = this->findChild<QDialog *>("connectdlg");
    if (dlg2 == nullptr)
        return;
    if (updateModel(dlg2))
        qCritical() << Error::GeneralError;
    dlg->close();
}

void ConnectDialog::rsAccepted()
{
    QDialog *dlg = this->findChild<QDialog *>("rsdlg");
    if (dlg == nullptr)
        return;
    QString name = WDFunc::LEData(dlg, "namele");
    // check if there's such name in registry
    if (isKeyExist("RS485-", name))
    {
        QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
        return;
    }
    rotateSettings("RS485-", name);
    QString key = QCoreApplication::applicationName();
    key += "\\" + name;
    auto settings = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));
    settings->setValue("port", WDFunc::CBData(dlg, "portcb"));
    settings->setValue("speed", WDFunc::CBData(dlg, "speedcb"));
    settings->setValue("parity", WDFunc::CBData(dlg, "paritycb"));
    settings->setValue("stop", WDFunc::CBData(dlg, "stopbitcb"));
    int spbdata;
    WDFunc::SPBData(dlg, "addressspb", spbdata);
    settings->setValue("address", QString::number(spbdata));
    QDialog *dlg2 = this->findChild<QDialog *>("connectdlg");
    if (dlg2 == nullptr)
        return;
    if (!updateModel(dlg2))
        qCritical() << Error::GeneralError;
    dlg->close();
}

void ConnectDialog::setEth(QModelIndex index)
{
    auto *mdl = index.model();
    int row = index.row();
    QString name = mdl->data(mdl->index(row, 0)).toString();
    IEC104Settings settings;
    settings.ip = mdl->data(mdl->index(row, 1)).toString();
    settings.baseadr = mdl->data(mdl->index(row, 2)).toUInt();
    ConnectStruct st { name, settings };
    emit accepted(st);
}

void ConnectDialog::handlePing()
{
    QFutureWatcher<quint32> *watcher = static_cast<QFutureWatcher<quint32> *>(sender());
    Q_ASSERT(watcher);
    if (watcher->result())
    {
        quint32 ip_addr = watcher->result();
        m_hosts.append(ip_addr);
        qDebug() << QHostAddress(m_hosts.last());
    }
    watcher->deleteLater();
}

void ConnectDialog::handlePingFinish()
{
    createPortTask();
}

void ConnectDialog::handlePortFinish()
{
    m_hosts.removeAll(0);
    for (const auto &host : qAsConst(m_hosts))
    {
        qDebug() << QHostAddress(host);
    }
    QTableView *tv = this->findChild<QTableView *>();
    QStandardItemModel *mdl = qobject_cast<QStandardItemModel *>(tv->model());
    mdl->removeRows(0, mdl->rowCount());
    for (const auto &host : qAsConst(m_hosts))
    {
        QList<QStandardItem *> row { new QStandardItem("AVM"), new QStandardItem(QHostAddress(host).toString()),
            new QStandardItem("205") };
        mdl->appendRow(row);
    }
    emit modelUpdated();
}

void ConnectDialog::createPingTask(quint32 ip)
{
    QFutureWatcher<quint32> *watcher = new QFutureWatcher<quint32>(this);

    connect(watcher, &QFutureWatcher<quint32>::finished, this, &ConnectDialog::handlePing);
    connect(watcher, &QFutureWatcher<quint32>::canceled, &QObject::deleteLater);
    connect(watcher, &QFutureWatcher<quint32>::finished, [this]() {
        m_progress->setValue(m_progress->value() + 1);
        if (m_progress->value() == -1)
            emit pingFinished();
    });
    QFuture<quint32> future = QtConcurrent::run(&StdFunc::ping, ip);
    watcher->setFuture(future);
}

void ConnectDialog::createPortTask()
{
    QFutureWatcher<QList<quint32>> *watcher = new QFutureWatcher<QList<quint32>>;
    quint16 port = 2404;

    m_progress->setLabelText("Port scanning");
    connect(watcher, &QFutureWatcher<QList<quint32>>::progressRangeChanged, m_progress, &QProgressDialog::setRange);
    connect(watcher, &QFutureWatcher<QList<quint32>>::progressValueChanged, m_progress, &QProgressDialog::setValue);
    QFuture<QList<quint32>> future;
    future = QtConcurrent::mappedReduced(m_hosts,
        std::bind(qOverload<quint32, quint16>(&StdFunc::checkPort), std::placeholders::_1, port),
        &StdFunc::joinItem<quint32>);
    watcher->setFuture(future);
    future.waitForFinished();
    m_hosts = future.result();
    watcher->deleteLater();
    handlePortFinish();
}

void ConnectDialog::scanEth()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);
    connect(this, &ConnectDialog::modelUpdated, button, &QPushButton::show);
    bool ok;
    QString text = QInputDialog::getText(
        this, tr("Scanner"), tr("Ip address and mask:"), QLineEdit::Normal, "172.16.29.0/24", &ok);
    QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(text);
    if (!ok || text.isEmpty() || (subnet.second == -1))
        return;
    button->hide();
    m_hosts.clear();

    qDebug() << subnet.first << subnet.second;
    quint32 addr_count = std::pow(2, 32 - subnet.second);
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");

    m_progress = new QProgressDialog(dlg);
    m_progress->setLabelText("Scanning ping");
    m_progress->setMinimumDuration(0);
    m_progress->setRange(0, addr_count - 1);

    connect(this, &ConnectDialog::pingFinished, &ConnectDialog::handlePingFinish);
    for (quint32 i = 0; i < addr_count; ++i)
    {
        createPingTask(subnet.first.toIPv4Address() + i);
    }
}

void ConnectDialog::addRs()
{
    QStringList ports;
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : portlist)
        ports << info.portName();
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("rsdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE2(dlg, "namele"), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB2(dlg, "portcb", ports), count++, 1, 1, 1);
    QStringList sl { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB2(dlg, "speedcb", sl), count++, 1, 1, 1);
    sl = QStringList() << "Нет"
                       << "Нечет"
                       << "Чет";
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB2(dlg, "paritycb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList() << "1"
                       << "2";
    lyout->addWidget(WDFunc::NewCB2(dlg, "stopbitcb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewSPB2(dlg, "addressspb", 1, 255, 0), count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(dlg, "acceptpb", "Сохранить", this, &ConnectDialog::rsAccepted));
    hlyout->addWidget(WDFunc::NewPB(dlg, "cancelpb", "Отмена", [dlg] { dlg->close(); }));

    lyout->addLayout(hlyout, count, 0, 1, 2, Qt::AlignCenter);
    dlg->setLayout(lyout);
    dlg->exec();
}

void ConnectDialog::setRs(QModelIndex index)
{

    auto *mdl = index.model();
    int row = index.row();

    QString name = mdl->data(mdl->index(row, 0)).toString();
    SerialPortSettings settings;
    settings.Port = mdl->data(mdl->index(row, 1)).toString();
    settings.Baud = mdl->data(mdl->index(row, 2)).toUInt();
    settings.Parity = mdl->data(mdl->index(row, 3)).toString();
    settings.Stop = mdl->data(mdl->index(row, 4)).toString();
    settings.Address = mdl->data(mdl->index(row, 5)).toUInt();
    ConnectStruct st { name, settings };
    emit accepted(st);
}

void ConnectDialog::rotateSettings(const QString &type, const QString &name)
{
    auto settings = std::unique_ptr<QSettings>(new QSettings);
    QStringList sl;
    QString namename, oldnamename;
    // 1. get all type+'i' from registry (count)
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        namename = type + QString::number(i);
        QString value = settings->value(namename, "").toString();
        // QString value = sets->value(namename, "").toString();
        if (!value.isEmpty())
            sl << value;
    }
    // 2. if count < 5 then return count
    if (sl.size() < MAXREGISTRYINTERFACECOUNT)
        sl.append("");
    else
    {
        // 3. else delete group "type + sl.size()-1"
        namename = type + QString::number(sl.size() - 1);
        settings->remove(settings->value(namename, "").toString());
    }
    // and rotate it (1->0, 2->1 etc)
    for (int i = (sl.size() - 1); i > 0; --i)
    {
        oldnamename = type + QString::number(i);
        namename = type + QString::number(i - 1);
        settings->setValue(oldnamename, settings->value(namename, ""));
    }
    namename = type + "0";
    settings->setValue(namename, name);
}

bool ConnectDialog::isKeyExist(const QString &type, const QString &chstr)
{
    auto settings = std::unique_ptr<QSettings>(new QSettings);
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString key = type + QString::number(i);
        if (settings->value(key, "").toString() == chstr)
            return true;
    }
    return false;
}

bool ConnectDialog::updateModel(QDialog *dlg)
{

    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        return updateUModel(dlg);
    }
    case Board::InterfaceType::Ethernet:
    {
        return updateEModel(dlg);
    }
    case Board::InterfaceType::RS485:
    {
        return updateRModel(dlg);
    }
    default:
        return false;
    }
    return true;
}

bool ConnectDialog::updateUModel(QDialog *dlg)
{
    auto usbDevices = UsbHidPortInfo::devicesFound();
    if (usbDevices.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка", "Устройства не найдены");
        qCritical() << Error::Msg::NoDeviceError;
        return false;
    }
    QStringList sl { "VID", "PID", "Serial" };
#if _DEBUG
    sl.push_back("Path");
#endif
    QStandardItemModel *mdl = new QStandardItemModel(dlg);

    mdl->setHorizontalHeaderLabels(sl);
    for (const auto &row : usbDevices)
    {
        QList<QStandardItem *> device
        {
            new QStandardItem(QString::number(row.vendor_id, 16)),      //
                new QStandardItem(QString::number(row.product_id, 16)), //
                new QStandardItem(row.serial),                          //
#if _DEBUG
                new QStandardItem(row.path) //
#endif
        };
        mdl->appendRow(device);
    }
    WDFunc::SetQTVModel(dlg, "usbtv", mdl);
    return true;
}

bool ConnectDialog::updateRModel(QDialog *dlg)
{
    QStringList rslist;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        auto sets = std::unique_ptr<QSettings>(new QSettings);
        QString rsname = "RS485-" + QString::number(i);
        rslist << sets->value(rsname, "").toString();
    }
    QStringList sl { "Имя", "Порт", "Скорость", "Четность", "Стоп бит", "Адрес" };
    QStandardItemModel *mdl = new QStandardItemModel(dlg);
    mdl->setHorizontalHeaderLabels(sl);
    for (const auto &item : qAsConst(rslist))
    {
        QString key = QCoreApplication::applicationName();
        key += "\\" + item;
        auto sets = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));
        QList<QStandardItem *> items { new QStandardItem(item), new QStandardItem(sets->value("port", "").toString()),
            new QStandardItem(sets->value("speed", "").toString()),
            new QStandardItem(sets->value("parity", "").toString()),
            new QStandardItem(sets->value("stop", "").toString()),
            new QStandardItem(sets->value("address", "").toString()) };
        mdl->appendRow(items);
    }
    WDFunc::SetQTVModel(dlg, "rstv", mdl);
    return true;
}

bool ConnectDialog::updateEModel(QDialog *dlg)
{
    QStringList ethlist;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        auto sets = std::unique_ptr<QSettings>(new QSettings);
        QString ethname = "Ethernet-" + QString::number(i);
        ethlist << sets->value(ethname, "").toString();
    }
    QStringList sl { "Имя", "IP", "Адрес БС" };
    QStandardItemModel *mdl = new QStandardItemModel(dlg);
    mdl->setHorizontalHeaderLabels(sl);
    for (const auto &item : qAsConst(ethlist))
    {
        QString key = QCoreApplication::applicationName();
        key += "\\" + item;
        auto sets = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));
        QList<QStandardItem *> items { new QStandardItem(item), new QStandardItem(sets->value("ip", "").toString()),
            new QStandardItem(sets->value("bs", "").toString()) };
        mdl->appendRow(items);
    }
    WDFunc::SetQTVModel(dlg, "ethtv", mdl);
    return true;
}
