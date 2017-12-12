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
#include "../gen/commands.h"

HiddenDialog::HiddenDialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Type = 0x00;
//    setStyleSheet("QDialog {background-color: rgb(0,192,0);}");
/*    if (pc.ModuleBsi.MTypeB != MTB_00)
    { */
        Bhb.BoardBBhb.HWVer = pc.ModuleBsi.HwverB;
        Bhb.BoardBBhb.SerialNum = pc.ModuleBsi.SerialNumB;
        Bhb.BoardBBhb.MType = pc.ModuleBsi.MTypeB;
        Type |= BYMN;
/*    }
    if (pc.ModuleBsi.MTypeM != MTM_00)
    { */
        Bhb.BoardMBhb.HWVer = pc.ModuleBsi.HwverM;
        Bhb.BoardMBhb.SerialNum = pc.ModuleBsi.SerialNumM;
        Bhb.BoardMBhb.MType = pc.ModuleBsi.MTypeM;
        Type |= BNMY;
//    }
    Bhb.BoardBBhb.ModSerialNum = pc.ModuleBsi.SerialNum;

    if (Bhb.BoardBBhb.MType == 0xA1)
        BGImage="images/pkdn.png";
    else
    {
        switch (Type)
        {
        case BYMY:
            BGImage="images/BM.png";
            break;
        case BNMY:
            BGImage="images/BnM.png";
            break;
        case BYMN:
            BGImage="images/BMn.png";
            break;
        case BNMN:
            BGImage="images/BnMn.png";
            break;
        default:
            BGImage="";
            break;
        }
    }
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
    WDFunc::AddLabelAndLineeditH(hlyout, "Тип "+tmps+":", "modtype");
    hlyout->setStretch(1, 20);
    hlyout->addStretch(600);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер "+tmps+":", "modsn", true);
    hlyout->setStretch(1, 20);
    hlyout->addStretch(600);
    vlyout->addLayout(hlyout);
    if ((Type == BYMY) || (Type == BNMY)) // ввод данных по мезонинной плате открывается только в случае её наличия
    {
        QGroupBox *gb = new QGroupBox("Мезонинная плата");
        QVBoxLayout *gblyout = new QVBoxLayout;
        hlyout = new QHBoxLayout;
        WDFunc::AddLabelAndLineeditH(hlyout, "Тип платы:", "meztp", true);
        gblyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        WDFunc::AddLabelAndLineeditH(hlyout, "Версия платы:", "mezhwmv", true);
        WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwlv", true);
        WDFunc::AddLabelAndLineeditH(hlyout, ".", "mezhwsv", true);
        gblyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "mezsn", true);
        gblyout->addLayout(hlyout);
        gb->setLayout(gblyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(gb,1);
        hlyout->addStretch(600);
        vlyout->addLayout(hlyout);
    }
    vlyout->addStretch(800);
    QGroupBox *gb = new QGroupBox("Базовая плата");
    QVBoxLayout *gblyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Тип платы:", "bastp", true);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Версия платы:", "bashwmv", true);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "bashwlv", true);
    WDFunc::AddLabelAndLineeditH(hlyout, ".", "bashwsv", true);
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер платы:", "bassn", true);
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
    WDFunc::SetLEData(this, "modtype", pc.ModuleTypeString);
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
    QPushButton *pb = qobject_cast<QPushButton *>(this->sender());
    GetVersion(Bhb.BoardBBhb.HWVer, "bashw");
    QString tmps;
    WDFunc::LEData(this, "modsn", tmps);
    Bhb.BoardBBhb.ModSerialNum = tmps.toInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bassn", tmps);
    Bhb.BoardBBhb.SerialNum = tmps.toInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bastp", tmps);
    Bhb.BoardBBhb.MType = tmps.toInt(Q_NULLPTR, 16);
    if (Type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
    {
        GetVersion(Bhb.BoardMBhb.HWVer, "mezhw");
        WDFunc::LEData(this, "meztp", tmps);
        Bhb.BoardMBhb.MType = tmps.toInt(Q_NULLPTR, 16);
        WDFunc::LEData(this, "mezsn", tmps);
        Bhb.BoardMBhb.SerialNum = tmps.toInt(Q_NULLPTR, 16);
        Bhb.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
    }
    SendBhb();
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

void HiddenDialog::SendBhb()
{
    if (Commands::WriteHiddenBlock(BT_BSMZ, &Bhb, sizeof(Bhb)) != NOERROR)
    {
        ERMSG("Проблема при записи блока Hidden block");
        return;
    }
    MessageBox2::information(this, "Успешно", "Записано успешно");
}
