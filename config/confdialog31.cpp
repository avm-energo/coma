#include "confdialog31.h"
#include "config31.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

ConfDialog31::ConfDialog31(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog3x(parent)
{
    this->S2Config = &S2Config;
    C31 = new Config31(S2Config, BaseBoard);
    Params.InTypes = QStringList() << "Не исп." << "Обычный" << "Инверсия";
    Params.NumCh = AIN31_NUMCH;
    SetupUI();
    PrereadConf();
}

void ConfDialog31::Fill()
{
    int i;
    for (i = 0; i < Params.NumCh; i++)
    {
        int Intype = C31->Bci_block.inblk.in_type[i];
        if (Intype == Config3x::DIT_NONE)
        {
            WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), 0);
            DisableChannel(i, true);
            continue;
        }
        if (Intype == Config3x::DIT_NORM)
        {
            WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), 1);
            DisableChannel(i, false);
        }
        if (Intype == Config3x::DIT_INV)
        {
            WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), 2);
            DisableChannel(i, false);
        }
        WDFunc::SetSPBData(this, "chdlyspb."+QString::number(i), static_cast<double>(C31->Bci_block.inblk.dly_time[i])/4);
    }
    quint16 pair;
    foreach(pair, C31->Bci_block.inblk.pair)
    {
        quint8 first = static_cast<quint8>((pair&0xFF00)>>8);
        quint8 second = static_cast<quint8>(pait&0x00FF);
        SetPair(first, second);
    }
    CheckConf();
}

void ConfDialog31::SetPair(int firstch, int secondch)
{
    if ((firstch > Params.NumCh) || (secondch > Params.NumCh))
    {
        MessageBox2::information(this, "Предупреждение", "Ошибка при чтении пары каналов из конфигурации");
        return;
    }
    WDFunc::SetCBIndex(this, "chpaircb."+QString::number(firstch), secondch+1); // +1 из-за 0-го элемента "нет пары"
    WDFunc::SetCBIndex(this, "chpaircb."+QString::number(secondch), firstch+1);
}

bool ConfDialog31::CheckConf()
{
    // проверяем, все ли пары соответствуют друг другу
    bool result = true;
    QMap<quint8, quint8> Pairs;
    QList<int> InPairs;
    for (int first=0; first<Params.NumCh; ++first)
    {
        WDFunc::SetCBColor(this, "chpaircb."+QString::number(first), DCONFWCLR);
        if (InPairs.contains(first)) // if current index is in Pairs already
            continue;
        int second, firstcheck;
        WDFunc::CBIndex(this, "chpaircb."+QString::number(first), second);
        if (second == 0) // нет пары
            continue;
        WDFunc::CBIndex(this, "chpaircb."+QString::number(--second), firstcheck); // -- из-за 0-го элемента "нет пары"
        if (first != firstcheck)
        {
            WDFunc::SetCBColor(this, "chpaircb."+QString::number(first), DCONFRCLR);
            WDFunc::SetCBColor(this, "chpaircb."+QString::number(second), DCONFRCLR);
            result = false;
        }
        else
            InPairs.append(second); // если всё хорошо с парой, добавить в список пропускаемых проверкой
    }
    return result;
}

void ConfDialog31::SetChTypData(int value)
{
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    if (tmpi < C31->Bci_block.inblk.in_type.size())
    {
        switch (value)
        {
        case 0: // не исп.
        {
            WDFunc::SetCBIndex(this, "chpaircb."+QString::number(tmpi), 0); // убираем пару у канала
            DisableChannel(tmpi, true); // отключаем все поля для данного канала
            C31->Bci_block.inblk.in_type[tmpi] = Config3x::DIT_NONE;
            break;
        }
        case 1: // обычный
        {
            DisableChannel(tmpi, false); // отключаем все поля для данного канала
            C31->Bci_block.inblk.in_type[tmpi] = Config3x::DIT_NORM;
            break;
        }
        case 2: // инверсия
        {
            DisableChannel(tmpi, false); // отключаем все поля для данного канала
            C31->Bci_block.inblk.in_type[tmpi] = Config3x::DIT_INV;
            break;
        }
        default:
            break;
        }
    }
}

