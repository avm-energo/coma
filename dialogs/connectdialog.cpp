#include "connectdialog.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../usb/eprotocom.h"
#include "../widgets/wd_func.h"

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

ConnectDialog::ConnectDialog()
{
    QStringList intersl { "USB", "Ethernet", "RS485" };
    setMinimumWidth(150);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Выберите интерфейс связи"));
    lyout->addWidget(WDFunc::NewCB(this, "intercb", intersl));
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, &QPushButton::clicked, this, &ConnectDialog::SetInterface);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, &QPushButton::clicked, this, &ConnectDialog::Cancelled);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

void ConnectDialog::SetInterface()
{
    auto comboBox = this->findChild<EComboBox *>();
    Board::GetInstance().setProperty("interface", comboBox->currentText());
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    dlg->setMinimumWidth(400);
    QVBoxLayout *lyout = new QVBoxLayout;

    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        auto *table = WDFunc::NewQTV(dlg, "usbtv", nullptr);
        lyout->addWidget(table);
        connect(table, &QTableView::doubleClicked, this, qOverload<QModelIndex>(&ConnectDialog::SetUsb));
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
        auto *table = WDFunc::NewQTV(dlg, "ethtv", nullptr);
        lyout->addWidget(table);
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addStretch(10);
        hlyout->addWidget(WDFunc::NewPB2(dlg, "newethpb", "Добавить", this, &ConnectDialog::AddEth));
        hlyout->addWidget(WDFunc::NewPB2(dlg, "scanethpb", "Сканировать", this, &ConnectDialog::ScanEth));
        hlyout->addStretch(10);
        lyout->addLayout(hlyout);
        connect(table, &QTableView::doubleClicked, this, qOverload<QModelIndex>(&ConnectDialog::SetEth));
        break;
    }
    case Board::InterfaceType::RS485:
    {
        auto *table = WDFunc::NewQTV(dlg, "rstv", nullptr);
        lyout->addWidget(table);
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addStretch(10);
        // hlyout->addWidget(WDFunc::NewPB(dlg, "newrspb", "Добавить", this, SLOT(AddRs())));
        hlyout->addWidget(WDFunc::NewPB2(dlg, "newrspb", "Добавить", this, &ConnectDialog::AddRs));
        // hlyout->addWidget(WDFunc::NewPB(dlg, "scanrspb", "Сканировать", this, SLOT(ScanRs())));
        hlyout->addStretch(10);
        lyout->addLayout(hlyout);
        connect(table, &QTableView::doubleClicked, this, qOverload<QModelIndex>(&ConnectDialog::SetRs));
        break;
    }
    default:
    {
        delete lyout;
        return;
    }
    }
    QPushButton *pb = new QPushButton("Отмена");
    connect(pb, &QAbstractButton::clicked, this, &ConnectDialog::SetCancelled);
    lyout->addStretch(20);
    lyout->addWidget(pb);
    lyout->addStretch(20);
    dlg->setLayout(lyout);

    UpdateModel();
    dlg->exec();
}

void ConnectDialog::SetUsb(QModelIndex index)
{
    ConnectStruct st; // temporary var
    if (index.isValid())
    {
        QDialog *dlg = this->findChild<QDialog *>("connectdlg");
        EProtocom::GetInstance().setDeviceName(WDFunc::TVData(dlg, "usbtv", 1).toString());
        EProtocom::GetInstance().setDevicePosition(index.row());
    }
    emit Accepted(&st);
}

