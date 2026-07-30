#include "dialogs/interfaceethernetdialog.h"

#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QtNetwork/QHostAddress>
#include <libavm-gen/error.h>
#include <libavm-gen/settings.h>
#include <libavm-gen/stdfunc.h>
#include <libavm-widgets/emessagebox.h>
#include <libavm-widgets/lefunc.h>
#include <libavm-widgets/pbfunc.h>
#include <libavm-widgets/spbfunc.h>
#include <libavm-widgets/tvfunc.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <qsizegrip.h>

InterfaceEthernetDialog::InterfaceEthernetDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
    setWindowTitle("Ethernet соединения");
    setMinimumSize(QSize(400, 350));
}

InterfaceEthernetDialog::~InterfaceEthernetDialog() noexcept { }

void InterfaceEthernetDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    m_tableView = TVFunc::New(this, "", nullptr);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lyout->addWidget(m_tableView);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(PBFunc::New(this, "", tr("Добавить"), this, &InterfaceEthernetDialog::addInterface));
    hlyout->addWidget(PBFunc::New(this, "", tr("Сканировать"), this, &InterfaceEthernetDialog::scanInterface));
    hlyout->addWidget(PBFunc::New(this, "", tr("Удалить"), this,
        [this]
        {
            auto name = m_tableView->currentIndex().siblingAtColumn(0).data().toString();
            Settings::pushGroup("Ethernet");
            Settings::remove(name);
            Settings::popGroup();
            updateModel();
        }));
    lyout->addLayout(hlyout);
    setLayout(lyout);
    connect(m_tableView, &QTableView::doubleClicked, this, &InterfaceEthernetDialog::setInterface);
    AbstractInterfaceDialog::setupUI();
}

void InterfaceEthernetDialog::setInterface(QModelIndex index)
{
    auto *mdl = index.model();
    int row = index.row();
    QString name = mdl->data(mdl->index(row, 0)).toString();
    IEC104Settings *settings = new IEC104Settings;
    settings->set("ip", mdl->data(mdl->index(row, 1)).toString());
    settings->set("port", mdl->data(mdl->index(row, 2)).toUInt());
    settings->set("bsAddress", mdl->data(mdl->index(row, 3)).toUInt());
    settings->set("timeout", Settings::get("iec104Timeout", 1000));
    settings->set("reconnectInterval", Settings::get("iec104Reconnect", 1000));
    settings->set("disconnectTimeout", Settings::get("iec104DisconnectTimeout", 5000));
    settings->set("connectTimeout", Settings::get("iec104ConnectTimeout", 5000));
    settings->set("t0", Settings::get("iec104T0", 30));
    settings->set("t1", Settings::get("iec104T1", 15));
    settings->set("t2", Settings::get("iec104T2", 10));
    settings->set("t3", Settings::get("iec104T3", 20));
    settings->set("k", Settings::get("iec104K", 12));
    settings->set("w", Settings::get("iec104W", 8));
    apply(settings);

    if (!settings->isValid())
        return;
    ConnectionSettings st { name, settings };
    emit accepted(st);
}

