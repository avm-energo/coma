#include "chekdialogktf.h"

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

ChekDialogKTF::ChekDialogKTF(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: " + QString(UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 17;
    ChKTF = new CheckKTF;
    Ch = new Check;

    setAttribute(Qt::WA_DeleteOnClose);

    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(17, &ChKTF->Bd_block13, sizeof(CheckKTF::Bd13));

    if (Config::MTB_A2) //(ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8)
        sl = QStringList() << "Основные"
                           << "Модель"
                           << "Ресурс"
                           << "1-я гармоника"
                           << "Несиметрия"
                           << "Гармоники U 2-11"
                           << "Гармоники U 12-21"
                           << "Гармоники U 22-31"
                           << "Гармоники U 32-41"
                           << "Гармоники U 42-51"
                           << "Гармоники U 52-61"
                           << "Гармоники I 2-11"
                           << "Гармоники I 12-21"
                           << "Гармоники I 22-31"
                           << "Гармоники I 32-41"
                           << "Гармоники I 42-51"
                           << "Гармоники I 52-61";

    BdUINum = sl.size();

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
}

QWidget *ChekDialogKTF::BdUI(int bdnum)
{
    switch (bdnum)
    {

    case 0:
        return ChKTF->Bd1W(this);

    case 1:
        return ChKTF->Bd2W(this);

    case 2:
        return ChKTF->Bd3W(this);

    case 3:
        return ChKTF->Bd4W(this);

    case 4:
        return ChKTF->Bd5W(this);
    case 5:
        return ChKTF->Bd6W(this);
    case 6:
        return ChKTF->Bd7W(this);
    case 7:
        return ChKTF->Bd8W(this);
    case 8:
        return ChKTF->Bd9W(this);
    case 9:
        return ChKTF->Bd10W(this);
    case 10:
        return ChKTF->Bd11W(this);
    case 11:
        return ChKTF->Bd12W(this);
    case 12:
        return ChKTF->Bd13W(this);
    case 13:
        return ChKTF->Bd14W(this);
    case 14:
        return ChKTF->Bd15W(this);
    case 15:
        return ChKTF->Bd16W(this);
    case 16:
        return ChKTF->Bd17W(this);

        ;

    default:
        return new QWidget;
    }
}

void ChekDialogKTF::RefreshAnalogValues(int bdnum)
{
    Q_UNUSED(bdnum)
}

void ChekDialogKTF::PrepareHeadersForFile(int row)
{
    QString phase[4] = { "A:", "B:", "C:", "сред.:" };

    for (int i = 0; i < 4; i++)
    {

        xlsx->write(row, i + 3, QVariant(("Ueff ф") + phase[i] + ", кВ"));
        xlsx->write(row, i + 7, QVariant("Ieff ф" + phase[i] + ", А"));
        xlsx->write(row, i + 11, QVariant("P ф." + phase[i] + ", кВт"));
        xlsx->write(row, i + 15, QVariant("Q ф." + phase[i] + ", кВар"));
        xlsx->write(row, i + 19, QVariant("S ф." + phase[i] + ", кВА"));
        xlsx->write(row, i + 23, QVariant("CosPhi ф." + phase[i] + ", град."));
        xlsx->write(row, i + 27, QVariant("CosPhi ф." + phase[i] + ", град."));
    }
    xlsx->write(row, 31, QVariant("Tmk, °С"));
    xlsx->write(row, 32, QVariant("ExtTemp,°С"));
    xlsx->write(row, 33, QVariant("Tamb, °С"));
    xlsx->write(row, 33, QVariant("Freq, Гц"));
}

void ChekDialogKTF::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}
QWidget *ChekDialogKTF::CustomTab()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(ChKTF->Bd1W(this));
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
void ChekDialogKTF::ChooseValuesToWrite()
{
}
void ChekDialogKTF::SetDefaultValuesToWrite()
{
}
void ChekDialogKTF::PrepareAnalogMeasurements()
{
}

void ChekDialogKTF::StartBdMeasurements()
{
    BdTimer->start();
}

void ChekDialogKTF::StopBdMeasurements()
{
    BdTimer->stop();
}

