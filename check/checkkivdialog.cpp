#include "checkkivdialog.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/etabwidget.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTime>
#include <QVBoxLayout>
#include <QtMath>

CheckKIVDialog::CheckKIVDialog(BoardTypes board, QWidget *parent) : AbstractCheckDialog(board, parent)
{
    //    EParent = parent;
    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 6;
    ChKIV = new Check_KIV;
    Ch = new Check;
    //    BdNum = 11;
    setAttribute(Qt::WA_DeleteOnClose);

    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(6, &ChKIV->Bd_block1, sizeof(Check_KIV::Bd1));

    if (Config::MTB_A2)
        sl = QStringList { "Основные", "Дополнительные" };

    BdUINum = sl.size();

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
}

CheckKIVDialog::~CheckKIVDialog()
{
    delete ChKIV;
    delete Ch;
}

QWidget *CheckKIVDialog::BdUI(int bdnum)
{
    switch (bdnum)
    {

    case 0: // Блок #1

        return ChKIV->Bd1W(this);
    case 1: // Блок #1
        return ChKIV->Bd2W(this);

    default:
        return new QWidget;
    }
}

void CheckKIVDialog::RefreshAnalogValues(int bdnum) { Q_UNUSED(bdnum) }

void CheckKIVDialog::PrepareHeadersForFile(int row)
{
    QString phase[3] = { "A:", "B:", "C:" };

    for (int i = 0; i < 3; i++)
    {
        xlsx->write(row, i + 2, QVariant(("Ueff ф") + phase[i] + ", кВ"));
        xlsx->write(row, i + 5, QVariant("Ieff ф" + phase[i] + ", А"));
        xlsx->write(row, i + 8, QVariant("Cbush ф" + phase[i] + ", пФ"));
        xlsx->write(row, i + 11, QVariant("Tg_d ф" + phase[i] + ", %"));
        xlsx->write(row, i + 14, QVariant("dCbush ф" + phase[i] + ", пФ"));
        xlsx->write(row, i + 17, QVariant("dTg_d ф" + phase[i] + ", %"));
    }
    xlsx->write(row, 20, QVariant("U0"));
    xlsx->write(row, 21, QVariant("U1"));
    xlsx->write(row, 22, QVariant("U2"));
    xlsx->write(row, 23, QVariant("I0"));
    xlsx->write(row, 24, QVariant("I1"));
    xlsx->write(row, 25, QVariant("I2"));
    xlsx->write(row, 26, QVariant("Iunb, мА"));
    xlsx->write(row, 27, QVariant("Phy_unb, град"));
    xlsx->write(row, 28, QVariant("Tmk, °С"));

    xlsx->write(row, 29, QVariant("Tamb, °С"));

    xlsx->write(row, 30, QVariant("Freq, Гц"));
}

void CheckKIVDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    for (int i = 0; i < 3; i++)
    {
        format.setNumberFormat("0.0000");
        xlsx->write(WRow, i + 2, ChKIV->Bd_block1.Ueff[i], format);
        xlsx->write(WRow, i + 5, ChKIV->Bd_block1.Ieff[i], format);
        xlsx->write(WRow, i + 8, ChKIV->Bd_block1.Cbush[i], format);
        xlsx->write(WRow, i + 11, ChKIV->Bd_block1.Tg_d[i], format);
        xlsx->write(WRow, i + 14, ChKIV->Bd_block1.dCbush[i], format);
        xlsx->write(WRow, i + 17, ChKIV->Bd_block1.dTg_d[i], format);
    }
    xlsx->write(WRow, 20, ChKIV->Bd_block1.U0, format);
    xlsx->write(WRow, 21, ChKIV->Bd_block1.U1, format);
    xlsx->write(WRow, 22, ChKIV->Bd_block1.U2, format);
    xlsx->write(WRow, 23, ChKIV->Bd_block1.I0, format);
    xlsx->write(WRow, 24, ChKIV->Bd_block1.I1, format);
    xlsx->write(WRow, 25, ChKIV->Bd_block1.I2, format);
    xlsx->write(WRow, 26, ChKIV->Bd_block1.Iunb, format);
    xlsx->write(WRow, 27, ChKIV->Bd_block1.Phy_unb, format);
    xlsx->write(WRow, 28, ChKIV->Bd_block1.Tmk, format);

    // if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    xlsx->write(WRow, 29, ChKIV->Bd_block1.Tamb, format);

    xlsx->write(WRow, 30, ChKIV->Bd_block1.Frequency, format);
}

