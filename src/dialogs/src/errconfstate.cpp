#include "dialogs/errconfstate.h"

#include <QBoxLayout>
#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <vector>

ErrConfState::ErrConfState(Device::CurrentDevice *device, QWidget *parent)
    : UWidget(device, parent), m_requestTimer(new QTimer(this))
{
    setupUI();
    configureRequest();
}

void ErrConfState::setupUI()
{
    constexpr auto defaultStyle = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
    static const std::vector<QString> labels = {
        "Код разборки конфигурации",           //
        "ID неверного параметра конфигурации", //
        "Принятая длина параметра",            //
        "Нужная длина параметра",              //
        "Результат записи конфигурации",       //
        "Принятая CRC",                        //
        "Вычисленная  CRC",                    //
        "Ошибка записи ВПО"                    //
    };

    auto mainLayout = new QVBoxLayout(this);
    quint32 index = errConfAddr;
    for (auto &label : labels)
    {
        auto layout = new QHBoxLayout;
        auto textLabel = new QLabel(label, this);
        layout->addWidget(textLabel);
        auto valueLabel = new QLabel("", this);
        valueLabel->setObjectName(QString::number(index));
        valueLabel->setStyleSheet(defaultStyle);
        layout->addWidget(valueLabel);
        mainLayout->addLayout(layout);
        index++;
    }
    setLayout(mainLayout);
    setFixedSize(500, 250);
    setWindowTitle("Статус конфигурирования");
}

void ErrConfState::configureRequest()
{
    constexpr quint32 requestInterval = 5000;
    engine()->setUpdatesEnabled(false);
    // TODO: Get from signals (config data storage)...
    engine()->addBs({ errConfAddr, errConfSize });
    m_requestTimer->setInterval(requestInterval);
    connect(m_requestTimer.get(), &QTimer::timeout, this, &ErrConfState::reqUpdate);
    m_requestTimer->start();
}

void ErrConfState::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (bs.sigAdr >= errConfAddr && bs.sigAdr < (errConfAddr + errConfSize))
    {
        auto dataLabel = this->findChild<QLabel *>(QString::number(bs.sigAdr));
        if (dataLabel != nullptr)
        {
            auto param = static_cast<ErrConfParam>(bs.sigAdr);
            switch (param)
            {
            case ErrConfParam::InvalidConfId:
            case ErrConfParam::SendedLength:
            case ErrConfParam::RequiredLength:
                dataLabel->setText(QString::number(bs.sigVal));
                break;
            case ErrConfParam::ConfParseCode:
            case ErrConfParam::ResultWritingConf:
            case ErrConfParam::SendedCRC:
            case ErrConfParam::RequiredCRC:
            case ErrConfParam::HardwareErrorCode:
                dataLabel->setText(createHexString(bs.sigVal));
                break;
            default:
                break;
            }
        }
    }
}

QString ErrConfState::createHexString(quint32 value) const
{
    auto hexStr = QString::number(value, 16).toUpper();
    while (hexStr.length() < (sizeof(quint32) * 2))
        hexStr = "0" + hexStr;
    return "0x" + hexStr;
}

void ErrConfState::closeEvent(QCloseEvent *event)
{
    engine()->setUpdatesEnabled(false);
    event->ignore();
    QWidget::hide();
}

void ErrConfState::show()
{
    engine()->setUpdatesEnabled(true);
    reqUpdate();
    QWidget::show();
}
