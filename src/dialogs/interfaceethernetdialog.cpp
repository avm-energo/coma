#include "interfaceethernetdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QtNetwork/QHostAddress>
#include <gen/error.h>
#include <gen/stdfunc.h>

InterfaceEthernetDialog::InterfaceEthernetDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
    m_settings.beginGroup("Ethernet");
    setWindowTitle("Ethernet соединения");
}

void InterfaceEthernetDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    m_tableView = WDFunc::NewQTV(this, "", nullptr);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lyout->addWidget(m_tableView);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Добавить"), this, &InterfaceEthernetDialog::addInterface));
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Сканировать"), this, &InterfaceEthernetDialog::scanInterface));
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Удалить"), this, [this] {
        auto name = m_tableView->currentIndex().siblingAtColumn(0).data().toString();
        removeConnection(name);
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
    IEC104Settings settings;
    settings.ip = mdl->data(mdl->index(row, 1)).toString();
    settings.port = mdl->data(mdl->index(row, 2)).toUInt();
    settings.baseadr = mdl->data(mdl->index(row, 3)).toUInt();
    if (!settings.isValid())
        return;
    ConnectStruct st { name, settings };
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
        auto ipCell = WDFunc::NewSPB2(dialog, QString("iple_%1").arg(i), 0, 255, 0);
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
    auto portspb = WDFunc::NewSPB2(dialog, "portspb", u16min, u16max, 0);
    /// TODO: Значение 2404 должно быть настраиваемым значением
    portspb->setValue(2404);
    hlayout->addWidget(portspb);
    mainLayout->addLayout(hlayout);

    hlayout = new QHBoxLayout;
    lbl = new QLabel(tr("Адрес БС:"), dialog);
    hlayout->addWidget(lbl);
    auto bsaddrspb = WDFunc::NewSPB2(dialog, "bsaddrspb", u16min, u16max, 0);
    /// TODO: Значение 205 должно быть настраиваемым значением
    bsaddrspb->setValue(205);
    hlayout->addWidget(bsaddrspb);
    mainLayout->addLayout(hlayout);

    hlayout = new QHBoxLayout;
    hlayout->addWidget(WDFunc::NewPB(dialog, "", tr("Сохранить"), this, &InterfaceEthernetDialog::acceptedInterface));
    hlayout->addWidget(WDFunc::NewPB(dialog, "", tr("Отмена"), dialog, [dialog] { dialog->close(); }));
    mainLayout->addLayout(hlayout);
    dialog->setLayout(mainLayout);
    dialog->adjustSize();
    dialog->exec();
}

void InterfaceEthernetDialog::scanInterface()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);
    connect(this, &InterfaceEthernetDialog::modelUpdated, button, &QPushButton::show);
    bool ok = false;
    QString text = QInputDialog::getText(
        this, tr("Scanner"), tr("Ip address and mask:"), QLineEdit::Normal, "172.16.29.0/24", &ok);
    QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(text);
    if (!ok || text.isEmpty() || (subnet.second == -1))
        return;
    button->hide();
    m_hosts.clear();

    qDebug() << subnet.first << subnet.second;
    int addr_count = std::pow(2, 32 - subnet.second);

    m_progress = new QProgressDialog(this);
    m_progress->setLabelText(tr("Scanning ping"));
    m_progress->setMinimumDuration(0);
    m_progress->setRange(0, addr_count - 1);

    connect(this, &InterfaceEthernetDialog::pingFinished, &InterfaceEthernetDialog::handlePingFinish);
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
    QString name = WDFunc::LEData(dialog, "namele");
    // check if there's such name in registry
    if (isConnectionExist(name))
    {
        EMessageBox::error(this, "Такое имя уже имеется");
        return;
    }

    QString ipstr =                                                     //
        QString::number(WDFunc::SPBData<int>(dialog, "iple_0")) + "." + //
        QString::number(WDFunc::SPBData<int>(dialog, "iple_1")) + "." + //
        QString::number(WDFunc::SPBData<int>(dialog, "iple_2")) + "." + //
        QString::number(WDFunc::SPBData<int>(dialog, "iple_3"));        //
    auto port = WDFunc::SPBData<quint16>(dialog, "portspb");
    auto bsAddress = WDFunc::SPBData<quint16>(dialog, "bsaddrspb");
    if (bsAddress == 0)
    {
        EMessageBox::error(this, "Адрес базовой станции не может быть равен нулю");
        return;
    }

    m_settings.beginGroup(name);
    m_settings.setValue("ipAddress", ipstr);
    m_settings.setValue("port", QString::number(port));
    m_settings.setValue("bsAddress", QString::number(bsAddress));
    m_settings.endGroup();

    if (!updateModel())
        qCritical() << Error::GeneralError;
    dialog->close();
}