void InterfaceEthernetDialog::addInterface()
{
    if (checkSize())
    {
        EMessageBox::warning(this, "Превышен лимит соединений!");
        return;
    }

    auto dialog = new QDialog(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;

    dialog->setObjectName("ethCreateDialog");
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowFlag(Qt::FramelessWindowHint);

    QHBoxLayout *hlayout = new QHBoxLayout;
    QLabel *lbl = new QLabel(tr("Имя:"), dialog);
    hlayout->addWidget(lbl);
    QLineEdit *le = new QLineEdit(dialog);
    le->setObjectName("namele");
    hlayout->addWidget(le);
    mainLayout->addLayout(hlayout);
    hlayout = new QHBoxLayout;
    lbl = new QLabel("IP:", dialog);
    hlayout->addWidget(lbl);
    for (int i = 0; i < 4; ++i)
    {
        auto ipCell = SPBFunc::New(dialog, QString("iple_%1").arg(i), 0, 255, 0);
        hlayout->addWidget(ipCell);
        if (i != 3)
        {
            lbl = new QLabel(".", this);
            hlayout->addWidget(lbl);
        }
    }
    mainLayout->addLayout(hlayout);
    constexpr auto u16min = std::numeric_limits<quint16>::min();
    constexpr auto u16max = std::numeric_limits<quint16>::max();

    hlayout = new QHBoxLayout;
    lbl = new QLabel(tr("Порт:"), dialog);
    hlayout->addWidget(lbl);
    auto portspb = SPBFunc::New(dialog, "portspb", u16min, u16max, 0);
    /// TODO: Значение 2404 должно быть настраиваемым значением
    portspb->setValue(2404);
    hlayout->addWidget(portspb);
    mainLayout->addLayout(hlayout);

    hlayout = new QHBoxLayout;
    lbl = new QLabel(tr("Адрес БС:"), dialog);
    hlayout->addWidget(lbl);
    auto bsaddrspb = SPBFunc::New(dialog, "bsaddrspb", u16min, u16max, 0);
    /// TODO: Значение 205 должно быть настраиваемым значением
    bsaddrspb->setValue(205);
    hlayout->addWidget(bsaddrspb);
    mainLayout->addLayout(hlayout);

    hlayout = new QHBoxLayout;
    hlayout->addWidget(PBFunc::New(dialog, "", tr("Сохранить"), this, &InterfaceEthernetDialog::acceptedInterface));
    hlayout->addWidget(PBFunc::New(dialog, "", tr("Отмена"), dialog, [dialog] { dialog->close(); }));
    mainLayout->addLayout(hlayout);
    dialog->setLayout(mainLayout);
    dialog->adjustSize();
    dialog->setSizeGripEnabled(false);
    dialog->exec();
}

void InterfaceEthernetDialog::scanInterface()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);
    connect(this, &InterfaceEthernetDialog::modelUpdated, button, &QPushButton::show);

    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("Сканер"));
    dialog.setLabelText(tr("Введите IP адрес и маску"));
    dialog.setTextValue("172.16.29.0/24");
    dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dialog.setSizeGripEnabled(false);
    dialog.setStyleSheet("QSizeGrip { width: 0px; height: 0px; }");

    bool ok = dialog.exec() == QDialog::Accepted;
    QString text = ok ? dialog.textValue() : QString();
    if (!ok || text.isEmpty())
        return;

    QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(text);
    if (subnet.second == -1)
    {
        EMessageBox::error(this, "Ошибка ввода адреса или маски");
        return;
    }

    m_hosts.clear();

    qDebug() << subnet.first << subnet.second;
    int addr_count = std::pow(2, 32 - subnet.second);

    m_progress = new QProgressDialog(this);
    m_progress->setLabelText(tr("Сканирование адресов"));
    m_progress->setMinimumDuration(0);
    m_progress->setRange(0, addr_count);
    m_progress->setModal(true);
    m_progress->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_progress->setSizeGripEnabled(false);
    m_progress->setValue(0);

    connect(this, &InterfaceEthernetDialog::pingFinished, this, &InterfaceEthernetDialog::handlePingFinish,
        Qt::UniqueConnection);

    for (int i = 0; i < addr_count; ++i)
    {
        createPingTask(subnet.first.toIPv4Address() + i);
    }
}

void InterfaceEthernetDialog::acceptedInterface()
{
    auto dialog = this->findChild<QDialog *>("ethCreateDialog");
    if (dialog == nullptr)
        return;
    QString name = LEFunc::data(dialog, "namele");
    // check if there's such name in registry
    Settings::pushGroup("Ethernet");
    if (Settings::groupExist(name))
    {
        EMessageBox::error(this, "Такое имя уже имеется");
        Settings::popGroup();
        return;
    }

    QString ipstr =                                                   //
        QString::number(SPBFunc::data<int>(dialog, "iple_0")) + "." + //
        QString::number(SPBFunc::data<int>(dialog, "iple_1")) + "." + //
        QString::number(SPBFunc::data<int>(dialog, "iple_2")) + "." + //
        QString::number(SPBFunc::data<int>(dialog, "iple_3"));        //
    auto port = SPBFunc::data<quint16>(dialog, "portspb");
    auto bsAddress = SPBFunc::data<quint16>(dialog, "bsaddrspb");
    if (bsAddress == 0)
    {
        EMessageBox::error(this, "Адрес базовой станции не может быть равен нулю");
        Settings::popGroup();
        return;
    }
    Settings::pushGroup(name);
    Settings::set("ipAddress", ipstr);
    Settings::set("ipPort", port);
    Settings::set("iec104BsAddress", bsAddress);
    Settings::popGroup();
    Settings::popGroup(); // exit from Ethernet
    if (!updateModel())
        qDebug() << Error::GeneralError;
    dialog->close();
}

