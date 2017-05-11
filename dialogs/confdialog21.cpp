#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include "confdialog21.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../canal.h"

ConfDialog21::ConfDialog21(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog2x(S2Config, BaseBoard, parent)
{
    ModuleConf.S2Config = QPointer(S2Config);
    C21 = new Config21(S2Config);
    ModuleConf.Bci_block = static_cast<void *>(C21->Bci_block);
    ModuleConf.Bci_block_size = sizeof(Config21::Bci);
    ModuleConf.Bci_defblock = static_cast<void *>(C21->Bci_defblock);
    Params.InTypes = QStringList() << "Не исп." << "мА" << "В";
    Params.RangeTypes = QStringList() << "Предуст. мА" << "Предуст. В" << "Произвольный";
    Params.NumCh = AIN21_NUMCH;
    PrereadConf();
}

void ConfDialog21::SetRangemA()
{
    QComboBox *cb = qobject_cast<QComboBox *>(sender());
    if (cb == 0)
        return;
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    SetRange(cb->currentIndex(), tmpi);
}

void ConfDialog21::SetRangeV()
{
    QComboBox *cb = qobject_cast<QComboBox *>(sender());
    if (cb == 0)
        return;
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == -1)
        return;
    SetRange(cb->currentIndex()+RT_V05, tmpi); // перемещаем диапазон комбобокса в область V
}

void ConfDialog21::SetRange(int Range, int ChNum)
{
    C21->Bci_block.inblk.in_min[ChNum] = RangeInMins.at(Range);
    C21->Bci_block.inblk.in_max[ChNum] = RangeInMaxs.at(Range);
}

void ConfDialog21::Fill()
{
    int i;
    WDFunc::SetSPBData(this, "oscdlyspb", C21->Bci_block.inblk.oscdly);
    for (i = 0; i < AIN21_NUMCH; i++)
    {
        WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), C21->Bci_block.inblk.in_type[i]);
        quint8 Osc = C21->Bci_block.inblk.osc[i];
        WDFunc::SetChBData(this, "choscdi2."+QString::number(i), Osc&0x20);
        WDFunc::SetChBData(this, "choscdi1."+QString::number(i), Osc&0x10);
        WDFunc::SetChBData(this, "choscthr."+QString::number(i), Osc&0x04);
        WDFunc::SetChBData(this, "chosccso0."+QString::number(i), Osc&0x01);
        SetMinMax(i);
        WDFunc::SetSPBData(this, "2."+QString::number(i), C21->Bci_block.inblk.in_vmin[i]); // in_vmin
        WDFunc::SetSPBData(this, "3."+QString::number(i), C21->Bci_block.inblk.in_vmax[i]); // in_vmax
        WDFunc::SetSPBData(this, "4."+QString::number(i), C21->Bci_block.inblk.setminmin[i]); // setminmin
        WDFunc::SetSPBData(this, "5."+QString::number(i), C21->Bci_block.inblk.setmin[i]); // setmin
        WDFunc::SetSPBData(this, "6."+QString::number(i), C21->Bci_block.inblk.setmax[i]); // setmax
        WDFunc::SetSPBData(this, "7."+QString::number(i), C21->Bci_block.inblk.setmaxmax[i]); // setmaxmax
        DisableChannel(i, (C21->Bci_block.inblk.in_type[i] == Config2x::AIT_NONE));
    }
}

void ConfDialog21::SetMinMax(int i)
{
    QComboBox *cb = this->findChild<QComboBox *>("inrange."+QString::number(i));
    if (cb == 0)
    {
        DBGMSG;
        return;
    }
    switch (C21->Bci_block.inblk.in_type[i])
    {
    case Config2x::AIT_MA: // канал с мА
    {
        if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],20.0))
        {
            cb->setCurrentText(RT_mAText);
            SetRangeWidget(i, RT_mA); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_mA020);
        }
        else if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],4.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],20.0))
        {
            cb->setCurrentText(RT_mAText);
            SetRangeWidget(i, RT_mA); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_mA420);
        }
        else if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],5.0))
        {
            cb->setCurrentText(RT_mAText);
            SetRangeWidget(i, RT_mA); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangemA."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_mA05);
        }
        else
        {
            cb->setCurrentText(RT_MText);
            SetRangeWidget(i, RT_M); // принудительный вызов, чтобы inmin не было равно 0
        }
        break;
    }
    case Config2x::AIT_V: // канал с В
    {
        if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i],0.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],5.0))
        {
            cb->setCurrentText(RT_VText);
            SetRangeWidget(i, RT_V); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangeV."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_V05-3);
        }
        else if (pc.FloatInRange(C21->Bci_block.inblk.in_min[i], -5.0) && pc.FloatInRange(C21->Bci_block.inblk.in_max[i],5.0))
        {
            cb->setCurrentText(RT_VText);
            SetRangeWidget(i, RT_V); // принудительный вызов, чтобы c2b не было равно 0
            QComboBox *c2b = this->findChild<QComboBox *>("rangeV."+QString::number(i));
            if (c2b == 0)
                return;
            c2b->setCurrentIndex(RT_V_55-3);
        }
        else
        {
            cb->setCurrentText(RT_MText);
            SetRangeWidget(i, RT_M); // принудительный вызов, чтобы inmin не было равно 0
        }
        break;
    }
    default: // INTYPENA, INTYPERES
        break;
    }
}

