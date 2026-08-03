#include "dialogs/connDialogs/ethernetDialog/scanethernetdevicesdialog.h"

#include <dialogs/connDialogs/ethernetDialog/interfaceethernetdialog.h>
#include <libavm-gen/stdfunc.h>
#include <libavm-widgets/emessagebox.h>
#include <libavm-widgets/pbfunc.h>

#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QtNetwork/QHostAddress>

#include <QFuture>
#include <QFutureWatcher>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

ScanEthernetDevicesDialog::ScanEthernetDevicesDialog(InterfaceEthernetDialog *targetDialog, QWidget *parent)
    : QWidget(parent)
    , m_targetDialog(targetDialog)
{
    setObjectName("ethScanDevicesDialog");
    setupUI();

    connect(this, &ScanEthernetDevicesDialog::pingFinished, this, &ScanEthernetDevicesDialog::handlePingFinish);
}

void ScanEthernetDevicesDialog::setupUI()
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto *groupBox = new QGroupBox("Поиск", this);
    auto *groupLayout = new QVBoxLayout(groupBox);

    m_stack = new QStackedWidget(groupBox);
    m_paramsPage = createParamsPage();
    m_addressScanPage = createAddressScanPage();
    m_portScanPage = createPortScanPage();

    m_stack->addWidget(m_paramsPage);
    m_stack->addWidget(m_addressScanPage);
    m_stack->addWidget(m_portScanPage);

    groupLayout->addWidget(m_stack);
    outerLayout->addWidget(groupBox);
}

QWidget *ScanEthernetDevicesDialog::createParamsPage()
{
    auto *page = new QWidget(m_stack);
    auto *layout = new QVBoxLayout(page);

    auto *subnetLayout = new QHBoxLayout;
    subnetLayout->addWidget(new QLabel("Подсеть:", page));
    m_subnetEdit = new QLineEdit("172.16.29.0/24", page);
    subnetLayout->addWidget(m_subnetEdit);
    layout->addLayout(subnetLayout);

    m_scanButton = PBFunc::New(page, "scanButton", "Сканировать", this, &ScanEthernetDevicesDialog::runScan);
    layout->addWidget(m_scanButton);

    return page;
}

QWidget *ScanEthernetDevicesDialog::createAddressScanPage()
{
    auto *page = new QWidget(m_stack);
    auto *layout = new QVBoxLayout(page);

    layout->addWidget(new QLabel("Сканирование адресов", page));

    m_addressProgressBar = new QProgressBar(page);
    layout->addWidget(m_addressProgressBar);

    m_addressCancelButton
        = PBFunc::New(page, "addressCancelButton", "Отмена", this, &ScanEthernetDevicesDialog::cancelScan);
    layout->addWidget(m_addressCancelButton);

    return page;
}

QWidget *ScanEthernetDevicesDialog::createPortScanPage()
{
    auto *page = new QWidget(m_stack);
    auto *layout = new QVBoxLayout(page);

    layout->addWidget(new QLabel("Сканирование портов", page));

    m_portProgressBar = new QProgressBar(page);
    layout->addWidget(m_portProgressBar);

    m_portCancelButton
        = PBFunc::New(page, "portCancelButton", "Отмена", this, &ScanEthernetDevicesDialog::cancelScan);
    layout->addWidget(m_portCancelButton);

    return page;
}

void ScanEthernetDevicesDialog::runScan()
{
    QString text = m_subnetEdit->text().trimmed();
    if (text.isEmpty())
        return;

    QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(text);
    if (subnet.second == -1)
    {
        EMessageBox::error(this, "Ошибка ввода адреса или маски");
        return;
    }

    ++m_scanGeneration;
    m_hosts.clear();

    int addr_count = std::pow(2, 32 - subnet.second);
    m_pendingPings = addr_count;

    m_addressProgressBar->setRange(0, addr_count);
    m_addressProgressBar->setValue(0);

    m_stack->setCurrentWidget(m_addressScanPage);

    for (int i = 0; i < addr_count; ++i)
    {
        createPingTask(subnet.first.toIPv4Address() + i);
    }
}

void ScanEthernetDevicesDialog::cancelScan()
{
    // Делает захваченный в фоновых задачах номер скана устаревшим — их результаты по приходу
    // будут просто отброшены, без обращения к уже сброшенному состоянию.
    ++m_scanGeneration;
    emit cancelRequested();

    m_hosts.clear();
    m_pendingPings = 0;
    m_addressProgressBar->setValue(0);
    m_portProgressBar->setValue(0);

    backToParams();
}

void ScanEthernetDevicesDialog::backToParams()
{
    m_stack->setCurrentWidget(m_paramsPage);
}

void ScanEthernetDevicesDialog::handlePingFinish()
{
    m_portProgressBar->setRange(0, m_hosts.size());
    m_portProgressBar->setValue(0);

    m_stack->setCurrentWidget(m_portScanPage);

    createPortTask();
}

void ScanEthernetDevicesDialog::handlePortFinish()
{
    m_hosts.removeAll(0);

    if (m_targetDialog)
        m_targetDialog->displayScanResults(m_hosts);

    backToParams();
}

void ScanEthernetDevicesDialog::createPingTask(quint32 ip)
{
    QFutureWatcher<quint32> *watcher = new QFutureWatcher<quint32>(this);
    int generation = m_scanGeneration;

    connect(this, &ScanEthernetDevicesDialog::cancelRequested, watcher, &QFutureWatcher<quint32>::cancel);
    connect(watcher, &QFutureWatcher<quint32>::finished, this,
        [this, watcher, generation]
        {
            watcher->deleteLater();

            // Скан, к которому относилась эта задача, уже отменён или заменён новым — не трогаем
            // ни m_hosts, ни прогресс, относящиеся к текущему (другому) скану.
            if (generation != m_scanGeneration)
                return;

            if (!watcher->isCanceled() && watcher->result())
                m_hosts.append(watcher->result());

            --m_pendingPings;
            m_addressProgressBar->setValue(m_addressProgressBar->maximum() - m_pendingPings);

            if (m_pendingPings == 0)
            {
                emit pingFinished();
            }
        });

    QFuture<quint32> future = QtConcurrent::run(&StdFunc::Ping, ip);
    watcher->setFuture(future);
}

void ScanEthernetDevicesDialog::createPortTask()
{
    QFutureWatcher<QList<quint32>> *watcher = new QFutureWatcher<QList<quint32>>(this);
    quint16 port = 2404;
    int generation = m_scanGeneration;

    connect(this, &ScanEthernetDevicesDialog::cancelRequested, watcher, &QFutureWatcher<QList<quint32>>::cancel);
    connect(
        watcher, &QFutureWatcher<QList<quint32>>::progressValueChanged, m_portProgressBar, &QProgressBar::setValue);
    connect(watcher, &QFutureWatcher<QList<quint32>>::finished, this,
        [this, watcher, generation]()
        {
            // Отменённый QFuture может не иметь ни одного результата — result() в этом случае
            // обращается к пустому хранилищу и падает, поэтому берём его только когда не отменён.
            QList<quint32> result = watcher->isCanceled() ? QList<quint32>() : watcher->result();
            watcher->deleteLater();

            if (generation != m_scanGeneration)
                return;

            m_hosts = result;
            handlePortFinish();
        });

    QFuture<QList<quint32>> future = QtConcurrent::mappedReduced(m_hosts,
        std::bind(qOverload<quint32, quint16>(&StdFunc::CheckPort), std::placeholders::_1, port),
        &StdFunc::joinItem<quint32>);
    watcher->setFuture(future);
}