void InterfaceEthernetDialog::handlePing()
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

void InterfaceEthernetDialog::handlePingFinish()
{
    createPortTask();
}

void InterfaceEthernetDialog::handlePortFinish()
{
    m_hosts.removeAll(0);
    for (const auto &host : qAsConst(m_hosts))
    {
        qDebug() << QHostAddress(host);
    }
    QStandardItemModel *mdl = qobject_cast<QStandardItemModel *>(m_tableView->model());
    mdl->removeRows(0, mdl->rowCount());
    for (const auto &host : qAsConst(m_hosts))
    {
        QList<QStandardItem *> row { new QStandardItem("AVM"), new QStandardItem(QHostAddress(host).toString()),
            new QStandardItem("205") };
        mdl->appendRow(row);
    }
    emit modelUpdated();
}

void InterfaceEthernetDialog::createPingTask(quint32 ip)
{
    QFutureWatcher<quint32> *watcher = new QFutureWatcher<quint32>(this);

    connect(watcher, &QFutureWatcher<quint32>::finished, this, &InterfaceEthernetDialog::handlePing);
    connect(watcher, &QFutureWatcher<quint32>::canceled, &QObject::deleteLater);
    connect(watcher, &QFutureWatcher<quint32>::finished, this, [this] {
        m_progress->setValue(m_progress->value() + 1);
        if (m_progress->value() == -1)
            emit pingFinished();
    });
    QFuture<quint32> future = QtConcurrent::run(&StdFunc::Ping, ip);
    watcher->setFuture(future);
}

void InterfaceEthernetDialog::createPortTask()
{
    QFutureWatcher<QList<quint32>> *watcher = new QFutureWatcher<QList<quint32>>;
    quint16 port = 2404;

    m_progress->setLabelText(tr("Port scanning"));
    connect(watcher, &QFutureWatcher<QList<quint32>>::progressRangeChanged, m_progress, &QProgressDialog::setRange);
    connect(watcher, &QFutureWatcher<QList<quint32>>::progressValueChanged, m_progress, &QProgressDialog::setValue);
    QFuture<QList<quint32>> future;
    future = QtConcurrent::mappedReduced(m_hosts,
        std::bind(qOverload<quint32, quint16>(&StdFunc::CheckPort), std::placeholders::_1, port),
        &StdFunc::joinItem<quint32>);
    watcher->setFuture(future);
    future.waitForFinished();
    m_hosts = future.result();
    watcher->deleteLater();
    handlePortFinish();
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

    QStringList ethList = m_settings.childGroups();
    for (const auto &item : qAsConst(ethList))
    {
        m_settings.beginGroup(item);
        QList<QStandardItem *> items {
            new QStandardItem(item),                                                  //
            new QStandardItem(m_settings.value("ipAddress", "127.0.0.1").toString()), //
            new QStandardItem(m_settings.value("port", "2404").toString()),           //
            new QStandardItem(m_settings.value("bsAddress", "205").toString())        //
        };
        model->appendRow(items);
        m_settings.endGroup();
    }

    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();
    return true;
}