void InterfaceEthernetDialog::handlePing()
{
    QFutureWatcher<quint32> *watcher = static_cast<QFutureWatcher<quint32> *>(sender());
    Q_ASSERT(watcher);

    if (watcher->isCanceled())
    {
        watcher->deleteLater();
        return;
    }

    if (watcher->result())
    {
        quint32 ip_addr = watcher->result();
        m_hosts.append(ip_addr);
        qDebug() << QHostAddress(m_hosts.last());
    }
    watcher->deleteLater();
}

void InterfaceEthernetDialog::handlePingFinish()
{
    if (m_progress && m_progress->wasCanceled())
        return;

    createPortTask();
}

void InterfaceEthernetDialog::handlePortFinish()
{
    m_hosts.removeAll(0);
    for (const auto &host : std::as_const(m_hosts))
    {
        qDebug() << QHostAddress(host);
    }
    QStandardItemModel *mdl = qobject_cast<QStandardItemModel *>(m_tableView->model());
    mdl->removeRows(0, mdl->rowCount());
    for (const auto &host : std::as_const(m_hosts))
    {
        QList<QStandardItem *> row { new QStandardItem("AVM"), new QStandardItem(QHostAddress(host).toString()),
            new QStandardItem("2404"), new QStandardItem("205") };
        mdl->appendRow(row);
    }
    emit modelUpdated();
}

void InterfaceEthernetDialog::createPingTask(quint32 ip)
{
    QFutureWatcher<quint32> *watcher = new QFutureWatcher<quint32>(this);

    connect(m_progress, &QProgressDialog::canceled, watcher, &QFutureWatcher<quint32>::cancel);
    connect(watcher, &QFutureWatcher<quint32>::finished, this, &InterfaceEthernetDialog::handlePing);
    connect(watcher, &QFutureWatcher<quint32>::finished, watcher, &QObject::deleteLater);
    connect(watcher, &QFutureWatcher<quint32>::canceled, &QObject::deleteLater);
    connect(watcher, &QFutureWatcher<quint32>::finished, this,
        [this]
        {
            if (m_progress && m_progress->wasCanceled())
                return;

            int newValue = m_progress->value() + 1;
            m_progress->setValue(newValue);

            if (newValue == m_progress->maximum())
            {
                emit pingFinished();
            }
        });

    QFuture<quint32> future = QtConcurrent::run(&StdFunc::Ping, ip);
    watcher->setFuture(future);
}

void InterfaceEthernetDialog::createPortTask()
{
    QFutureWatcher<QList<quint32>> *watcher = new QFutureWatcher<QList<quint32>>(this);
    quint16 port = 2404;
    m_progress->setLabelText(tr("Сканирование портов"));
    m_progress->setRange(0, m_hosts.size());
    m_progress->setValue(0);

    connect(m_progress, &QProgressDialog::canceled, watcher, &QFutureWatcher<QList<quint32>>::cancel);
    connect(watcher, &QFutureWatcher<QList<quint32>>::progressValueChanged, m_progress, &QProgressDialog::setValue);
    connect(watcher, &QFutureWatcher<QList<quint32>>::finished, this,
        [this, watcher]()
        {
            if (watcher->isCanceled() || (m_progress && m_progress->wasCanceled()))
            {
                watcher->deleteLater();
                return;
            }

            m_hosts = watcher->result();
            watcher->deleteLater();
            handlePortFinish();
        });

    QFuture<QList<quint32>> future = QtConcurrent::mappedReduced(m_hosts,
        std::bind(qOverload<quint32, quint16>(&StdFunc::CheckPort), std::placeholders::_1, port),
        &StdFunc::joinItem<quint32>);
    watcher->setFuture(future);
}

bool InterfaceEthernetDialog::updateModel()
{
    QStringList headers { "Имя", "IP", "Порт", "Адрес БС" };
    auto model = static_cast<QStandardItemModel *>(m_tableView->model());
    if (model == nullptr)
        model = new QStandardItemModel(this);
    else
        model->clear();
    model->setHorizontalHeaderLabels(headers);

    Settings::pushGroup("Ethernet");
    QStringList ethList = Settings::groups();
    for (const auto &item : std::as_const(ethList))
    {
        Settings::pushGroup(item);
        QList<QStandardItem *> items {
            new QStandardItem(item),                                             //
            new QStandardItem(QString(Settings::get("ipAddress", "127.0.0.1"))), //
            new QStandardItem(QString(Settings::get("ipPort", 2404))),           //
            new QStandardItem(QString(Settings::get("iec104BsAddress", 205)))    //
        };
        model->appendRow(items);
        Settings::popGroup();
    }
    Settings::popGroup();

    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();
    return true;
}
