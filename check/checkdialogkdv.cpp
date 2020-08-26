#include "checkdialogkdv.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/emessagebox.h"
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

CheckDialogKDV::CheckDialogKDV(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: " + QString(UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 6;
    ChKDV = new CheckKDV();
    Ch = new Check;
    //    BdNum = 11;
    setAttribute(Qt::WA_DeleteOnClose);

    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(1, &ChKDV->Bda_block, sizeof(CheckKDV::Bda_in_struct));
    SetBd(2, &ChKDV->Bd2_Gr1, sizeof(CheckKDV::Bd2));
    SetBd(3, &ChKDV->Bd3_Gr2, sizeof(CheckKDV::Bd2));
    SetBd(4, &ChKDV->Bd4_U0, sizeof(CheckKDV::Bd4));
    SetBd(5, &ChKDV->Bd5_UI_Harm, sizeof(CheckKDV::Bd5));
    SetBd(6, &ChKDV->Bd6_U0, sizeof(CheckKDV::Bd4));
    SetBd(7, &ChKDV->Bd7_UI_Harm, sizeof(CheckKDV::Bd5));
    SetBd(9, &ChKDV->Bd9_Resurs, sizeof(CheckKDV::Bda_RESURS));
    SetBd(10, &ChKDV->Bd10_Power, sizeof(CheckKDV::Bd10));
    SetBd(11, &ChKDV->Bd11_Model, sizeof(CheckKDV::Bda_MODEL));

    SetBd(13, &ChKDV->Bd13_Temp, sizeof(CheckKDV::Bda_TEMP));

    sl = QStringList() << "Общие"
                       << "Входные сигналы"
                       << "Температуры"
                       << "Первичные"
                       << "Качество"
                       << "Мощности"
                       << "Гармоники"
                       << "Ресурс"
                       << "Допустимые перегрузки"
                       << "Температуры";

    BdUINum = sl.size();

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
}

QWidget *CheckDialogKDV::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case 0:
        return Ch->Bd0W(this);
    case 1: // Блок #1
        return ChKDV->Bd1W(this);
    case 2: // Блок #1
        return ChKDV->Bd8W(this);
    case 3: // Блок #1
        return ChKDV->Bd3W(this);
    case 4: // Блок #1
        return ChKDV->Bd4W(this);
    case 5: // Блок #1
        return ChKDV->Bd5W(this);
    case 6: // Блок #1
        return ChKDV->Bd6W(this);
    case 7: // Блок #1
        return ChKDV->Bd7W(this);
    case 8: // Блок #1
        return ChKDV->Bd8W(this);

    default:
        return new QWidget;
    }
}

void CheckDialogKDV::RefreshAnalogValues(int bdnum)
{
    Q_UNUSED(bdnum)
}

void CheckDialogKDV::PrepareHeadersForFile(int row)
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

void CheckDialogKDV::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    for (int i = 0; i < 3; i++)
    {
        format.setNumberFormat("0.0000");
    }
}

QWidget *CheckDialogKDV::CustomTab()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(ChKDV->Bd1W(this));
    QPushButton *pb = new QPushButton("Начать измерения Bd");

    connect(pb, SIGNAL(clicked(bool)), this, SLOT(StartBdMeasurements()));

    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения Bd");

    connect(pb, SIGNAL(clicked(bool)), this, SLOT(StopBdMeasurements()));

    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return nullptr;
}

void CheckDialogKDV::ChooseValuesToWrite()
{
}
void CheckDialogKDV::SetDefaultValuesToWrite()
{
}
void CheckDialogKDV::PrepareAnalogMeasurements()
{
}

void CheckDialogKDV::StartBdMeasurements()
{
    BdTimer->start();
}

void CheckDialogKDV::StopBdMeasurements()
{
    BdTimer->stop();
}