void ConnectDialog::AddEth()
{
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("ethdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE(dlg, "namele"), count++, 1, 1, 7);
    lyout->addWidget(WDFunc::NewLBL(dlg, "IP:"), count, 0, 1, 1, Qt::AlignLeft);
    for (int i = 0; i < 4; ++i)
    {
        lyout->addWidget(WDFunc::NewLE(dlg, "iple." + QString::number(i)), count, (i * 2 + 1), 1, 1);
        if (i != 3)
            lyout->addWidget(WDFunc::NewLBL(dlg, "."), count, (i * 2 + 2), 1, 1);
    }
    ++count;
    lyout->addWidget(WDFunc::NewLBL(dlg, "Адрес БС:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewSPB(dlg, "bsadrspb", 1, 255, 0), count++, 1, 1, 7);
    lyout->addWidget(WDFunc::NewPB2(dlg, "acceptpb", "Сохранить", this, &ConnectDialog::EthAccepted), count, 0, 1, 4);
    lyout->addWidget(WDFunc::NewPB2(dlg, "cancelpb", "Отмена", [dlg] { dlg->close(); }), count, 4, 1, 3);
    dlg->setLayout(lyout);
    dlg->exec();
}

void ConnectDialog::EthAccepted()
{
    QDialog *dlg = this->findChild<QDialog *>("ethdlg");
    if (dlg != nullptr)
    {
        QString name = WDFunc::LEData(dlg, "namele");
        // check if there's such name in registry
        if (IsKeyExist("Ethernet-", name))
        {
            QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
            return;
        }
        QString ipstr = WDFunc::LEData(dlg, "iple.0") + "." + WDFunc::LEData(dlg, "iple.1") + "."
            + WDFunc::LEData(dlg, "iple.2") + "." + WDFunc::LEData(dlg, "iple.3");
        RotateSettings("Ethernet-", name);
        QString key = PROGNAME;
        key += "\\" + name;
        QSettings *sets = new QSettings(SOFTDEVELOPER, key);
        sets->setValue("ip", ipstr);
        int spbdata;
        WDFunc::SPBData(dlg, "bsadrspb", spbdata);
        sets->setValue("bs", QString::number(spbdata));
        QDialog *dlg2 = this->findChild<QDialog *>("connectdlg");
        if (dlg2 != nullptr)
            UpdateModel();
        dlg->close();
    }
}

void ConnectDialog::RsAccepted()
{
    QDialog *dlg = this->findChild<QDialog *>("rsdlg");
    if (dlg != nullptr)
    {
        QString name = WDFunc::LEData(dlg, "namele");
        // check if there's such name in registry
        if (IsKeyExist("RS485-", name))
        {
            QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
            return;
        }
        RotateSettings("RS485-", name);
        QString key = PROGNAME;
        key += "\\" + name;
        QSettings *sets = new QSettings(SOFTDEVELOPER, key);
        sets->setValue("port", WDFunc::CBData(dlg, "portcb"));
        sets->setValue("speed", WDFunc::CBData(dlg, "speedcb"));
        sets->setValue("parity", WDFunc::CBData(dlg, "paritycb"));
        sets->setValue("stop", WDFunc::CBData(dlg, "stopcb"));
        int spbdata;
        WDFunc::SPBData(dlg, "addressspb", spbdata);
        sets->setValue("address", QString::number(spbdata));
        QDialog *dlg2 = this->findChild<QDialog *>("connectdlg");
        if (dlg2 != nullptr)
            UpdateModel();
        dlg->close();
    }
}

void ConnectDialog::SetCancelled()
{
    emit Cancelled();
}

void ConnectDialog::SetEth()
{
    ConnectStruct st;
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (dlg != nullptr)
    {
        st.name = WDFunc::TVData(dlg, "ethtv", 1).toString();
        st.iec104st.ip = WDFunc::TVData(dlg, "ethtv", 2).toString();
        st.iec104st.baseadr = WDFunc::TVData(dlg, "ethtv", 3).toUInt();
    }
    emit Accepted(&st);
}

void ConnectDialog::SetEth(QModelIndex index)
{
    ConnectStruct st;
    auto *mdl = index.model();
    int row = index.row();
    st.name = mdl->data(mdl->index(row, 0)).toString();
    st.iec104st.ip = mdl->data(mdl->index(row, 1)).toString();
    st.iec104st.baseadr = mdl->data(mdl->index(row, 2)).toUInt();
    emit Accepted(&st);
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
    emit ModelUpdated();
}

void ConnectDialog::createPingTask(quint32 ip)
{
    QFutureWatcher<quint32> *watcher = new QFutureWatcher<quint32>(this);

    connect(watcher, &QFutureWatcher<quint32>::finished, this, &ConnectDialog::handlePing);
    connect(watcher, &QFutureWatcher<quint32>::canceled, &QObject::deleteLater);
    connect(watcher, &QFutureWatcher<quint32>::finished, [this]() {
        m_progress->setValue(m_progress->value() + 1);
        if (m_progress->value() == -1)
            emit PingFinished();
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

void ConnectDialog::ScanEth()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);
    connect(this, &ConnectDialog::ModelUpdated, button, &QPushButton::show);
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

    connect(this, &ConnectDialog::PingFinished, &ConnectDialog::handlePingFinish);
    for (quint32 i = 0; i < addr_count; ++i)
    {
        createPingTask(subnet.first.toIPv4Address() + i);
    }
}

void ConnectDialog::AddRs()
{
    QStringList ports;
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo info, portlist)
        ports << info.portName();
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("rsdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE(dlg, "namele"), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL(dlg, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB(dlg, "portcb", ports), count++, 1, 1, 1);
    QStringList sl { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    lyout->addWidget(WDFunc::NewLBL(dlg, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB(dlg, "speedcb", sl), count++, 1, 1, 1);
    sl = QStringList() << "Нет"
                       << "Нечет"
                       << "Чет";
    lyout->addWidget(WDFunc::NewLBL(dlg, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB(dlg, "paritycb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL(dlg, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList() << "1"
                       << "2";
    lyout->addWidget(WDFunc::NewCB(dlg, "stopbitcb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL(dlg, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewSPB(dlg, "addressspb", 1, 255, 0), count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB2(dlg, "acceptpb", "Сохранить", this, &ConnectDialog::RsAccepted));
    hlyout->addWidget(WDFunc::NewPB2(dlg, "cancelpb", "Отмена", [dlg] { dlg->close(); }));

    lyout->addLayout(hlyout, count, 0, 1, 2, Qt::AlignCenter);
    dlg->setLayout(lyout);
    dlg->exec();
}

void ConnectDialog::SetRs()
{
    ConnectStruct st;
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (dlg != nullptr)
    {
        st.name = WDFunc::TVData(dlg, "rstv", 1).toString();
        st.serialst.Port = WDFunc::TVData(dlg, "rstv", 2).toString();
        st.serialst.Baud = WDFunc::TVData(dlg, "rstv", 3).toUInt();
        st.serialst.Parity = WDFunc::TVData(dlg, "rstv", 4).toString();
        st.serialst.Stop = WDFunc::TVData(dlg, "rstv", 5).toString();
        st.serialst.Address = WDFunc::TVData(dlg, "rstv", 6).toUInt();
    }
    emit Accepted(&st);
}

void ConnectDialog::SetRs(QModelIndex index)
{
    ConnectStruct st;
    auto *mdl = index.model();
    int row = index.row();
    st.name = mdl->data(mdl->index(row, 0)).toString();
    st.serialst.Port = mdl->data(mdl->index(row, 1)).toString();
    st.serialst.Baud = mdl->data(mdl->index(row, 2)).toUInt();
    st.serialst.Parity = mdl->data(mdl->index(row, 3)).toString();
    st.serialst.Stop = mdl->data(mdl->index(row, 4)).toString();
    st.serialst.Address = mdl->data(mdl->index(row, 5)).toUInt();
    emit Accepted(&st);
}

void ConnectDialog::RotateSettings(const QString &type, const QString &name)
{
    QSettings *sets = new QSettings(SOFTDEVELOPER, PROGNAME);
    QStringList sl;
    QString namename, oldnamename;
    // 1. get all type+'i' from registry (count)
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        namename = type + QString::number(i);
        QString value = sets->value(namename, "").toString();
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
        sets->remove(sets->value(namename, "").toString());
    }
    // and rotate it (1->0, 2->1 etc)
    for (int i = (sl.size() - 1); i > 0; --i)
    {
        oldnamename = type + QString::number(i);
        namename = type + QString::number(i - 1);
        sets->setValue(oldnamename, sets->value(namename, ""));
    }
    namename = type + "0";
    sets->setValue(namename, name);
}

bool ConnectDialog::IsKeyExist(const QString &type, const QString &chstr)
{
    QSettings *sets = new QSettings(SOFTDEVELOPER, PROGNAME);
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString key = type + QString::number(i);
        if (sets->value(key, "").toString() == chstr)
            return true;
    }
    return false;
}

bool ConnectDialog::UpdateModel()
{
    QStringList ethlist, rslist;

    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (!dlg)
        return false;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings(SOFTDEVELOPER, PROGNAME));
        QString rsname = "RS485-" + QString::number(i);
        QString ethname = "Ethernet-" + QString::number(i);
        ethlist << sets->value(ethname, "").toString();
        rslist << sets->value(rsname, "").toString();
    }
    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        QList<QStringList> USBsl = EProtocom::GetInstance().DevicesFound();
        QStringList sl { "VID", "PID", "Serial", "Path" };
        QStandardItemModel *mdl = new QStandardItemModel(dlg);

        if (USBsl.isEmpty())
        {
            QMessageBox::critical(this, "Ошибка", "Устройства не найдены");
            qCritical() << QVariant::fromValue(Error::Msg::CN_NOPORTSERROR).toString();
            return false;
        }
        mdl->setHorizontalHeaderLabels(sl);
        for (const auto &row : USBsl)
        {
            QList<QStandardItem *> items;
            for (const auto &column : row)
            {
                items.append(new QStandardItem(column));
            }
            mdl->appendRow(items);
        }
        WDFunc::SetQTVModel(dlg, "usbtv", mdl);
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
        QStringList sl { "Имя", "IP", "Адрес БС" };
        QStandardItemModel *mdl = new QStandardItemModel(dlg);
        mdl->setHorizontalHeaderLabels(sl);
        for (const auto &item : qAsConst(ethlist))
        {
            QString key = PROGNAME;
            key += "\\" + item;
            QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings(SOFTDEVELOPER, key));
            QList<QStandardItem *> items { new QStandardItem(item), new QStandardItem(sets->value("ip", "").toString()),
                new QStandardItem(sets->value("bs", "").toString()) };
            mdl->appendRow(items);
        }
        WDFunc::SetQTVModel(dlg, "ethtv", mdl);
        break;
    }
    case Board::InterfaceType::RS485:
    {
        QStringList sl { "Имя", "Порт", "Скорость", "Четность", "Стоп бит", "Адрес" };
        QStandardItemModel *mdl = new QStandardItemModel(dlg);
        mdl->setHorizontalHeaderLabels(sl);
        for (const auto &item : qAsConst(rslist))
        {
            QString key = PROGNAME;
            key += "\\" + item;
            QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings(SOFTDEVELOPER, key));
            QList<QStandardItem *> items { new QStandardItem(item),
                new QStandardItem(sets->value("port", "").toString()),
                new QStandardItem(sets->value("speed", "").toString()),
                new QStandardItem(sets->value("parity", "").toString()),
                new QStandardItem(sets->value("stop", "").toString()),
                new QStandardItem(sets->value("address", "").toString()) };
            mdl->appendRow(items);
        }
        WDFunc::SetQTVModel(dlg, "rstv", mdl);
        break;
    }
    default:
        return false;
    }
    return true;
}