void ConfDialog21::SetChTypData()
{
    s_tqComboBox *cb = qobject_cast<s_tqComboBox *>(sender());
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    int value = cb->currentIndex();
    C21->Bci_block.inblk.in_type[tmpi] = value;
    DisableChannel(tmpi, (value == 0));
}

void ConfDialog21::SetOscDly(int dly)
{
    C21->Bci_block.inblk.oscdly = dly;
}

void ConfDialog21::SetChOsc(int isChecked)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    if (ChNum == -1)
        return;
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ChNum;
    QComboBox *cb = this->findChild<QComboBox *>("oscsrccb."+QString::number(ChNum));
    QLabel *lbl = this->findChild<QLabel *>("oscsrcl."+QString::number(ChNum));
    if (isChecked == Qt::Checked)
    {
/* !!!        C21->Bci_block.inblk.discosc |= tmpint;
        if (cb != 0)
            cb->setVisible(true);
        if (lbl != 0)
            lbl->setVisible(true);
    }
    else
    {
        C21->Bci_block.inblk.discosc &= ~tmpint;
        if (cb != 0)
            cb->setVisible(false);
        if (lbl != 0)
            lbl->setVisible(false); */
    }
}

void ConfDialog21::SetChOscSrc(int srctyp)
{
    int ChNum = GetChNumFromObjectName(sender()->objectName());
    if (ChNum == -1)
        return;
    quint8 tmpi = ChNum << 1;
    quint32 tmpint = srctyp << tmpi;
    quint32 tmpmask = ~(0x00000003 << tmpi);
/* !!!    C21->Bci_block.inblk.oscsrc &= tmpmask;
    C21->Bci_block.inblk.oscsrc |= tmpint; */
}

