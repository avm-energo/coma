#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QTime>
#include <QCoreApplication>
#include <QGroupBox>
#include <QPushButton>
#include <QPixmap>
#include "hiddendialog.h"
#include "../widgets/wd_func.h"
#include "../widgets/emessagebox.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

HiddenDialog::HiddenDialog(int status, QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Type = 0x00;
    ResultOk = false;
//    setStyleSheet("QDialog {background-color: rgb(0,192,0);}");
    ModuleBSI::Bsi bsi = ModuleBSI::GetBsi();
    if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_00)
    {
        Bhb.BoardBBhb.HWVer = bsi.HwverB;
        Bhb.BoardBBhb.SerialNum = bsi.SerialNumB;
        Bhb.BoardBBhb.MType = bsi.MTypeB;
        Type |= BYMN;
    }
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != Config::MTM_00)
    {
        Bhb.BoardMBhb.HWVer = bsi.HwverM;
        Bhb.BoardMBhb.SerialNum = bsi.SerialNumM;
        Bhb.BoardMBhb.MType = bsi.MTypeM;
        Type |= BNMY;
    }
    Bhb.BoardBBhb.ModSerialNum = bsi.SerialNum;

    WithMezzanine = false;
    if (Bhb.BoardBBhb.MType == 0xA1)
        BGImage="images/pkdn.png";
    else
    {
        switch (Type)
        {
        case BYMY:
            BGImage="images/BM.png";
            WithMezzanine = true;
            break;
        case BNMY:
            BGImage="images/BnM.png";
            Type = BYMY;
            WithMezzanine = true;
            break;
        case BYMN:
            BGImage="images/BMn.png";
            break;
        case BNMN:
            BGImage="images/BnMn.png";
            Type = BYMN;
            break;
        default:
            BGImage="";
            break;
        }
    }
    st = status;
    SetupUI();
}

void HiddenDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(BGImage));
    e->accept();
}

void HiddenDialog::SetupUI()
{
    setMinimumSize(QSize(800, 600));
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Тип "+tmps+" (hex):", "modtype"), 10);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Серийный номер "+tmps+":", "modsn", true), 10);
    vlyout->addLayout(hlyout);
/*    if (WithMezzanine) // ввод данных по мезонинной плате открывается только в случае её наличия
    { */
        QGroupBox *gb = new QGroupBox("Мезонинная плата");
        QVBoxLayout *gblyout = new QVBoxLayout;
        gblyout->addWidget(WDFunc::NewChB(this, "withmezzanine", "Установлена"));
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLAndLE(this, "Тип платы (hex):", "meztp", true));
        gblyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        WDFunc::AddLabelAndLineeditH(hlyout, "Версия платы:", "mezhwmv", true);
        WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwlv", true);
        WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwsv", true);
        gblyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        if(st)
        WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "mezsn", true);
        else
        WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "mezsn", false);
        gblyout->addLayout(hlyout);
        gb->setLayout(gblyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(gb,1);
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
    if(st)
    WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "bassn", true);
    else
    WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "bassn", false);
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(600);
    hlyout->addWidget(gb,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Записать и закрыть");
    pb->setObjectName("acceptpb");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(AcceptChanges()));
    hlyout->addStretch(800);
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
    setLayout(vlyout);
    WDFunc::SetLEData(this, "modsn", "00000000", "^\\d{8}$");
    if (Type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
        WDFunc::SetLEData(this, "mezsn", "00000000", "^\\d{8}$");
    WDFunc::SetLEData(this, "bassn", "00000000", "^\\d{8}$");
    WDFunc::SetChBData(this, "withmezzanine", WithMezzanine);
    SetMezzanineEnabled(WithMezzanine);
    QCheckBox *cb = this->findChild<QCheckBox *>("withmezzanine");
    if (cb != nullptr)
        connect(cb,SIGNAL(stateChanged(int)), this, SLOT(SetMezzanineEnabled(int)));
}