void ConfDialog31::SetDly(double dly)
{
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    if (tmpi < C31->Bci_block.inblk.dly_time.size())
        C31->Bci_block.inblk.dly_time[tmpi] = static_cast<quint32>(dly)*4;
}

/*
void ConfDialog31::SetChTypData(int num, s_tqComboBox *cb)
{
    Bci_block.in_type[cb->getAData().toInt()] = num;
}

void ConfDialog31::SetOscDly(int dly)
{
    Bci_block.oscdly = dly;
}

void ConfDialog31::SetChOsc(int isChecked, s_tqCheckBox *ptr)
{
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ptr->getAData().toInt();
    s_tqComboBox *cb = this->findChild<s_tqComboBox *>("oscsrccb"+QString::number(ptr->getAData().toInt()));
    QLabel *lbl = this->findChild<QLabel *>("oscsrcl"+QString::number(ptr->getAData().toInt()));
    if (isChecked == Qt::Checked)
    {
        Bci_block.discosc |= tmpint;
        if (cb != 0)
            cb->setVisible(true);
        if (lbl != 0)
            lbl->setVisible(true);
    }
    else
    {
        Bci_block.discosc &= ~tmpint;
        if (cb != 0)
            cb->setVisible(false);
        if (lbl != 0)
            lbl->setVisible(false);
    }
}

void ConfDialog31::SetChOscSrc(int srctyp, s_tqComboBox *ptr)
{
    quint8 tmpi = ptr->getAData().toInt() << 1;
    quint32 tmpint = srctyp << tmpi;
    quint32 tmpmask = ~(0x00000003 << tmpi);
    Bci_block.oscsrc &= tmpmask;
    Bci_block.oscsrc |= tmpint;
}

void ConfDialog31::SetInMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_min[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::SetInMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_max[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::SetInVMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_vmin[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::SetInVMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.in_vmax[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::SetMinMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setminmin[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::SetMin(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setmin[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::SetMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setmax[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::SetMaxMax(double dbl, s_tqSpinBox *ptr)
{
    Bci_block.setmaxmax[ptr->getAData().toInt()] = dbl;
}

void ConfDialog31::Set104(double dbl, s_tqSpinBox *ptr)
{
    switch (ptr->getAData().toInt())
    {
    case 0:
    {
        Bci_block.Abs_104=dbl;
        break;
    }
    case 1:
    {
        Bci_block.Cycle_104=dbl;
        break;
    }
    case 2:
    {
        Bci_block.T1_104=dbl;
        break;
    }
    case 3:
    {
        Bci_block.T2_104=dbl;
        break;
    }
    case 4:
    {
        Bci_block.T3_104=dbl;
        break;
    }
    case 5:
    {
        Bci_block.k_104=dbl;
        break;
    }
    case 6:
    {
        Bci_block.w_104=dbl;
        break;
    }
    default:
        break;
    }
}

void ConfDialog31::SetCType(int num)
{
    Bci_block.Ctype = num;
}

void ConfDialog31::WriteConfDataToModule()
{
    connect(cn,SIGNAL(sendend()),this,SLOT(WriteCompleted()));
    cn->Send(CN_WF, canal::BT_NONE, &Bci_block, sizeof(Bci_block), 2, Config);
    if (cn->result == CN_OK)
        QMessageBox::information(this,"Успешно!","Операция проведена успешно!");
}

void ConfDialog31::SetNewConf()
{
    cn->Send(CN_Cnc);
    if (cn->result == CN_OK)
        emit BsiIsNeedToBeAcquiredAndChecked();
}

void ConfDialog31::SetDefConf()
{
    memcpy(&Bci_block, &Bci_defblock, sizeof(Bci));
    FillConfData();
}

void ConfDialog31::UpdateProper(bool tmpb)
{
    NoProperConf = tmpb;
} */