QWidget *CheckKIVDialog::CustomTab()
{
    // Оно точно должно возвращать nullptr?
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(ChKIV->Bd1W(this));
    QPushButton *pb = new QPushButton("Начать измерения Bd");
    connect(pb, &QAbstractButton::clicked, this, &AbstractCheckDialog::StartBdMeasurements);
    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения Bd");
    connect(pb, &QAbstractButton::clicked, this, &AbstractCheckDialog::StopBdMeasurements);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return nullptr;
}

void CheckKIVDialog::ChooseValuesToWrite() { }
void CheckKIVDialog::SetDefaultValuesToWrite() { }
void CheckKIVDialog::PrepareAnalogMeasurements() { }

void CheckKIVDialog::USBUpdate()
{
    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw0");
    if (CheckTW == nullptr)
    {
        DBGMSG;
        return;
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(BdNum, &ChKIV->Bd_block1, sizeof(Check_KIV::Bd1)) == Error::Msg::NoError)
            ChKIV->FillBdUSB(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(5, &ChKIV->Bd_block5, sizeof(Check_KIV::Bd5)) == Error::Msg::NoError)
            ChKIV->FillBd5(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(8, &ChKIV->Bd_block8, sizeof(Check_KIV::Bd8)) == Error::Msg::NoError)
            ChKIV->FillBd8(this);
    }
}

void CheckKIVDialog::UpdateFlData(IEC104Thread::FlSignals104 *Signal)
{
    for (int i = 0; i < Signal->SigNumber; i++)
    {
        ChKIV->FillBd(
            this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal + i)->fl.SigVal, 3));
    }
}

