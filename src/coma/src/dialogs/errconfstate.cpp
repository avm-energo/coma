#include "dialogs/errconfstate.h"

#include <QBoxLayout>
#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <vector>

ErrConfState::ErrConfState(Device::CurrentDevice *device, QWidget *parent)
    : UWidget(device, parent), m_requestTimer(new QTimer(this))
{
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setupUI();
    configureRequest();
}

void ErrConfState::setupUI()
{
    constexpr auto defaultStyle = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
    static const std::vector<QString> labels = {
        "Код разборки конфигурации",           //
        "ID неверного параметра конфигурации", //
        "Принятая длина параметра",            //
        "Нужная длина параметра",              //
        "Результат записи конфигурации",       //
        "Принятая CRC",                        //
        "Вычисленная  CRC",                    //
        "Результат записи ВПО"                 //
    };

    auto mainLayout = new QVBoxLayout(this);
    quint32 index = errConfAddr;
    for (auto &label : labels)
    {
        auto layout = new QHBoxLayout;
        auto textLabel = new QLabel(label, this);
        layout->addWidget(textLabel);
        auto valueEdit = new QLineEdit(this);
        valueEdit->setObjectName(QString::number(index));
        valueEdit->setReadOnly(true);
        valueEdit->setStyleSheet(defaultStyle);
        layout->addWidget(valueEdit);
        mainLayout->addLayout(layout);
        index++;
    }
    auto okButton = new QPushButton("OK", this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &QWidget::close);
    mainLayout->addWidget(okButton, 0, Qt::AlignHCenter);
    setLayout(mainLayout);
    setFixedSize(500, 340);
    setWindowTitle("Статус конфигурирования");
}

void ErrConfState::configureRequest()
{
    constexpr quint32 requestInterval = 10000;
    engine()->setUpdatesEnabled(true);
    // TODO: Get from signals (config data storage)...
    engine()->addBs({ errConfAddr, errConfSize });
    m_requestTimer->setInterval(requestInterval);
    connect(m_requestTimer.get(), &QTimer::timeout, this, &ErrConfState::reqUpdate);
    m_requestTimer->start();
    reqUpdate();
}

void ErrConfState::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (bs.sigAdr >= errConfAddr && bs.sigAdr < (errConfAddr + errConfSize))
    {
        if (bs.sigAdr == errConfAddr)
            emit confParseCodeUpdated(bs.sigVal);
        auto dataEdit = this->findChild<QLineEdit *>(QString::number(bs.sigAdr));
        if (dataEdit != nullptr)
        {
            auto param = static_cast<ErrConfParam>(bs.sigAdr);
            switch (param)
            {
            case ErrConfParam::InvalidConfId:
            case ErrConfParam::SendedLength:
            case ErrConfParam::RequiredLength:
                dataEdit->setText(QString::number(bs.sigVal));
                break;
            case ErrConfParam::ConfParseCode:
            case ErrConfParam::ResultWritingConf:
            case ErrConfParam::SendedCRC:
            case ErrConfParam::RequiredCRC:
            case ErrConfParam::HardwareErrorCode:
                dataEdit->setText(createHexString(bs.sigVal));
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
    event->ignore();
    QWidget::hide();
}

void ErrConfState::show()
{
    reqUpdate();
    QWidget::show();
}
