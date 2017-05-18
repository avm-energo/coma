#include "hiddendialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QGroupBox>
#include <QPushButton>
#include "../widgets/wd_func.h"
#include "../widgets/messagebox.h"
#include "../canal.h"

HiddenDialog::HiddenDialog(int type, QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("QDialog {background-color: rgb(0,192,0);}");
    Type = type;
    switch (type)
    {
    case BYMY:
        BGImage=":/pic/BM.png";
        break;
    case BNMY:
        BGImage=":/pic/BnM.png";
        break;
    case BYMN:
        BGImage=":/pic/BMn.png";
        break;
    case BNMN:
        BGImage=":/pic/BnMn.png";
        break;
    default:
        BGImage="";
        break;
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
    WDFunc::AddLabelAndLineeditH(hlyout, "Тип модуля:", "modtype");
    hlyout->setStretch(1, 20);
    hlyout->addStretch(600);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineeditH(hlyout, "Серийный номер модуля:", "modsn", true);
    hlyout->setStretch(1, 20);
    hlyout->addStretch(600);
    vlyout->addLayout(hlyout);
    if (Type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
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
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(AcceptChanges()));
    hlyout->addStretch(800);
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
    setLayout(vlyout);
}

void HiddenDialog::Fill()
{
    SetVersion(pc.BoardBBhb.HWVer, "bashw");
    SetVersion(pc.BoardMBhb.HWVer, "mezhw");
    QString tmps = QString::number(pc.BoardBBhb.MType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "bastp", tmps);
    WDFunc::SetLEData(this, "modsn", QString::number(pc.BoardBBhb.ModSerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    WDFunc::SetLEData(this, "bassn", QString::number(pc.BoardBBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    WDFunc::SetLEData(this, "mezsn", QString::number(pc.BoardMBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    tmps = QString::number(pc.BoardMBhb.MType, 16);
    tmps.truncate(8);
    WDFunc::SetLEData(this, "meztp", tmps);
    WDFunc::SetLEData(this, "modtype", pc.ModuleTypeString);
    pc.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
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
    GetVersion(pc.BoardBBhb.HWVer, "bashw");
    GetVersion(pc.BoardMBhb.HWVer, "mezhw");
    QString tmps;
    WDFunc::LEData(this, "modsn", tmps);
    pc.BoardBBhb.ModSerialNum = tmps.toInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bassn", tmps);
    pc.BoardBBhb.SerialNum = tmps.toInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "mezsn", tmps);
    pc.BoardMBhb.SerialNum = tmps.toInt(Q_NULLPTR, 16);
    pc.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
    WDFunc::LEData(this, "bastp", tmps);
    pc.BoardBBhb.MType = tmps.toInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "meztp", tmps);
    pc.BoardMBhb.MType = tmps.toInt(Q_NULLPTR, 16);
    SendBhb();
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
    cn->Send(CN_WHv, Canal::BT_BASE, &pc.BoardBBhb, sizeof(pc.BoardBBhb));
    if (cn->result != NOERROR)
    {
        ERMSG("Проблема при записи блока Hidden block базовой платы");
        return;
    }
    if (pc.BoardMBhb.MType != MTM_00)
        cn->Send(CN_WHv, Canal::BT_MEZONIN, &pc.BoardMBhb, sizeof(pc.BoardMBhb));
    if (cn->result == NOERROR)
        MessageBox2::information(this, "Успешно", "Записано успешно");
    else
        ERMSG("Проблема при записи блока Hidden block мезонинной платы");
}