void CheckKIVDialog::UpdateSponData(IEC104Thread::SponSignals *Signal)
{
    int i, j;
    for (j = 0; j < Signal->SigNumber; j++)
    {
        quint32 sigadr = Signal->Spon[j].SigAdr;
        quint8 sigval = Signal->Spon[j].SigVal;
        if ((sigadr >= 3011) && (sigadr <= 3013))
        {
            i = sigadr - 3011;
            WDFunc::SetLBLTColor(this, QString::number(1000 + i),
                (sigval == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
        }
        if ((sigadr >= 3014) && (sigadr <= 3016))
        {
            i = sigadr - 3014;
            WDFunc::SetLBLTColor(this, QString::number(1100 + i),
                (sigval == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
        }
        if ((sigadr >= 3018) && (sigadr <= 3020))
        {
            i = sigadr - 3018;
            WDFunc::SetLBLTColor(this, QString::number(1000 + i),
                (sigval == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
        }
        if ((sigadr >= 3021) && (sigadr <= 3023))
        {
            i = sigadr - 3021;
            WDFunc::SetLBLTColor(this, QString::number(1000 + i),
                (sigval == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
            if (sigval == 0)
                stColor[i] = 1;
        }
        if ((sigadr >= 3024) && (sigadr <= 3026))
        {
            i = sigadr - 3024;
            if (sigval == 1)
            {
                stColor[i] = 0;
                WDFunc::SetLBLTColor(this, QString::number(2429 + i), Colors::REDCOLOR);
            }
            else
            {
                if (!stColor[i])
                    WDFunc::SetLBLTColor(this, QString::number(2429 + i), Colors::TABCOLORA1);
            }
        }
        if ((sigadr >= 3027) && (sigadr <= 3029))
        {
            i = sigadr - 3027;
            if (sigval == 1)
                WDFunc::SetLBLTColor(this, QString::number(2426 + i), Colors::TABCOLORA1);
            else
            {
                stColor[3 + i] = 1;
                WDFunc::SetLBLTColor(this, QString::number(2426 + i), Colors::ACONFOCLR);
            }
        }
        if ((sigadr >= 3030) && (sigadr < 3033))
        {
            i = sigadr - 3030;
            if (sigval == 1)
            {
                stColor[3 + i] = 0;
                WDFunc::SetLBLTColor(this, QString::number(2426 + i), Colors::REDCOLOR);
            }
            else
            {
                if (!stColor[3 + i])
                    WDFunc::SetLBLTColor(this, QString::number(2426 + i), Colors::TABCOLORA1);
            }
        }
        if (sigadr == 3034)
        {
            if (sigval == 1)
                WDFunc::SetLBLTColor(this, QString::number(2432), Colors::TABCOLORA1);
            else
            {
                stColor[6] = 1;
                WDFunc::SetLBLTColor(this, QString::number(2432), Colors::ACONFOCLR);
            }
        }
        if (sigadr == 3035)
        {
            if (sigval == 1)
            {
                stColor[6] = 0;
                WDFunc::SetLBLTColor(this, QString::number(2432), Colors::REDCOLOR);
            }
            else
            {
                if (!stColor[6])
                    WDFunc::SetLBLTColor(this, QString::number(2432), Colors::TABCOLORA1);
            }
        }
    }
}

void CheckKIVDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
{

    // ModBusSignal sig = *new ModBusSignal;
    int i = 0;
    for (i = 0; i < Signal.size(); ++i)
    {
        // sig = *(Signal+i);
        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
            ChKIV->FillBd(
                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
        else
            ChKIV->FillBd(
                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
    }
}

void CheckKIVDialog::SetWarnColor(int position, bool value)
{
    /*    if (WarnAlarm.isEmpty())
            return;
        for (int i = 0; i < 18; i++)
        { */
    //        if ((i >= 0) && (i < 3))
    //        {
    //            WDFunc::SetLBLTColor(
    //                this, QString::number(1000 + i), (WarnAlarm[i] == true) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
    //        }
    if ((position >= 0) && (position < 3))
    {
        WDFunc::SetLBLTColor(this, QString::number(1000 + position), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
    }

    if ((position >= 3) && (position < 6))
    {
        WDFunc::SetLBLTColor(this, QString::number(1100 + 3), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
    }

    if ((position >= 7) && (position < 10))
    {
        if (value)
            WDFunc::SetLBLTColor(this, QString::number(1000 + position - 7), Colors::TABCOLORA1);
    }

    if ((position >= 10) && (position < 13))
    {
        WDFunc::SetLBLTColor(
            this, QString::number(2429 + position - 10), (value) ? Colors::ACONFYCLR : Colors::ACONFOCLR);
    }
    else if ((position >= 13) && (position < 16))
    {
        WDFunc::SetLBLTColor(
            this, QString::number(2426 + position - 13), (value) ? Colors::ACONFYCLR : Colors::ACONFOCLR);
    }
    else if (position == 17)
    {
        WDFunc::SetLBLTColor(
            this, QString::number(2432 + position - 13), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
    }
    //}
}

// void CheckKIVDialog::SetAlarmColor(QList<bool> Alarm)
void CheckKIVDialog::SetAlarmColor(int position, bool value)
{
    /*    if (Alarm.isEmpty())
            return;
        for (int i = 0; i < 7; i++)
        { */
    if (position < 3)
    {
        if (value == true)
            WDFunc::SetLBLTColor(this, QString::number(2429 + position), Colors::REDCOLOR);
    }
    else if ((position >= 3) && (position < 6))
    {
        if (value == true)
            WDFunc::SetLBLTColor(this, QString::number(2426 + position - 3), Colors::REDCOLOR);
    }
    else if (position == 6)
    {
        if (value == true)
            WDFunc::SetLBLTColor(this, "2432", Colors::REDCOLOR);
    }
    //}
}
