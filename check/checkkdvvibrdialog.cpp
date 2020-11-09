#include "checkkdvvibrdialog.h"

#include "../config/config.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
//#include "../usb/commands.h"
#include "../gen/datamanager.h"
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

CheckKDVVibrDialog::CheckKDVVibrDialog(QWidget *parent) : AbstractCheckDialog(parent)
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

// void CheckKDVVibrDialog::SetWarnColor(int position, bool value)
//{
//    Q_UNUSED(position)
//    Q_UNUSED(value)
//}

// void CheckKDVVibrDialog::SetAlarmColor(int position, bool value)
//{
//    Q_UNUSED(position)
//    Q_UNUSED(value)
//}

QWidget *CheckKDVVibrDialog::BdUI(int bdnum)
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

void CheckKDVVibrDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKDVVibrDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

// void CheckKDVVibrDialog::ChooseValuesToWrite() { }
// void CheckKDVVibrDialog::SetDefaultValuesToWrite() { }
// void CheckKDVVibrDialog::PrepareAnalogMeasurements() { }

// void CheckKDVVibrDialog::USBUpdate()
//{
//    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw2");
//    if (CheckTW == nullptr)
//    {
//        qDebug("CheckTW is null");
//        return;
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0))
//    {
//        if (Commands::GetBd(19, &ChVibrKDV->Bd_block19, sizeof(CheckVibrKDV::Bd19)) == Error::Msg::NoError)
//            ChVibrKDV->FillBd19(this);
//    }

//    if ((CheckTW->currentIndex() == IndexWd.at(1)) | (CheckTW->currentIndex() == IndexWd.at(2)))
//    {
//        if (Commands::GetBd(20, &ChVibrKDV->Bd_block20, sizeof(CheckVibrKDV::Bd20)) == Error::Msg::NoError)
//            ChVibrKDV->FillBd20(this);
//    }
//}

// void CheckKDVVibrDialog::updateFloatData()
//{
//    QList<DataTypes::SignalsStruct> list;
//    DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
//    if (!list.isEmpty())
//    {
//        foreach (DataTypes::SignalsStruct signal, list)
//        {
//            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
//            ChVibrKDV->FillBd(this, QString::number(fwt.sigAdr), WDFunc::StringValueWithCheck(fwt.sigVal, 3));
//        }
//    }
//}

// void CheckKDVVibrDialog::reqUpdate()
//{
//    updateFloatData();
//}

// void CheckKDVVibrDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
//{

//    int i = 0;
//    for (i = 0; i < Signal.size(); ++i)
//    {
//        // sig = *(Signal+i);
//        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
//            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
//            ChVibrKDV->FillBd(
//                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal,
//                3));
//        else
//            ChVibrKDV->FillBd(
//                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
//    }
//}
