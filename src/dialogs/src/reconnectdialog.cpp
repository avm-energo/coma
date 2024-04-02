#include "dialogs/reconnectdialog.h"

#include "../../widgets/epopup.h"
#include "../../widgets/eprogressindicator.h"
#include "../../widgets/wd_func.h"

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

constexpr auto successMsg { //
    "Переподключение прошло успешно!\n"
    "Данное окно автоматически закроется через %1 сек."
};

ReconnectDialog::ReconnectDialog(QWidget *parent)
    : QDialog(parent), m_success(false), m_seconds(5), m_closeTimer(new QTimer(this))
{
    setupUI();
    m_closeTimer->setSingleShot(false);
    m_closeTimer->setInterval(1000);
    connect(m_closeTimer, &QTimer::timeout, this, &ReconnectDialog::closeTimerTick);
}

void ReconnectDialog::closeTimerTick()
{
    --m_seconds;
    m_messageLabel->setText(QString(successMsg).arg(m_seconds));
    if (m_seconds == 0)
        reject();
}

void ReconnectDialog::setupUI()
{
    setWindowTitle("Переподключение к устройству...");
    auto mainLayout = new QVBoxLayout;
    m_messageLabel = new QLabel("Произошёл разрыв связи с устройством.\nПытаемся наладить соединение...", this);
    mainLayout->addWidget(m_messageLabel, Qt::AlignCenter);

    auto hLayout = new QHBoxLayout;
    {
        m_progressIndicator = new EProgressIndicator(this);
        m_progressIndicator->setMinimumSize(40, 40);
        mainLayout->addStretch();
        hLayout->addStretch();
        hLayout->addWidget(m_progressIndicator, Qt::AlignCenter);
        hLayout->addStretch();
        mainLayout->addLayout(hLayout);
        mainLayout->addStretch();
    }

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0);
    mainLayout->addWidget(m_progressBar, Qt::AlignCenter);

    m_cancelButton = new QPushButton("Прервать соединение", this);
    QObject::connect(m_cancelButton, &QPushButton::clicked, this, &ReconnectDialog::reject);
    mainLayout->addWidget(m_cancelButton, Qt::AlignCenter);

    setLayout(mainLayout);
    setMinimumSize(350, 200);
    setAttribute(Qt::WA_DeleteOnClose);
}

void ReconnectDialog::open()
{
    m_progressIndicator->startAnimation();
    m_progressBar->setValue(0);
    QDialog::open();
}

void ReconnectDialog::reject()
{
    if (!m_success)
    {
        bool disconnect = EMessageBox::question(this, "Прервать соединение?");
        if (disconnect)
            emit breakConnection();
        else
            return;
    }
    QDialog::reject();
}

void ReconnectDialog::reconnectSuccess()
{
    m_success = true;
    auto mainLayout = this->layout();
    mainLayout->removeWidget(m_progressIndicator);
    mainLayout->removeWidget(m_progressBar);
    mainLayout->removeWidget(m_cancelButton);
    m_progressIndicator->stopAnimation();
    m_messageLabel->setText(QString(successMsg).arg(m_seconds));
    mainLayout->addWidget(WDFunc::NewHexagonPB(
        this, "", [this]() { this->reject(); }, ":/icons/tnyes.svg", "Закрыть"));
    m_closeTimer->start();
}