void CheckDialogKDV::USBUpdate()
{

    if (Commands::GetBd(1, &ChKDV->Bda_block, sizeof(CheckKDV::Bda_in_struct)) == NOERROR)
    {
        ChKDV->FillBda_block(this);
    }

    if (Commands::GetBd(2, &ChKDV->Bd2_Gr1, sizeof(CheckKDV::Bd2)) == NOERROR)
    {
        ChKDV->FillBd2_Gr1(this);
    }

    if (Commands::GetBd(3, &ChKDV->Bd3_Gr2, sizeof(CheckKDV::Bd2)) == NOERROR)
    {
        ChKDV->FillBd3_Gr2(this);
    }

    if (Commands::GetBd(4, &ChKDV->Bd4_U0, sizeof(CheckKDV::Bd4)) == NOERROR)
    {
        ChKDV->FillBd4_U0(this);
    }

    if (Commands::GetBd(5, &ChKDV->Bd5_UI_Harm, sizeof(CheckKDV::Bd5)) == NOERROR)
    {
        ChKDV->FillBd5_UI_Harm(this);
    }

    if (Commands::GetBd(6, &ChKDV->Bd6_U0, sizeof(CheckKDV::Bd4)) == NOERROR)
    {
        ChKDV->FillBd6_U0(this);
    }

    if (Commands::GetBd(7, &ChKDV->Bd7_UI_Harm, sizeof(CheckKDV::Bd5)) == NOERROR)
    {
        ChKDV->FillBd7_UI_Harm(this);
    }

    if (Commands::GetBd(9, &ChKDV->Bd9_Resurs, sizeof(CheckKDV::Bda_RESURS)) == NOERROR)
    {
        ChKDV->FillBd9_Resurs(this);
    }

    if (Commands::GetBd(10, &ChKDV->Bd10_Power, sizeof(CheckKDV::Bd10)) == NOERROR)
    {
        ChKDV->FillBd10_Power(this);
    }

    if (Commands::GetBd(11, &ChKDV->Bd11_Model, sizeof(CheckKDV::Bda_MODEL)) == NOERROR)
    {
        ChKDV->FillBd11_Model(this);
    }

    if (Commands::GetBd(13, &ChKDV->Bd13_Temp, sizeof(CheckKDV::Bda_TEMP)) == NOERROR)
    {
        ChKDV->FillBd13_Temp(this);
    }
}

void CheckDialogKDV::onModbusStateChanged(ConnectionStates state)
{
    if (state == ConnectionStates::ConnectedState)
        EMessageBox::information(this, "Успешно", "Связь по MODBUS установлена");
}

void CheckDialogKDV::SetPredAlarmColor(quint8 *PredAlarm)
{

    for (int i = 0; i < 18; i++)
    {
        if ((i >= 0) && (i < 3))
        {
            if (PredAlarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(1000 + i),
                    TABCOLORA1); // TABCOLORA1
            else
                WDFunc::SetLBLTColor(this, QString::number(1000 + i), ACONFOCLR);
        }

        if ((i >= 3) && (i < 6))
        {
            if (PredAlarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(1100 + i - 3),
                    TABCOLORA1); // TABCOLORA1
            else
                WDFunc::SetLBLTColor(this, QString::number(1100 + i - 3), ACONFOCLR);
        }

        if ((i >= 7) && (i < 10))
        {
            if (PredAlarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(1000 + i - 7),
                    TABCOLORA1); // TABCOLORA1
        }

        if ((i >= 10) && (i < 13))
        {
            if (PredAlarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(2429 + i - 10),
                    TABCOLORA1); // TABCOLORA1
            else
                WDFunc::SetLBLTColor(this, QString::number(2429 + i - 10), ACONFOCLR);
        }
        else if ((i >= 13) && (i < 16))
        {
            if (PredAlarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(2426 + i - 13),
                    TABCOLORA1); // TABCOLORA1
            else
                WDFunc::SetLBLTColor(this, QString::number(2426 + i - 13), ACONFOCLR);
        }
        else if (i == 17)
        {
            if (PredAlarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(2432),
                    TABCOLORA1); // TABCOLORA1
            else
                WDFunc::SetLBLTColor(this, QString::number(2432), ACONFOCLR);
        }
    }
}

void CheckDialogKDV::SetAlarmColor(quint8 *Alarm)
{

    for (int i = 0; i < 7; i++)
    {
        if (i < 3)
        {
            if (Alarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(2429 + i), REDCOLOR);
        }
        else if ((i >= 3) && (i < 6))
        {
            if (Alarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(2426 + i - 3), REDCOLOR);
        }
        else if (i == 6)
        {
            if (Alarm[i] == 1)
                WDFunc::SetLBLTColor(this, QString::number(2432), REDCOLOR);
        }
    }
}
void CheckDialogKDV::UpdateFlData(IEC104Thread::FlSignals104 *Signal)
{
    IEC104Thread::FlSignals104 sig = *new IEC104Thread::FlSignals104;
    int i;
    for (i = 0; i < Signal->SigNumber; i++)
    {
        sig = *(Signal + i);

        ChKDV->FillBd(this, QString::number(sig.fl.SigAdr), WDFunc::StringValueWithCheck(sig.fl.SigVal, 3));
    }
}

void CheckDialogKDV::UpdateSponData(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}

void CheckDialogKDV::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
{

    int i = 0;
    for (i = 0; i < Signal.size(); ++i)
    {

        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
            ChKDV->FillBd(
                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
        else
            ChKDV->FillBd(
                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
    }
}