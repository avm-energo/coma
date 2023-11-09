#include "hiddendialog.h"

#include "../module/board.h"
#include "../widgets/wd_func.h"
#include "keypressdialog.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtSvg/QSvgRenderer>
#include <interfaces/connection.h>

namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char hashLevel2[] = "fb001dfcffd1c899f3297871406242f097aecf1a5342ccf3ebcd116146188e4b";
static constexpr char name[] = "hiddenHash";
}

HiddenDialog::HiddenDialog(QWidget *parent) : UDialog(crypto::hash, crypto::name, parent)
{
    setSuccessMsg("Записано успешно \nНеобходимо переподключиться");
    m_type = 0x00;
    m_isMezzSelected = false;
    m_isGodMode = false;

    const auto &board = Board::GetInstance();
    const auto bsi = board.baseSerialInfo();
    if (board.typeB() != Modules::BaseBoard::MTB_00)
    {
        m_deviceBlock.baseBoardBlock.hardwareVer = bsi.HwverB;
        m_deviceBlock.baseBoardBlock.serialNum = bsi.SerialNumB;
        m_deviceBlock.baseBoardBlock.boardType = bsi.MTypeB;
        m_type |= BYMN;
    }
    if (board.typeM() != Modules::MezzanineBoard::MTM_00)
    {
        m_deviceBlock.mezzBoardBlock.hardwareVer = bsi.HwverM;
        m_deviceBlock.mezzBoardBlock.serialNum = bsi.SerialNumM;
        m_deviceBlock.mezzBoardBlock.boardType = bsi.MTypeM;
        m_type |= BNMY;
    }
    m_deviceBlock.baseBoardBlock.deviceSerialNum = bsi.SerialNum;

    m_isMezzSelected = false;

    switch (m_type)
    {
    case BYMY:
        m_BGImage = ":/images/BM.svg";
        m_isMezzSelected = true;
        break;
    case BNMY:
        m_BGImage = ":/images/BnM.svg";
        m_type = BYMY;
        m_isMezzSelected = true;
        break;
    case BYMN:
        m_BGImage = ":/images/BMn.svg";
        break;
    case BNMN:
        m_BGImage = ":/images/BnMn.svg";
        m_type = BYMN;
        break;
    default:
        m_BGImage = "";
        break;
    }
    setupUI();
}

void HiddenDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QSvgRenderer svg(m_BGImage);
    svg.render(&painter);
    painter.end();
    e->accept();
}

