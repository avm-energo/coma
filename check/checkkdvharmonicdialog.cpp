#include "checkkdvharmonicdialog.h"

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

CheckKDVHarmonicDialog::CheckKDVHarmonicDialog(QWidget *parent) : AbstractCheckDialog(parent)
{

    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 12;
    Ch = new Check;
    ChHarmKDV = new CheckHarmonicKDV;

    setAttribute(Qt::WA_DeleteOnClose);

    if (Config::MTB_A2)

        sl = QStringList { "Гармоники U 2-11", "Гармоники U 12-21", "Гармоники U 22-31", "Гармоники U 32-41",
            "Гармоники U 42-51", "Гармоники U 52-61", "Гармоники I 2-11", "Гармоники I 12-21", "Гармоники I 22-31",
            "Гармоники I 32-41", "Гармоники I 42-51", "Гармоники I 52-61" };

    BdUINum = sl.size();

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
}

QWidget *CheckKDVHarmonicDialog::BdUI(int bdnum)
{
    switch (bdnum)
    {

    case 0:
        return ChHarmKDV->Bd6W(this);
    case 1:
        return ChHarmKDV->Bd7W(this);
    case 2:
        return ChHarmKDV->Bd8W(this);
    case 3:
        return ChHarmKDV->Bd9W(this);
    case 4:
        return ChHarmKDV->Bd10W(this);
    case 5:
        return ChHarmKDV->Bd11W(this);
    case 6:
        return ChHarmKDV->Bd12W(this);
    case 7:
        return ChHarmKDV->Bd13W(this);
    case 8:
        return ChHarmKDV->Bd14W(this);
    case 9:
        return ChHarmKDV->Bd15W(this);
    case 10:
        return ChHarmKDV->Bd16W(this);
    case 11:
        return ChHarmKDV->Bd17W(this);

    default:
        return new QWidget;
    }
}
void CheckKDVHarmonicDialog::RefreshAnalogValues(int bdnum) { Q_UNUSED(bdnum) }

void CheckKDVHarmonicDialog::PrepareHeadersForFile(int row) { Q_UNUSED(row) }

void CheckKDVHarmonicDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}
// QWidget *CheckDialogHarmonicKDV::CustomTab()
//{
//    QWidget *w = new QWidget;
//    QVBoxLayout *lyout = new QVBoxLayout;
//    QHBoxLayout *hlyout = new QHBoxLayout;
//    lyout->addWidget(ChHarmKTF->Bd1W(this));
//    QPushButton *pb = new QPushButton("Начать измерения Bd");
//    connect(pb, SIGNAL(clicked(bool)), this, SLOT(StartBdMeasurements()));
//    hlyout->addWidget(pb);
//    pb = new QPushButton("Остановить измерения Bd");
//    connect(pb, SIGNAL(clicked(bool)), this, SLOT(StopBdMeasurements()));
//    hlyout->addWidget(pb);
//    lyout->addLayout(hlyout);
//    w->setLayout(lyout);
//    return nullptr;
//}
void CheckKDVHarmonicDialog::ChooseValuesToWrite() { }
void CheckKDVHarmonicDialog::SetDefaultValuesToWrite() { }
void CheckKDVHarmonicDialog::PrepareAnalogMeasurements() { }

void CheckKDVHarmonicDialog::USBUpdate()
{
    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw1");
    if (CheckTW == nullptr)
    {
        DBGMSG;
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        if (CheckTW->currentIndex() == IndexWd.at(i))
        {
            if (Commands::GetBd(5, &ChHarmKDV->Bd_block5, sizeof(CheckHarmonicKDV::Bd_5_7)) == Error::Msg::NoError)
                ChHarmKDV->FillBd5(this);
        }
    }

    for (int i = 6; i < 12; i++)
    {
        if (CheckTW->currentIndex() == IndexWd.at(i))
        {
            if (Commands::GetBd(7, &ChHarmKDV->Bd_block7, sizeof(CheckHarmonicKDV::Bd_5_7)) == Error::Msg::NoError)
                ChHarmKDV->FillBd7(this);
        }
    }
}

void CheckKDVHarmonicDialog::UpdateFlData(IEC104Thread::FlSignals104 *Signal)
{
    for (int i = 0; i < Signal->SigNumber; i++)
    {

        ChHarmKDV->FillBd(
            this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal + i)->fl.SigVal, 3));
    }
}

void CheckKDVHarmonicDialog::UpdateSponData(IEC104Thread::SponSignals *Signal) { Q_UNUSED(Signal) }

void CheckKDVHarmonicDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
{

    int i = 0;
    for (i = 0; i < Signal.size(); ++i)
    {
        // sig = *(Signal+i);
        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
            ChHarmKDV->FillBd(
                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
        else
            ChHarmKDV->FillBd(
                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
    }
}

void CheckKDVHarmonicDialog::SetWarnColor(int position, bool value)
{
    Q_UNUSED(position)
    Q_UNUSED(value)
}

void CheckKDVHarmonicDialog::SetAlarmColor(int position, bool value)
{
    Q_UNUSED(position)
    Q_UNUSED(value)
}
