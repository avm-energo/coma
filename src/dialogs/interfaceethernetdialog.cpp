#include "interfaceethernetdialog.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
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
InterfaceEthernetDialog::InterfaceEthernetDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
}

void InterfaceEthernetDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    tableView = WDFunc::NewQTV(this, "", nullptr);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lyout->addWidget(tableView);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(10);
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Добавить"), this, &InterfaceEthernetDialog::addInterface));
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Сканировать"), this, &InterfaceEthernetDialog::scanInterface));
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Удалить"), this, [this] {
        auto name = tableView->currentIndex().siblingAtColumn(0).data().toString();
        removeDevice(name);
        updateModel();
    }));
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    connect(tableView, &QTableView::doubleClicked, this, &InterfaceEthernetDialog::setInterface);
    AbstractInterfaceDialog::setupUI();
}

void InterfaceEthernetDialog::setInterface(QModelIndex index)
{
    auto *mdl = index.model();
    int row = index.row();
    QString name = mdl->data(mdl->index(row, 0)).toString();
    IEC104Settings settings;
    settings.ip = mdl->data(mdl->index(row, 1)).toString();
    settings.baseadr = mdl->data(mdl->index(row, 2)).toUInt();
    if (!settings.isValid())
        return;
    ConnectStruct st { name, settings };
    emit accepted(st);
}

void InterfaceEthernetDialog::addInterface()
{
    QDialog *dlg = new QDialog(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    dlg->setObjectName("ethdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QHBoxLayout *hlayout = new QHBoxLayout;
    QLabel *lbl = new QLabel(tr("Имя:"), this);
    hlayout->addWidget(lbl);
    QLineEdit *le = new QLineEdit(this);
    le->setObjectName("namele");
    hlayout->addWidget(le);
    mainLayout->addLayout(hlayout);
    hlayout = new QHBoxLayout;
    lbl = new QLabel("IP:", this);
    hlayout->addWidget(lbl);
    QIntValidator *validator = new QIntValidator(0, 255, this);
    for (int i = 0; i < 4; ++i)
    {
        le = new QLineEdit(this);
        le->setValidator(validator);
        le->setMaximumWidth(this->width() / 10);
        le->setObjectName("iple." + QString::number(i));
        hlayout->addWidget(le);
        if (i != 3)
        {
            lbl = new QLabel(".", this);
            hlayout->addWidget(lbl);
        }
    }
    mainLayout->addLayout(hlayout);
    hlayout = new QHBoxLayout;
    lbl = new QLabel(tr("Адрес БС:"), this);
    hlayout->addWidget(lbl);
    le = new QLineEdit(QString::number(205), this);
    le->setObjectName("bsadrspb");
    hlayout->addWidget(le);
    mainLayout->addLayout(hlayout);
    hlayout = new QHBoxLayout;
    hlayout->addWidget(WDFunc::NewPB(dlg, "", tr("Сохранить"), this, &InterfaceEthernetDialog::acceptedInterface));
    hlayout->addWidget(WDFunc::NewPB(dlg, "", tr("Отмена"), [=] { dlg->close(); }));
    mainLayout->addLayout(hlayout);
    dlg->setLayout(mainLayout);
    dlg->adjustSize();
    dlg->exec();
}

void InterfaceEthernetDialog::scanInterface()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);
    connect(this, &InterfaceEthernetDialog::modelUpdated, button, &QPushButton::show);
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

    m_progress = new QProgressDialog(this);
    m_progress->setLabelText(tr("Scanning ping"));
    m_progress->setMinimumDuration(0);
    m_progress->setRange(0, addr_count - 1);

    connect(this, &InterfaceEthernetDialog::pingFinished, &InterfaceEthernetDialog::handlePingFinish);
    for (quint32 i = 0; i < addr_count; ++i)
    {
        createPingTask(subnet.first.toIPv4Address() + i);
    }
}

void InterfaceEthernetDialog::acceptedInterface()
{
    QDialog *dlg = this->findChild<QDialog *>("ethdlg");
    if (dlg == nullptr)
        return;
    QString name = WDFunc::LEData(dlg, "namele");
    // check if there's such name in registry
    if (isKeyExist("Ethernet-", name))
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Такое имя уже имеется"));
        return;
    }
    QString ipstr = WDFunc::LEData(dlg, "iple.0") + "." + WDFunc::LEData(dlg, "iple.1") + "."
        + WDFunc::LEData(dlg, "iple.2") + "." + WDFunc::LEData(dlg, "iple.3");

    QString key = QCoreApplication::applicationName();
    key += "\\" + name;
    auto settings = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));

    int spbdata;
    WDFunc::LEData(dlg, "bsadrspb", spbdata);
    if (spbdata == 0)
    {
        QMessageBox::critical(
            this, tr("Ошибка"), tr("Адрес базовой станции не может быть ") + QString::number(spbdata));
        return;
    }
    rotateSettings("Ethernet-", name);
    settings->setValue("ip", ipstr);
    settings->setValue("bs", QString::number(spbdata));

    if (updateModel())
        qCritical() << Error::GeneralError;
    dlg->close();
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
    QStandardItemModel *mdl = qobject_cast<QStandardItemModel *>(tableView->model());
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
    connect(watcher, &QFutureWatcher<quint32>::finished, [this]() {
        m_progress->setValue(m_progress->value() + 1);
        if (m_progress->value() == -1)
            emit pingFinished();
    });
    QFuture<quint32> future = QtConcurrent::run(&StdFunc::ping, ip);
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
        std::bind(qOverload<quint32, quint16>(&StdFunc::checkPort), std::placeholders::_1, port),
        &StdFunc::joinItem<quint32>);
    watcher->setFuture(future);
    future.waitForFinished();
    m_hosts = future.result();
    watcher->deleteLater();
    handlePortFinish();
}

bool InterfaceEthernetDialog::updateModel()
{
    QStringList ethlist;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        auto sets = std::unique_ptr<QSettings>(new QSettings);
        QString ethname = "Ethernet-" + QString::number(i);
        if (sets->contains(ethname))
            ethlist << sets->value(ethname, "").toString();
    }
    QStringList sl { "Имя", "IP", "Адрес БС" };
    QStandardItemModel *mdl = static_cast<QStandardItemModel *>(tableView->model());
    if (mdl == nullptr)
        mdl = new QStandardItemModel(this);
    else
        mdl->clear();
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
    tableView->setModel(mdl);
    tableView->resizeColumnsToContents();
    return true;
}