void HiddenDialog::Fill()
{
    SetVersion(Bhb.BoardBBhb.HWVer, "bashw");
    QString tmps = QString::number(Bhb.BoardBBhb.MType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "bastp", tmps);
    WDFunc::SetLEData(this, "modsn", QString::number(Bhb.BoardBBhb.ModSerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    WDFunc::SetLEData(this, "bassn", QString::number(Bhb.BoardBBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    tmps = QString::number(Bhb.BoardMBhb.MType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "modtype", ModuleBSI::GetModuleTypeString());
    Bhb.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
    if (Type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
    {
        SetVersion(Bhb.BoardMBhb.HWVer, "mezhw");
        WDFunc::SetLEData(this, "mezsn", QString::number(Bhb.BoardMBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
        WDFunc::SetLEData(this, "meztp", tmps);
    }
}

void HiddenDialog::SetVersion(quint32 number, QString lename)
{
    QString tmps = QString::number(static_cast<quint8>(number >> 24), 16);
    WDFunc::SetLEData(this, lename+"mv", tmps, "^[a-fA-F0-9]$");
    tmps = QString::number(static_cast<quint8>((number & 0x00FF0000) >> 16), 16);
    WDFunc::SetLEData(this, lename+"lv", tmps, "^[a-fA-F0-9]$");
    tmps = QString::number(static_cast<quint16>(number & 0x0000FFFF), 16);
    WDFunc::SetLEData(this, lename+"sv", tmps, "^[a-fA-F0-9]{0,2}$");
}

void HiddenDialog::AcceptChanges()
{
    bool chbdata;
    QPushButton *pb = qobject_cast<QPushButton *>(this->sender());
    GetVersion(Bhb.BoardBBhb.HWVer, "bashw");
    QString tmps;
    WDFunc::LEData(this, "modsn", tmps);
    Bhb.BoardBBhb.ModSerialNum = tmps.toUInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bassn", tmps);
    Bhb.BoardBBhb.SerialNum = tmps.toUInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bastp", tmps);
    Bhb.BoardBBhb.MType = tmps.toUInt(Q_NULLPTR, 16);
    if (WDFunc::ChBData(this, "withmezzanine", chbdata))  // ввод данных по мезонинной плате открывается только в случае её наличия
    {
        if(chbdata)
        {
            Type |= 0x02;
            GetVersion(Bhb.BoardMBhb.HWVer, "mezhw");
            WDFunc::LEData(this, "meztp", tmps);
            Bhb.BoardMBhb.MType = tmps.toUInt(Q_NULLPTR, 16);
            WDFunc::LEData(this, "mezsn", tmps);
            Bhb.BoardMBhb.SerialNum = tmps.toUInt(Q_NULLPTR, 16);
            Bhb.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
        }
    }
#if PROGSIZE != PROGSIZE_EMUL
    if (!SendBhb())
    {
        EMessageBox::error(this, "Ошибка", "Ошибка при записи Hidden block");
        return;
    }
#endif
    QTime tme;
    tme.start();
    int endcounter = RSTTIMEOUT / 1000;
    int counter = endcounter;
    pb->setEnabled(false);
    while (counter > 0)
    {
        pb->setText(QString::number(counter));
        while (tme.elapsed() < 1000)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        tme.start();
        --counter;
    }
    this->close();
}

void HiddenDialog::SetMezzanineEnabled(int Enabled)
{
    WDFunc::SetEnabled(this, "meztp", Enabled);
    WDFunc::SetEnabled(this, "mezhwmv", Enabled);
    WDFunc::SetEnabled(this, "mezhwsv", Enabled);
    WDFunc::SetEnabled(this, "mezhwlv", Enabled);
    if(st)
    WDFunc::SetEnabled(this, "mezsn", Enabled);
    //else
    //WDFunc::SetDisabled(this, "mezsn", Enabled);
}

void HiddenDialog::GetVersion(quint32 &number, QString lename)
{
    QString tmps;
    WDFunc::LEData(this, lename+"mv", tmps);
    number = static_cast<quint32>(tmps.toInt()) << 24;
    WDFunc::LEData(this, lename+"lv", tmps);
    number += static_cast<quint32>(tmps.toInt()) << 16;
    WDFunc::LEData(this, lename+"sv", tmps);
    number += static_cast<quint32>(tmps.toInt());
}

#if PROGSIZE != PROGSIZE_EMUL
bool HiddenDialog::SendBhb()
{
    void *ptr;
    int size;
    bool chbdata;

    if (WDFunc::ChBData(this, "withmezzanine", chbdata))
    {
        if (chbdata)
        {
            ptr = &Bhb;
            size = sizeof(Bhb);
        }
        else
        {
            ptr = &Bhb.BoardBBhb;
            size = sizeof(Bhb.BoardBBhb);
        }
    }
    else
    {
        ERMSG("Неправильный тип платы при записи Hidden block");
        return false;
    }
    if (Commands::WriteHiddenBlock(Type, ptr, size) != Error::ER_NOERROR)
    {
        ERMSG("Проблема при записи блока Hidden block");
        return false;
    }
    EMessageBox::information(this, "Успешно", "Записано успешно");
    ResultOk = true;
    return true;
}
#endif
