#include "confdialog35.h"
#include "config35.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

ConfDialog35::ConfDialog35(QVector<publicclass::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog3x(parent)
{
    this->S2Config = &S2Config;
    C35 = new Config35(S2Config, BaseBoard);
    SetupUI();
    PrereadConf();
}

void ConfDialog35::Fill()
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
        quint8 second = static_cast<quint8>(pair&0x00FF);
        SetPair(first, second);
    }
    CheckConf();
}

void ConfDialog35::SetPair(int firstch, int secondch)
{
    if ((firstch > Params.NumCh) || (secondch > Params.NumCh) || (firstch < 0) || (secondch < 0))
    {
        MessageBox2::information(this, "Предупреждение", "Ошибка при чтении пары каналов из конфигурации");
        return;
    }
    WDFunc::SetCBIndex(this, "chpaircb."+QString::number(firstch), secondch+1); // +1 из-за 0-го элемента "нет пары"
    WDFunc::SetCBIndex(this, "chpaircb."+QString::number(secondch), firstch+1);
}

void ConfDialog35::CheckConf()
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
}

void ConfDialog35::SetChTypData(int value)
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

void ConfDialog35::SetDly(double dly)
{
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    if (tmpi < C31->Bci_block.inblk.dly_time.size())
        C31->Bci_block.inblk.dly_time[tmpi] = static_cast<quint32>(dly)*4;
}

void ConfDialog35::SetPair(int ch)
{
    int tmpi = GetChNumFromObjectName(sender()->objectName());
    if (tmpi == GENERALERROR)
        return;
    SetPair(tmpi, ch);
}
