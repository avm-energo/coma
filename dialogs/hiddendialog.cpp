#include "hiddendialog.h"
#include "../publicclass.h"
#include "../config/config.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QGroupBox>
#include <QPushButton>
#include "../widgets/wd_func.h"

HiddenDialog::HiddenDialog(int type, QWidget *parent) :
    QDialog(parent)
{
    Type = type;
    switch (type)
    {
    case BYMY:
        BGImage=":/BM.png";
    case BNMY:
        BGImage=":/BnM.png";
    case BYMN:
        BGImage=":/BMn.png";
    case BNMN:
        BGImage=":/BnMn.png";
    default:
        BGImage="";
    }
    SetupUI();
    Fill();
}

void HiddenDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(BGImage));
    e->accept();
}

void HiddenDialog::SetupUI()
{
    QString tmps = "QMainWindow {background-color: "+QString(MAINWINCLR)+";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 600));
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineedit(vlyout, "Серийный номер модуля:", "modsn", true);
    WDFunc::AddLabelAndLineedit(hlyout, "Версия ПО модуля:", "modswmv", true);
    WDFunc::AddLabelAndLineedit(hlyout, ".", "modswlv", true);
    WDFunc::AddLabelAndLineedit(hlyout, ".", "modswsv", true);
    vlyout->addLayout(hlyout);
    QGroupBox *gb = new QGroupBox("Мезонинная плата");
    QVBoxLayout *gblyout = new QVBoxLayout;
    QLabel *lbl = new QLabel ("Мезонинная плата");
    gblyout->addWidget(lbl);
    WDFunc::AddLabelAndLineedit(gblyout, "Тип платы:", "meztp");
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineedit(hlyout, "Версия платы:", "mezhwmv", true);
    WDFunc::AddLabelAndLineedit(hlyout, ".", "mezhwlv", true);
    WDFunc::AddLabelAndLineedit(hlyout, ".", "mezhwsv", true);
    vlyout->addLayout(hlyout);
    WDFunc::AddLabelAndLineedit(gblyout, "Серийный номер платы:", "mezsn", true);
    gb->setLayout(gblyout);
    hlyout->addWidget(gb);
    hlyout->addStretch(10);
    if (Type == BYMY) // ввод данных по мезонинной плате открывается только в случае её наличия
        vlyout->addLayout(hlyout);
    vlyout->addStretch(800);
    hlyout = new QHBoxLayout;
    gb = new QGroupBox("Базовая плата");
    gblyout = new QVBoxLayout;
    lbl = new QLabel ("Базовая плата");
    gblyout->addWidget(lbl);
    WDFunc::AddLabelAndLineedit(gblyout, "Тип платы:", "bastp");
    hlyout = new QHBoxLayout;
    WDFunc::AddLabelAndLineedit(hlyout, "Версия платы:", "bashwmv", true);
    WDFunc::AddLabelAndLineedit(hlyout, ".", "bashwlv", true);
    WDFunc::AddLabelAndLineedit(hlyout, ".", "bashwsv", true);
    vlyout->addLayout(hlyout);
    WDFunc::AddLabelAndLineedit(gblyout, "Серийный номер платы:", "bassn", true);
    gb->setLayout(gblyout);
    hlyout->addWidget(gb);
    hlyout->addStretch(10);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(200);
    QPushButton *pb = new QPushButton("Записать и закрыть");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(AcceptChanges()));
    hlyout->addWidget(pb);
    hlyout->addStretch(200);
    vlyout->addLayout(hlyout);
    setLayout(vlyout);
}

void HiddenDialog::Fill()
{
    SetVersion(pc.BoardBBhb.HWVer, "bashw");
    SetVersion(pc.BoardMBhb.HWVer, "mezhw");
    SetVersion(pc.BoardBBhb.SWVer, "modsw");
    WDFunc::SetLEData(this, "modsn", QString::number(pc.BoardBBhb.ModSerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    WDFunc::SetLEData(this, "bassn", QString::number(pc.BoardBBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    WDFunc::SetLEData(this, "mezsn", QString::number(pc.BoardMBhb.SerialNum, 16), "^[a-fA-F0-9]{1,8}$");
    pc.BoardMBhb.ModSerialNum = pc.BoardMBhb.SWVer = 0xFFFFFFFF;
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
    GetVersion(pc.BoardBBhb.SWVer, "modsw");
    QString tmps;
    WDFunc::LEData(this, "modsn", tmps);
    pc.BoardBBhb.ModSerialNum = tmps.toInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "bassn", tmps);
    pc.BoardBBhb.SerialNum = tmps.toInt(Q_NULLPTR, 16);
    WDFunc::LEData(this, "mezsn", tmps);
    pc.BoardMBhb.SerialNum = tmps.toInt(Q_NULLPTR, 16);
    pc.BoardMBhb.ModSerialNum = pc.BoardMBhb.SWVer = 0xFFFFFFFF;
    this->close();
}

void HiddenDialog::GetVersion(quint32 &number, QString lename)
{
    QString tmps;
    WDFunc::LEData(this, lename+"mv", tmps);
    number = static_cast<quint32>(tmps.toInt()) << 24;
    WDFunc::LEData(this, lename+"sv", tmps);
    number += static_cast<quint32>(tmps.toInt()) << 16;
    WDFunc::LEData(this, lename+"lv", tmps);
    number += static_cast<quint32>(tmps.toInt());
}