void ChekDialogKTF::USBUpdate()
{
    if (Commands::GetBd(13, &ChKTF->Bd_block13, sizeof(CheckKTF::Bd13)) == NOERROR)
    {
        ChKTF->FillBd13(this);
    }
    if (Commands::GetBd(0, &ChKTF->Bd_block0, sizeof(CheckKTF::Bd0)) == NOERROR)
    {
        ChKTF->FillBd0(this);
    }

    if (Commands::GetBd(17, &ChKTF->Bd_block17, sizeof(CheckKTF::Bd17)) == NOERROR)
    {
        ChKTF->FillBd17(this);
        // Ch84->FillBd2(this);
    }

    if (Commands::GetBd(10, &ChKTF->Bd_block10, sizeof(CheckKTF::Bd10)) == NOERROR)
    {
        ChKTF->FillBd10(this);
        // Ch84->FillBd2(this);
    }

    if (Commands::GetBd(2, &ChKTF->Bd_block2, sizeof(CheckKTF::Bd2)) == NOERROR)
    {
        ChKTF->FillBd2(this);
    }
    if (Commands::GetBd(3, &ChKTF->Bd_block3, sizeof(CheckKTF::Bd3)) == NOERROR)
    {
        ChKTF->FillBd3(this);
    }
    if (Commands::GetBd(8, &ChKTF->Bd_block8, sizeof(CheckKTF::Bd8)) == NOERROR)
    {
        ChKTF->FillBd8(this);
    }
    if (Commands::GetBd(11, &ChKTF->Bd_block11, sizeof(CheckKTF::Bd11)) == NOERROR)
    {
        ChKTF->FillBd11(this);
    }
    if (Commands::GetBd(9, &ChKTF->Bd_block9, sizeof(CheckKTF::Bd9)) == NOERROR)
    {
        ChKTF->FillBd9(this);
    }
    if (Commands::GetBd(18, &ChKTF->Bd_block9, sizeof(CheckKTF::Bd18)) == NOERROR)
    {
        ChKTF->FillBd18(this);
    }

    if (Commands::GetBd(4, &ChKTF->Bd_block4, sizeof(CheckKTF::Bd4)) == NOERROR)
    {
        ChKTF->FillBd4(this);
    }
    if (Commands::GetBd(5, &ChKTF->Bd_block5, sizeof(CheckKTF::Bd5)) == NOERROR)
    {
        ChKTF->FillBd5(this);
    }
    if (Commands::GetBd(6, &ChKTF->Bd_block6, sizeof(CheckKTF::Bd6)) == NOERROR)
    {
        ChKTF->FillBd6(this);
    }
    if (Commands::GetBd(7, &ChKTF->Bd_block7, sizeof(CheckKTF::Bd7)) == NOERROR)
    {
        ChKTF->FillBd7(this);
    }
}

void ChekDialogKTF::UpdateFlData(IEC104Thread::FlSignals104 *Signal)
{
    IEC104Thread::FlSignals104 sig = *new IEC104Thread::FlSignals104;
    int i;
    for (i = 0; i < Signal->SigNumber; i++)
    {
        sig = *(Signal + i);
        // WDFunc::SetLBLText(Ch, QString::number((Signal+i)->fl.SigAdr),
        // WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal)); if((Signal+i)->fl.SigAdr == 101 ||
        // (Signal+i)->fl.SigAdr
        // == 102) Ch->FillBd0(this, QString::number((Signal+i)->fl.SigAdr),
        // WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));

        // if((Signal+i)->fl.SigAdr >= 1000 || (Signal+i)->fl.SigAdr <= 1009)
        ChKTF->FillBd(this, QString::number(sig.fl.SigAdr), WDFunc::StringValueWithCheck(sig.fl.SigVal, 3));
    }
}

void ChekDialogKTF::UpdateSponData(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}

void ChekDialogKTF::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
{

    // ModBusSignal sig = *new ModBusSignal;
    int i = 0;
    for (i = 0; i < Signal.size(); ++i)
    {
        // sig = *(Signal+i);
        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
            ChKTF->FillBd(
                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
        else
            ChKTF->FillBd(
                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
    }
}

void ChekDialogKTF::onModbusStateChanged(ConnectionStates state)
{
    if (state == ConnectionStates::ConnectedState)
        EMessageBox::information(this, "Успешно", "Связь по MODBUS установлена");
}

void ChekDialogKTF::SetPredAlarmColor(quint8 *PredAlarm)
{
    Q_UNUSED(PredAlarm);
}

void ChekDialogKTF::SetAlarmColor(quint8 *Alarm)
{
    Q_UNUSED(Alarm);
}
