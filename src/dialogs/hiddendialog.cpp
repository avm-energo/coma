#include "hiddendialog.h"

#include "../gen/board.h"
#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/modules.h"
#include "../interfaces/baseinterface.h"
#include "../widgets/wd_func.h"
#include "keypressdialog.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QtSvg/QSvgRenderer>
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

    const auto &board = Board::GetInstance();
    const auto bsi = board.baseSerialInfo();
    if (board.typeB() != Modules::BaseBoard::MTB_00)
    {

        m_bhb.BoardBBhb.HWVer = bsi.HwverB;
        m_bhb.BoardBBhb.SerialNum = bsi.SerialNumB;
        m_bhb.BoardBBhb.MType = bsi.MTypeB;
        m_type |= BYMN;
    }
    if (board.typeM() != Modules::MezzanineBoard::MTM_00)
    {

        m_bhb.BoardMBhb.HWVer = bsi.HwverM;
        m_bhb.BoardMBhb.SerialNum = bsi.SerialNumM;
        m_bhb.BoardMBhb.MType = bsi.MTypeM;
        m_type |= BNMY;
    }
    m_bhb.BoardBBhb.ModSerialNum = bsi.SerialNum;

    m_withMezzanine = false;
    if (m_bhb.BoardBBhb.MType == 0xA1)
        m_BGImage = ":/icons/pkdn.svg";
    else
    {
        switch (m_type)
        {
        case BYMY:
            m_BGImage = ":/icons/BM.svg";
            m_withMezzanine = true;
            break;
        case BNMY:
            m_BGImage = ":/icons/BnM.svg";
            m_type = BYMY;
            m_withMezzanine = true;
            break;
        case BYMN:
            m_BGImage = ":/icons/BMn.svg";
            break;
        case BNMN:
            m_BGImage = ":/icons/BnMn.svg";
            m_type = BYMN;
            break;
        default:
            m_BGImage = "";
            break;
        }
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
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Тип " + tmps + " (hex):", "modtype"), 10);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Серийный номер " + tmps + ":", "modsn", true), 10);
    vlyout->addLayout(hlyout);
    /*    if (WithMezzanine) // ввод данных по мезонинной плате открывается только в случае её наличия
        { */
    QGroupBox *gb = new QGroupBox("Мезонинная плата");
    QVBoxLayout *gblyout = new QVBoxLayout;
    gblyout->addWidget(WDFunc::NewChB2(this, "withmezzanine", "Установлена"));
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Тип платы (hex):", "meztp", true));
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Версия платы:", "mezhwmv", true);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwlv", true);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwsv", true);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    if (m_status)
        WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "mezsn", true);
    else
        WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "mezsn", false);
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(gb, 1);
    hlyout->addStretch(600);
    vlyout->addLayout(hlyout);
    //    }
    vlyout->addStretch(800);
    gb = new QGroupBox("Базовая плата");
    gblyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Тип платы:", "bastp", true);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Версия платы:", "bashwmv", true);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "bashwlv", true);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "bashwsv", true);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    if (m_status)
        WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "bassn", true);
    else
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
        updateMode(status);
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
    WDFunc::SetChBData(this, "withmezzanine", m_withMezzanine);
    setMezzanineEnabled(m_withMezzanine);
    QCheckBox *cb = this->findChild<QCheckBox *>("withmezzanine");
    if (cb != nullptr)
        connect(cb, &QCheckBox::stateChanged, this, &HiddenDialog::setMezzanineEnabled);
}

void HiddenDialog::fill()
{
    setVersion(m_bhb.BoardBBhb.HWVer, "bashw");
    QString tmps = QString::number(m_bhb.BoardBBhb.MType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "bastp", tmps);
    WDFunc::SetLEData(this, "modsn", QString::number(m_bhb.BoardBBhb.ModSerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    WDFunc::SetLEData(this, "bassn", QString::number(m_bhb.BoardBBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    tmps = QString::number(m_bhb.BoardMBhb.MType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "modtype", Board::GetInstance().moduleName());
    m_bhb.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
    if (m_type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
    {
        setVersion(m_bhb.BoardMBhb.HWVer, "mezhw");
        WDFunc::SetLEData(this, "mezsn", QString::number(m_bhb.BoardMBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
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
    // QPushButton *pb = qobject_cast<QPushButton *>(this->sender());
    m_bhb.BoardBBhb.HWVer = getVersion("bashw");
    QString tmps;
    WDFunc::LEData(this, "modsn", tmps);
    m_bhb.BoardBBhb.ModSerialNum = tmps.toUInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bassn", tmps);
    m_bhb.BoardBBhb.SerialNum = tmps.toUInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bastp", tmps);
    m_bhb.BoardBBhb.MType = tmps.toUInt(Q_NULLPTR, 16);
    bool chbdata;
    if (WDFunc::ChBData(
            this, "withmezzanine", chbdata)) // ввод данных по мезонинной плате открывается только в случае её наличия
    {
        if (chbdata)
        {
            m_type |= 0x02;
            m_bhb.BoardMBhb.HWVer = getVersion("mezhw");
            WDFunc::LEData(this, "meztp", tmps);
            m_bhb.BoardMBhb.MType = tmps.toUInt(Q_NULLPTR, 16);
            WDFunc::LEData(this, "mezsn", tmps);
            m_bhb.BoardMBhb.SerialNum = tmps.toUInt(Q_NULLPTR, 16);
            m_bhb.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
        }
    }

    sendBhb();
}

void HiddenDialog::setMezzanineEnabled(int Enabled)
{
    WDFunc::SetEnabled(this, "meztp", Enabled);
    WDFunc::SetEnabled(this, "mezhwmv", Enabled);
    WDFunc::SetEnabled(this, "mezhwsv", Enabled);
    WDFunc::SetEnabled(this, "mezhwlv", Enabled);
    if (m_status)
        WDFunc::SetEnabled(this, "mezsn", Enabled);
    // else
    // WDFunc::SetDisabled(this, "mezsn", Enabled);
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

void HiddenDialog::sendBhb()
{
    void *ptr;
    int size;
    bool chbdata;

    if (!WDFunc::ChBData(this, "withmezzanine", chbdata))
        return;
    if (chbdata)
    {
        ptr = &m_bhb;
        size = sizeof(m_bhb);
    }
    else
    {
        ptr = &m_bhb.BoardBBhb;
        size = sizeof(m_bhb.BoardBBhb);
    }

    auto buffer = QByteArray::fromRawData(static_cast<char *>(ptr), size);
    DataTypes::HardwareStruct block { static_cast<quint32>(m_type), buffer };
    BaseInterface::iface()->writeCommand(Queries::QUSB_WriteHardware, QVariant::fromValue(block));
}

void HiddenDialog::updateMode(bool status)
{
    WDFunc::SetEnabled(this, "mezsn", status);
    WDFunc::SetEnabled(this, "bassn", status);
    WDFunc::SetEnabled(this, "mezsn", status);
}
