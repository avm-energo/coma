#include "osckivdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <device/current_device.h>
#include <gen/stdfunc.h>
#include <limits>

constexpr std::string_view oscFilenumLblFmt { "Текущий номер осциллограммы: %1" };
constexpr u32 oscStateAddr = 13100;

constexpr u32 noOsc = std::numeric_limits<u16>::min();
constexpr u32 recordingOsc = std::numeric_limits<u16>::max();

OscKivDialog::OscKivDialog(Device::CurrentDevice *device, QWidget *parent)
    : UDialog(device, parent)
    , m_commandBtn(nullptr)
    , m_readBtn(nullptr)
    , m_oscFilenumLbl(nullptr)
    , m_reqStateOscTimer(new QTimer(this))
    , m_oscFilenum(0)
    , m_oldOscFilenum(0)
    , m_state(State::Init)
{
    m_reqStateOscTimer->setInterval(100);
    QObject::connect(m_reqStateOscTimer, &QTimer::timeout, this, &OscKivDialog::reqOscState);
    m_device->async()->connection(this, &OscKivDialog::updateBitStringData);
    m_device->async()->connection(this, &OscKivDialog::receiveOscFile);
    setupUI();
    reqOscState();
}

void OscKivDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    m_oscFilenumLbl = WDFunc::NewLBL2(this, oscFilenumLblFmt.data(), "oscFilenumLabel");
    mainLayout->addWidget(m_oscFilenumLbl);
    auto pointsSpinbox = WDFunc::NewSPB2(this, "n_point", 1, 8, 0);
    mainLayout->addWidget(pointsSpinbox);
    auto phaseCombobox = WDFunc::NewCB2(this, "phase", { "Все фазы", "Фаза A", "Фаза B", "Фаза C" });
    phaseCombobox->setCurrentIndex(0);
    mainLayout->addWidget(phaseCombobox);
    auto controlLayout = new QHBoxLayout;
    m_commandBtn = WDFunc::NewPB(this, "commandBtn", "Запуск осциллограмм", this, &OscKivDialog::writeTypeOsc);
    controlLayout->addWidget(m_commandBtn);
    m_readBtn = WDFunc::NewPB(this, "readBtn", "Прочитать осциллограмму", this, &OscKivDialog::reqOscFile);
    // m_readBtn->setEnabled(false);
    controlLayout->addWidget(m_readBtn);
    mainLayout->addLayout(controlLayout);
    setLayout(mainLayout);
}

void OscKivDialog::updateOscFilenum(const u32 newOscFilenum)
{
    if (m_state == State::Init)
        m_oldOscFilenum = newOscFilenum;
    else
        m_oldOscFilenum = m_oscFilenum;
    m_oscFilenum = newOscFilenum;

    if (m_oscFilenum == noOsc)
        m_oscFilenumLbl->setText("В памяти нет осциллограммы");
    else if (m_oscFilenum == recordingOsc)
        m_oscFilenumLbl->setText("Идёт запись осциллограммы");
    else
        m_oscFilenumLbl->setText(QString(oscFilenumLblFmt.data()).arg(m_oscFilenum));

    if (m_state == State::RequestOscState)
    {
        // Если отличаются номера осциллограммы и в данный момент не идёт запись осциллограммы
        if (m_oldOscFilenum != m_oscFilenum && m_oscFilenum != recordingOsc)
        {
            m_reqStateOscTimer->stop();
            auto infoMsg = QString("Записано успешно\n%1").arg(oscFilenumLblFmt.data()).arg(m_oscFilenum);
            m_state = State::Init;
            enableButtons(true);
            EMessageBox::information(this, infoMsg);
        }
    }
}

void OscKivDialog::enableButtons(const bool enabling)
{
    m_commandBtn->setEnabled(enabling);
    m_readBtn->setEnabled(enabling);
}

void OscKivDialog::reqOscState()
{
    m_device->async()->reqBitStrings(oscStateAddr, 1);
}

void OscKivDialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (bs.sigAdr == oscStateAddr)
        updateOscFilenum(bs.sigVal);
}

void OscKivDialog::updateGeneralResponse(const DataTypes::GeneralResponseStruct &response)
{
    if (response.type == DataTypes::GeneralResponseTypes::Ok)
    {
        if (m_state == State::WritingTypeOsc)
        {
            reqOscState();
            m_reqStateOscTimer->start();
            m_state = State::RequestOscState;
        }
    }
    else if (response.type == DataTypes::GeneralResponseTypes::Error)
    {
        auto errorMsg = //
            QString("Произошла ошибка: %1")
                .arg(Error::MsgStr.value(static_cast<Error::Msg>(response.data), "Неизвестная ошибка"));
        m_state = State::Init;
        enableButtons(true);
        EMessageBox::error(this, errorMsg);
    }
}

void OscKivDialog::receiveOscFile(const S2::FileStruct &file)
{
    ;
}

void OscKivDialog::writeTypeOsc()
{
    enableButtons(false);
    TypeOsc command { 0, 0, 0 };
    command.n_point = WDFunc::SPBData<u8>(this, "n_point");
    command.phase = WDFunc::CBIndex(this, "phase");
    DataTypes::BlockStruct block { 13, StdFunc::toByteArray(command) };
    m_device->async()->writeCommand(Commands::C_WriteTypeOsc, QVariant::fromValue(block));
    m_state = State::WritingTypeOsc;
}

void OscKivDialog::reqOscFile()
{
    if (m_oscFilenum == noOsc)
    {
        EMessageBox::error(this, "Невозможно прочитать файл,\nидёт запись осциллограммы");
        return;
    }
    else if (m_oscFilenum == recordingOsc)
    {
        EMessageBox::error(this, "Невозможно прочитать файл,\nидёт запись осциллограммы");
        return;
    }
    else
    {
        enableButtons(false);
        m_device->async()->reqFile(m_oscFilenum, DataTypes::FileFormat::CustomS2);
        m_state = State::RequestFile;
    }
}
