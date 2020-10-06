#include "checkktfdialog.h"

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

CheckKTFDialog::CheckKTFDialog(BoardTypes board, QWidget *parent) : AbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 17;
    ChKTF = new CheckKTF;
    Ch = new Check;

    setAttribute(Qt::WA_DeleteOnClose);

    //    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    //    SetBd(17, &ChKTF->Bd_block13, sizeof(CheckKTF::Bd13));

    if (Config::MTB_A2) //(ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8)

        sl = QStringList { "Основные", "Модель", "Ресурс", "1-я гармоника", "Несимметрия" };

    BdUINum = sl.size();
    // m_RCount = rcount;

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
}

QWidget *CheckKTFDialog::BdUI(int bdnum)
{
    switch (bdnum)
    {

    case 0:
        //        return ChKTF->Bd1W(m_RCount,this);
        return ChKTF->Bd1W(this);

    case 1:
        return ChKTF->Bd2W(this);

    case 2:
        return ChKTF->Bd3W(this);

    case 3:
        return ChKTF->Bd4W(this);
    case 4:
        return ChKTF->Bd5W(this);

    default:
        return new QWidget;
    }
}

void CheckKTFDialog::RefreshAnalogValues(int bdnum)
{
    Q_UNUSED(bdnum)
}

void CheckKTFDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKTFDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}
QWidget *CheckKTFDialog::CustomTab()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(ChKTF->Bd1W(this));
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
void CheckKTFDialog::ChooseValuesToWrite()
{
}
void CheckKTFDialog::SetDefaultValuesToWrite()
{
}
void CheckKTFDialog::PrepareAnalogMeasurements()
{
}

void CheckKTFDialog::USBUpdate()
{
    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw1");
    if (CheckTW == nullptr)
    {
        DBGMSG;
        return;
    }

    if (CheckTW->currentIndex() == IndexWd.at(0))
    {
        if (Commands::GetBd(13, &ChKTF->Bd_block13, sizeof(CheckKTF::Bd13)) == Error::Msg::NoError)
            ChKTF->FillBd13(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0))
    {
        if (Commands::GetBd(0, &ChKTF->Bd_block0, sizeof(CheckKTF::Bd0)) == Error::Msg::NoError)
            ChKTF->FillBd0(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0))
    {
        if (Commands::GetBd(17, &ChKTF->Bd_block17, sizeof(CheckKTF::Bd17)) == Error::Msg::NoError)
            ChKTF->FillBd17(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
    {
        if (Commands::GetBd(10, &ChKTF->Bd_block10, sizeof(CheckKTF::Bd10)) == Error::Msg::NoError)
            ChKTF->FillBd10(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3)
        || CheckTW->currentIndex() == IndexWd.at(4))
    {
        if (Commands::GetBd(2, &ChKTF->Bd_block2, sizeof(CheckKTF::Bd2)) == Error::Msg::NoError)
            ChKTF->FillBd2(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3)
        || CheckTW->currentIndex() == IndexWd.at(4))
    {
        if (Commands::GetBd(3, &ChKTF->Bd_block3, sizeof(CheckKTF::Bd3)) == Error::Msg::NoError)
            ChKTF->FillBd3(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
    {
        if (Commands::GetBd(8, &ChKTF->Bd_block8, sizeof(CheckKTF::Bd8)) == Error::Msg::NoError)
            ChKTF->FillBd8(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(11, &ChKTF->Bd_block11, sizeof(CheckKTF::Bd11)) == Error::Msg::NoError)
            ChKTF->FillBd11(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(2))
    {
        if (Commands::GetBd(9, &ChKTF->Bd_block9, sizeof(CheckKTF::Bd9)) == Error::Msg::NoError)
            ChKTF->FillBd9(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(2))
    {
        if (Commands::GetBd(18, &ChKTF->Bd_block18, sizeof(CheckKTF::Bd18)) == Error::Msg::NoError)
            ChKTF->FillBd18(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(4))
    {
        if (Commands::GetBd(4, &ChKTF->Bd_block4, sizeof(CheckKTF::Bd4)) == Error::Msg::NoError)
            ChKTF->FillBd4(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(4))
    {
        if (Commands::GetBd(6, &ChKTF->Bd_block6, sizeof(CheckKTF::Bd6)) == Error::Msg::NoError)
            ChKTF->FillBd6(this);
    }
}

void CheckKTFDialog::UpdateFlData(IEC104Thread::FlSignals104 *Signal)
{
    for (int i = 0; i < Signal->SigNumber; i++)
    {
        ChKTF->FillBd(
            this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal + i)->fl.SigVal, 3));
    }
}

void CheckKTFDialog::UpdateSponData(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal)
}

void CheckKTFDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
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

void CheckKTFDialog::SetPredAlarmColor(quint8 *PredAlarm)
{
    Q_UNUSED(PredAlarm)
}

void CheckKTFDialog::SetAlarmColor(quint8 *Alarm)
{
    Q_UNUSED(Alarm)
}