bool ConfDialog21::CheckConf()
{
    bool NotGood = false;
    for (int i=0; i<Params.NumCh; ++i)
    {
        if (C21->Bci_block.inblk.in_min[i] > C21->Bci_block.inblk.in_max[i])
            NotGood = true;
        if (C21->Bci_block.inblk.in_vmin[i] > C21->Bci_block.inblk.in_vmax[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setminmin[i] > C21->Bci_block.inblk.setmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setminmin[i] < C21->Bci_block.inblk.in_vmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmin[i] < C21->Bci_block.inblk.in_vmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmax[i] < C21->Bci_block.inblk.setmin[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmax[i] > C21->Bci_block.inblk.setmaxmax[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmax[i] > C21->Bci_block.inblk.in_vmax[i])
            NotGood = true;
        if (C21->Bci_block.inblk.setmaxmax[i] > C21->Bci_block.inblk.in_vmax[i])
            NotGood = true;
    }
    return NotGood;

}

void ConfDialog21::SetDefConf()
{
    C21->SetDefConf();
    MessageBox2::information(this, "Успешно", "Задана конфигурация по умолчанию");
}

void AbstractConfDialog2x::SetRangeWidget(int ChNum, int RangeType)
{
    QGroupBox *gb = this->findChild<QGroupBox *>("RangeGB");
    if (gb == 0)
    {
        DBGMSG;
        return;
    }
    QGridLayout *lyout = static_cast<QGridLayout *>(gb->layout());
    if (lyout == 0)
    {
        DBGMSG;
        return;
    }
    QLayoutItem *OldLayoutItem = lyout->itemAtPosition(ChNum+1, 2);
    if (OldLayoutItem != 0)
    {
        QWidget *wdgt = OldLayoutItem->widget();
        if (wdgt != 0)
        {
            wdgt->hide();
            delete wdgt;
        }
        QLayout *olyout = OldLayoutItem->layout();
        if (olyout != 0)
        {
            QLayoutItem *item;
            QWidget *widget;
            while ((item = olyout->takeAt(0)) != 0)
            {
                if ((widget = item->widget()) != 0)
                {
                    widget->hide();
                    delete widget;
                }
                else
                    delete item;
            }
        }
    }
    if (C21->Bci_block.inblk.in_type[ChNum] == INTYPENA)
        return;
    QHBoxLayout *hlyout = new QHBoxLayout;
    switch (RangeType)
    {
    case RT_mA:
    {
        QComboBox *cb = new QComboBox;
        setStyleSheet("QComboBox {border: 1px solid gray; border-radius: 5px;}" \
                      "QComboBox::drop-down {background-color: rgba(100,100,100,255); width: 5px; border-style: none;}");
        cb->addItem("(4..20) мА");
        cb->addItem("(0..20) мА");
        cb->addItem("(0..5) мА");
        cb->setObjectName("rangemA."+QString::number(ChNum));
        connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangemA()));
        cb->setCurrentIndex(0);
        hlyout->addWidget(cb);
        lyout->addLayout(hlyout, ChNum+1, 2, 1, 1);
        SetRange(0, ChNum);
        break;
    }
    case RT_V:
    {
        QComboBox *cb = new QComboBox;
        setStyleSheet("QComboBox {border: 1px solid gray; border-radius: 5px;}" \
                      "QComboBox::drop-down {background-color: rgba(100,100,100,255); width: 5px; border-style: none;}");
        cb->addItem("(0..5) В");
        cb->addItem("(-5..5) В");
        cb->setObjectName("rangeV."+QString::number(ChNum));
        connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetRangeV()));
        cb->setCurrentIndex(0);
        hlyout->addWidget(cb);
        lyout->addLayout(hlyout, ChNum+1, 2, 1, 1);
        SetRange(0+RT_V05, ChNum);
        break;
    }
    case RT_M:
    {
        QDoubleSpinBox *dspbls = new QDoubleSpinBox;
        dspbls->setObjectName("0."+QString::number(ChNum)); // inmin
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-20.0);
        dspbls->setMaximum(20.0);
        QString tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        dspbls->setValue(C21->Bci_block.inblk.in_min[ChNum]);
        hlyout->addWidget(dspbls);
        dspbls = new QDoubleSpinBox(this);
        dspbls->setObjectName("1."+QString::number(ChNum)); // inmax
        dspbls->setSingleStep(0.01);
        dspbls->setMinimum(-20.0);
        dspbls->setMaximum(20.0);
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFGCLR)+";}";
        dspbls->setStyleSheet(tmps);
        connect(dspbls,SIGNAL(editingFinished()),this,SLOT(SetIn()));
        dspbls->setValue(C21->Bci_block.inblk.in_max[ChNum]);
        hlyout->addWidget(dspbls);
        lyout->addLayout(hlyout, ChNum+1, 2, 1, 1);
        break;
    }
    default:
        return;
        break;
    }
}

void ConfDialog21::SetInP2(int type, int chnum, double value)
{
    QString tmps;
    switch(type)
    {
    case 0: // inmin
        C21->Bci_block.inblk.in_min[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(C21GCLR)+";}";
        break;
    case 1: // inmax
        C21->Bci_block.inblk.in_max[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(C21GCLR)+";}";
        break;
    case 2: // invmin
        C21->Bci_block.inblk.in_vmin[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(C21GCLR)+";}";
        break;
    case 3: // invmax
        C21->Bci_block.inblk.in_vmax[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(C21GCLR)+";}";
        break;
    case 4: // setminmin
        C21->Bci_block.inblk.setminmin[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(ACONFRCLR)+";}";
        break;
    case 5: // setmin
        C21->Bci_block.inblk.setmin[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(C21YCLR)+";}";
        break;
    case 6: // setmax
        C21->Bci_block.inblk.setmax[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(C21YCLR)+";}";
        break;
    case 7: // setmaxmax
        C21->Bci_block.inblk.setmaxmax[chnum] = value;
        tmps = "QDoubleSpinBox {background-color: "+QString(C21RCLR)+";}";
        break;
    }
}
