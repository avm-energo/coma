#include "osckivdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"
#include "osc_ids.h"

#include <device/current_device.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <limits>

constexpr std::string_view oscFilenumLblFmt { "Текущий номер осциллограммы: %1" };
constexpr u32 oscStateAddr = 13100;
constexpr u32 oscBlockReqNum = 13;

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

QString OscKivDialog::getFilename()
{
    QString filename("%1-%2-%3-%4");
    QString devType("%1");
    devType = devType.arg(m_device->getDeviceType(), 4, 16, QChar('0'));
    filename = filename.arg(m_device->getUID(), devType, QString::number(m_oscFilenum), QString::number(m_timestamp));
    return filename;
}

void OscKivDialog::resetState()
{
    m_state = State::Init;
    enableButtons(true);
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
        EMessageBox::error(this, errorMsg);
        resetState();
    }
}

void OscKivDialog::receiveOscFile(const S2::FileStruct &file)
{
    if (m_state != State::RequestFile)
        return;

    if (file.ID == MT_HEAD_ID)
    {
        auto header = m_manager.loadCommon(file);
        m_timestamp = header.time;
        m_manager.setHeader(header);
    }
    else
    {
        m_oscModel = m_manager.load(file);
        if (!m_oscModel)
        {
            EMessageBox::error(this, "Ошибка чтения\nфайла осциллограммы");
            resetState();
            return;
        }
        m_manager.loadOsc(m_oscModel.get());
    }

    m_fileBuffer.push_back(file);
    if (m_fileBuffer.size() == 2)
    {
        // Сохраняем в файл
        QByteArray ba;
        S2Util::StoreDataMem(ba, m_fileBuffer, m_oscFilenum);
        auto filename = getFilename();
        auto filepath = WDFunc::ChooseFileForSave(this, "Oscillograms (*.osc)", "osc", filename);
        if (!filepath.isEmpty())
        {
            if (Files::SaveToFile(filepath, ba) == Error::Msg::NoError)
                EMessageBox::information(this, "Записано успешно!");
            else
                EMessageBox::error(this, "Не удалось записать файл");
        }
        m_fileBuffer.clear();
        resetState();
    }
}

void OscKivDialog::writeTypeOsc()
{
    enableButtons(false);
    TypeOsc command { 0, 0, 0 };
    command.n_point = WDFunc::SPBData<u8>(this, "n_point");
    command.phase = WDFunc::CBIndex(this, "phase");
    DataTypes::BlockStruct block { oscBlockReqNum, StdFunc::toByteArray(std::move(command)) };
    m_device->async()->writeCommand(Commands::C_WriteTypeOsc, QVariant::fromValue(block));
    m_state = State::WritingTypeOsc;
}

void OscKivDialog::reqOscFile()
{
    if (m_oscFilenum == noOsc)
        EMessageBox::error(this, "Запрашиваемый файл отсутствует");
    else if (m_oscFilenum == recordingOsc)
        EMessageBox::error(this, "Невозможно прочитать файл,\nидёт запись осциллограммы");
    else
    {
        enableButtons(false);
        m_device->async()->reqFile(m_oscFilenum, DataTypes::FileFormat::CustomS2);
        m_state = State::RequestFile;
    }
}
