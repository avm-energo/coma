#include "checkdialogvibrkdv.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
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

CheckDialogVibrKDV::CheckDialogVibrKDV(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 12;
    Ch = new Check;
    ChVibrKDV = new CheckVibrKDV;

    setAttribute(Qt::WA_DeleteOnClose);

    if (Config::MTB_A2)

        sl = QStringList { "Вибрации", "Вибрации", "Вибрации" };

    BdUINum = sl.size();

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
}

QWidget *CheckDialogVibrKDV::BdUI(int bdnum)
{
    switch (bdnum)
    {

    case 0:
        return ChVibrKDV->Bd1W(this);
    case 1:
        return ChVibrKDV->Bd2W(this);
    case 2:
        return ChVibrKDV->Bd3W(this);

    default:
        return new QWidget;
    }
}
void CheckDialogVibrKDV::RefreshAnalogValues(int bdnum) { Q_UNUSED(bdnum) }

void CheckDialogVibrKDV::PrepareHeadersForFile(int row) { Q_UNUSED(row) }

void CheckDialogVibrKDV::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}
QWidget *CheckDialogVibrKDV::CustomTab() { return nullptr; }
void CheckDialogVibrKDV::ChooseValuesToWrite() { }
void CheckDialogVibrKDV::SetDefaultValuesToWrite() { }
void CheckDialogVibrKDV::PrepareAnalogMeasurements() { }

void CheckDialogVibrKDV::StartBdMeasurements() { BdTimer->start(); }

void CheckDialogVibrKDV::StopBdMeasurements() { BdTimer->stop(); }

void CheckDialogVibrKDV::USBUpdate()
{
    if (Commands::GetBd(19, &ChVibrKDV->Bd_block19, sizeof(CheckVibrKDV::Bd19)) == NOERROR)
    {
        ChVibrKDV->FillBd19(this);
    }

    if (Commands::GetBd(20, &ChVibrKDV->Bd_block20, sizeof(CheckVibrKDV::Bd20)) == NOERROR)
    {
        ChVibrKDV->FillBd20(this);
    }
}

void CheckDialogVibrKDV::UpdateFlData(IEC104Thread::FlSignals104 *Signal)
{
    IEC104Thread::FlSignals104 sig = *new IEC104Thread::FlSignals104;
    int i;
    for (i = 0; i < Signal->SigNumber; i++)
    {
        sig = *(Signal + i);

        ChVibrKDV->FillBd(this, QString::number(sig.fl.SigAdr), WDFunc::StringValueWithCheck(sig.fl.SigVal, 3));
    }
}

void CheckDialogVibrKDV::UpdateSponData(IEC104Thread::SponSignals *Signal) { Q_UNUSED(Signal); }

void CheckDialogVibrKDV::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
{

    int i = 0;
    for (i = 0; i < Signal.size(); ++i)
    {
        // sig = *(Signal+i);
        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
            ChVibrKDV->FillBd(
                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
        else
            ChVibrKDV->FillBd(
                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
    }
}

void CheckDialogVibrKDV::onModbusStateChanged()
{

    if (Board::GetInstance()->connectionState() == Board::ConnectionState::ConnectedState)
        QMessageBox::information(this, "Успешно", "Связь по MODBUS установлена");
}

void CheckDialogVibrKDV::SetPredAlarmColor(quint8 *PredAlarm) { Q_UNUSED(PredAlarm); }

void CheckDialogVibrKDV::SetAlarmColor(quint8 *Alarm) { Q_UNUSED(Alarm); }
