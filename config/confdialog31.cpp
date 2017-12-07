#include "confdialog31.h"
#include "config31.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

ConfDialog31::ConfDialog31(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog3x(parent)
{
    this->S2Config = &S2Config;
    C31 = new Config31(S2Config, BaseBoard);
    QStringList sl = QStringList() << "Не исп." << "Обычный" << "Инверсия";
    SetInputs(sl, DIN31_NUMCH);
    SetupUI();
    PrereadConf();
}

void ConfDialog31::Fill()
{
    int i;
    for (i = 0; i < DIN31_NUMCH; i++)
    {
        int Intype = C31->Bci_block.inblk.in_type[i];
        if (Intype & D_INTYPE_NONE)
        {
            WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), 0);
            DisableChannel(i, true);
            continue;
        }
        if (Intype == D_INTYPE_NORM)
        {
            WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), 1);
            DisableChannel(i, false);
        }
        if (Intype & D_INTYPE_INV)
        {
            WDFunc::SetCBIndex(this, "chtypcb."+QString::number(i), 2);
            DisableChannel(i, false);
        }
        WDFunc::SetSPBData(this, "chdlyspb."+QString::number(i), static_cast<double>(C31->Bci_block.inblk.dly_time[i])/4);
    }
    int numpair = DIN31_NUMCH / 2;
    for (int i=0; i<numpair; ++i)
    {
        quint8 first = static_cast<quint8>((C31->Bci_block.inblk.pair[i]&0xFF00)>>8);
        quint8 second = static_cast<quint8>(C31->Bci_block.inblk.pair[i]&0x00FF);
        SetPair(first, second);
    }
    CheckConf();
}

void ConfDialog31::FillBack()
{

}

void ConfDialog31::SetPair(int firstch, int secondch)
{
    if ((firstch > DIN31_NUMCH) || (secondch > DIN31_NUMCH) || (firstch < 0) || (secondch < 0))
    {
        MessageBox2::information(this, "Предупреждение", "Ошибка при чтении пары каналов из конфигурации");
        return;
    }
    WDFunc::SetCBIndex(this, "chpaircb."+QString::number(firstch), secondch+1); // +1 из-за 0-го элемента "нет пары"
    WDFunc::SetCBIndex(this, "chpaircb."+QString::number(secondch), firstch+1);
}

void ConfDialog31::CheckConf()
{
    CheckConfErrors.clear();
    // проверяем, все ли пары соответствуют друг другу
    QList<int> InPairs;
    for (int first=0; first<DIN31_NUMCH; ++first)
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
            CheckConfErrors.append("Несоответствие в парах: "+QString::number(first)+", "+QString::number(second));
        else
            InPairs.append(second); // если всё хорошо с парой, добавить в список пропускаемых проверкой
    }
}

void ConfDialog31::SetDefConf()
{
    C31->SetDefConf();
    Fill();
}

void ConfDialog31::SetChTypData(int value)
{
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    if (tmpi < DIN31_NUMCH)
    {
        switch (value)
        {
        case 0: // не исп.
        {
            WDFunc::SetCBIndex(this, "inchpaircb."+QString::number(tmpi), 0); // убираем пару у канала
            DisableChannel(tmpi, true); // отключаем все поля для данного канала
            C31->Bci_block.inblk.in_type[tmpi] |= D_INTYPE_NONE;
            break;
        }
        case 1: // обычный
        {
            DisableChannel(tmpi, false); // отключаем все поля для данного канала
            C31->Bci_block.inblk.in_type[tmpi] |= D_INTYPE_NORM;
            break;
        }
        case 2: // инверсия
        {
            DisableChannel(tmpi, false); // отключаем все поля для данного канала
            C31->Bci_block.inblk.in_type[tmpi] |= D_INTYPE_INV;
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
    if (tmpi < DIN31_NUMCH)
        C31->Bci_block.inblk.dly_time[tmpi] = static_cast<quint32>(dly)*4;
}

void ConfDialog31::SetPair(int ch)
{
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    SetPair(tmpi, ch);
}