void HiddenDialog::setupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Тип модуля (hex):", "modtype"), 10);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Серийный номер модуля:", "modsn", false), 10);
    vlyout->addLayout(hlyout);
    QGroupBox *gb = new QGroupBox("Мезонинная плата");
    QVBoxLayout *gblyout = new QVBoxLayout;
    gblyout->addWidget(WDFunc::NewChB2(this, "withmezzanine", "Установлена"));
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Тип платы (hex):", "meztp", false));
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Версия платы:", "mezhwmv", false);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwlv", false);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwsv", false);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "mezsn", false);
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(gb, 1);
    hlyout->addStretch(600);
    vlyout->addLayout(hlyout);
    vlyout->addStretch(800);
    gb = new QGroupBox("Базовая плата");
    gblyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Тип платы:", "bastp", false);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Версия платы:", "bashwmv", false);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "bashwlv", false);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "bashwsv", false);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "bassn", false);
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(600);
    hlyout->addWidget(gb, 1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->setAlignment(Qt::AlignRight);
    QPushButton *pb = new QPushButton("Режим Д'Артяньян");
    connect(pb, &QAbstractButton::clicked, this, [=] {
        KeyPressDialog *dlg = new KeyPressDialog(this);
        bool status = dlg->CheckPassword(crypto::hashLevel2);
        if (!m_isGodMode)
        {
            m_isGodMode = status;
            updateMode(m_isGodMode);
        }
    });
    hlyout->addWidget(pb);
    pb = new QPushButton("Записать и закрыть");
    pb->setObjectName("acceptpb");
    connect(pb, &QAbstractButton::clicked, this, &HiddenDialog::acceptChanges);
    // hlyout->addStretch(800);
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
    setLayout(vlyout);
    WDFunc::SetLEData(this, "modsn", "00000000", "^\\d{8}$");
    if (m_type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
        WDFunc::SetLEData(this, "mezsn", "00000000", "^\\d{8}$");
    WDFunc::SetLEData(this, "bassn", "00000000", "^\\d{8}$");
    WDFunc::SetChBData(this, "withmezzanine", m_isMezzSelected);
    setMezzanineEnabled(m_isMezzSelected);
    QCheckBox *cb = this->findChild<QCheckBox *>("withmezzanine");
    if (cb != nullptr)
        connect(cb, &QCheckBox::stateChanged, this, &HiddenDialog::setMezzanineEnabled);
}

void HiddenDialog::fill()
{
    setVersion(m_deviceBlock.baseBoardBlock.hardwareVer, "bashw");
    QString tmps = QString::number(m_deviceBlock.baseBoardBlock.boardType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "bastp", tmps);
    WDFunc::SetLEData(
        this, "modsn", QString::number(m_deviceBlock.baseBoardBlock.deviceSerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    WDFunc::SetLEData(this, "bassn", QString::number(m_deviceBlock.baseBoardBlock.serialNum, 16), "^[a-fA-F0-9]{1,8}$");
    tmps = QString::number(m_deviceBlock.mezzBoardBlock.boardType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "modtype", Board::GetInstance().moduleName());
    m_deviceBlock.mezzBoardBlock.deviceSerialNum = 0xFFFFFFFF;
    if (m_type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
    {
        setVersion(m_deviceBlock.mezzBoardBlock.hardwareVer, "mezhw");
        WDFunc::SetLEData(
            this, "mezsn", QString::number(m_deviceBlock.mezzBoardBlock.serialNum, 16), "^[a-fA-F0-9]{1,8}$");
        WDFunc::SetLEData(this, "meztp", tmps);
    }
}

void HiddenDialog::setVersion(quint32 number, QString lename)
{
    QString tmps = QString::number(static_cast<quint8>(number >> 24), 16);
    WDFunc::SetLEData(this, lename + "mv", tmps, "^[a-fA-F0-9]$");
    tmps = QString::number(static_cast<quint8>((number & 0x00FF0000) >> 16), 16);
    WDFunc::SetLEData(this, lename + "lv", tmps, "^[a-fA-F0-9]$");
    tmps = QString::number(static_cast<quint16>(number & 0x0000FFFF), 16);
    WDFunc::SetLEData(this, lename + "sv", tmps, "^[a-fA-F0-9]{0,2}$");
}

void HiddenDialog::acceptChanges()
{
    if (!checkPassword())
        return;
    m_deviceBlock.baseBoardBlock.hardwareVer = getVersion("bashw");
    QString tmps;
    WDFunc::LEData(this, "modsn", tmps);
    m_deviceBlock.baseBoardBlock.deviceSerialNum = tmps.toUInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bassn", tmps);
    m_deviceBlock.baseBoardBlock.serialNum = tmps.toUInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bastp", tmps);
    m_deviceBlock.baseBoardBlock.boardType = tmps.toUInt(Q_NULLPTR, 16);
    bool chbdata;
    // ввод данных по мезонинной плате открывается только в случае её наличия
    if (WDFunc::ChBData(this, "withmezzanine", chbdata))
    {
        if (chbdata)
        {
            m_type |= 0x02;
            m_deviceBlock.mezzBoardBlock.hardwareVer = getVersion("mezhw");
            WDFunc::LEData(this, "meztp", tmps);
            m_deviceBlock.mezzBoardBlock.boardType = tmps.toUInt(Q_NULLPTR, 16);
            WDFunc::LEData(this, "mezsn", tmps);
            m_deviceBlock.mezzBoardBlock.serialNum = tmps.toUInt(Q_NULLPTR, 16);
            m_deviceBlock.mezzBoardBlock.deviceSerialNum = 0xFFFFFFFF;
        }
    }
    sendDeviceHiddenBlock();
}

void HiddenDialog::setMezzanineEnabled(int enabled)
{
    m_isMezzSelected = enabled;
    if ((m_isGodMode && m_isMezzSelected) || (!m_isMezzSelected))
    {
        WDFunc::SetEnabled(this, "meztp", enabled);
        WDFunc::SetEnabled(this, "mezhwmv", enabled);
        WDFunc::SetEnabled(this, "mezhwsv", enabled);
        WDFunc::SetEnabled(this, "mezhwlv", enabled);
        WDFunc::SetEnabled(this, "mezsn", enabled);
    }
}

quint32 HiddenDialog::getVersion(QString lename)
{
    QString tmps;
    WDFunc::LEData(this, lename + "mv", tmps);
    quint32 number = static_cast<quint32>(tmps.toInt()) << 24;
    WDFunc::LEData(this, lename + "lv", tmps);
    number += static_cast<quint32>(tmps.toInt()) << 16;
    WDFunc::LEData(this, lename + "sv", tmps);
    number += static_cast<quint32>(tmps.toInt());
    return number;
}

void HiddenDialog::sendDeviceHiddenBlock()
{
    void *ptr;
    int size;
    bool chbdata;

    if (!WDFunc::ChBData(this, "withmezzanine", chbdata))
        return;
    if (chbdata)
    {
        ptr = &m_deviceBlock;
        size = sizeof(m_deviceBlock);
    }
    else
    {
        ptr = &m_deviceBlock.baseBoardBlock;
        size = sizeof(m_deviceBlock.baseBoardBlock);
    }

    if (Board::GetInstance().type() == Modules::Model::MPE)
    {
        m_dataUpdater->currentConnection()->writeCommand(Commands::C_EnableWritingHardware, quint16(0x5c5c));
        StdFunc::Wait(300);
    }

    auto buffer = QByteArray::fromRawData(static_cast<char *>(ptr), size);
    DataTypes::HardwareStruct block { static_cast<quint32>(m_type), buffer };
    m_dataUpdater->currentConnection()->writeCommand(Commands::C_WriteHardware, QVariant::fromValue(block));
}

void HiddenDialog::updateMode(bool enabled)
{
    WDFunc::SetEnabled(this, "modsn", enabled);

    WDFunc::SetEnabled(this, "bastp", enabled);
    WDFunc::SetEnabled(this, "bashwmv", enabled);
    WDFunc::SetEnabled(this, "bashwsv", enabled);
    WDFunc::SetEnabled(this, "bashwlv", enabled);
    WDFunc::SetEnabled(this, "bassn", enabled);

    if (m_isMezzSelected)
    {
        WDFunc::SetEnabled(this, "meztp", enabled);
        WDFunc::SetEnabled(this, "mezhwmv", enabled);
        WDFunc::SetEnabled(this, "mezhwsv", enabled);
        WDFunc::SetEnabled(this, "mezhwlv", enabled);
        WDFunc::SetEnabled(this, "mezsn", enabled);
    }
}
