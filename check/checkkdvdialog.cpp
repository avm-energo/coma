#include "checkkdvdialog.h"

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

CheckKDVDialog::CheckKDVDialog(QWidget *parent) : AbstractCheckDialog(parent)
{
    EParent = parent;
    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 6;
    ChKDV = new CheckKDV;
    Ch = new Check;
    //    BdNum = 11;
    setAttribute(Qt::WA_DeleteOnClose);

    //    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    //    SetBd(1, &ChKDV->Bda_block, sizeof(CheckKDV::Bda_in_struct));
    //    SetBd(2, &ChKDV->Bd_block2, sizeof(CheckKDV::Bd_2_3));
    //    SetBd(3, &ChKDV->Bd_block3, sizeof(CheckKDV::Bd_2_3));
    //    SetBd(4, &ChKDV->Bd_block4, sizeof(CheckKDV::Bd_4_6));
    //    SetBd(5, &ChKDV->Bd5_UI_Harm, sizeof(CheckKDV::Bd5));
    //    SetBd(6, &ChKDV->Bd_block6, sizeof(CheckKDV::Bd_4_6));
    //    SetBd(7, &ChKDV->Bd7_UI_Harm, sizeof(CheckKDV::Bd5));
    //    SetBd(9, &ChKDV->Bd_block9, sizeof(CheckKDV::Bda_RESURS));
    //    SetBd(10, &ChKDV->Bd_block10, sizeof(CheckKDV::Bd10));
    //    SetBd(11, &ChKDV->Bd11, sizeof(CheckKDV::Bda_MODEL));

    //    SetBd(13, &ChKDV->Bd_block13, sizeof(CheckKDV::Bda_TEMP));

    sl = QStringList { "Основные", "Модель", "Ресурс", "1-я гармоника", "Несимметрия" };

    BdUINum = sl.size();

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
}

QWidget *CheckKDVDialog::BdUI(int bdnum)
{
    switch (bdnum)
    {

    case 0:
        return ChKDV->Bd1W(this);

    case 1:
        return ChKDV->Bd2W(this);

    case 2:
        return ChKDV->Bd3W(this);

    case 3:
        return ChKDV->Bd4W(this);
    case 4:
        return ChKDV->Bd5W(this);

    default:
        return new QWidget;
    }
}

void CheckKDVDialog::PrepareHeadersForFile(int row)
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

void CheckKDVDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}

void CheckKDVDialog::ChooseValuesToWrite() { }
void CheckKDVDialog::SetDefaultValuesToWrite() { }
void CheckKDVDialog::PrepareAnalogMeasurements() { }

void CheckKDVDialog::setConnections()
{
    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &CheckKDVDialog::updateFloatData);
}

/*void CheckKDVDialog::USBUpdate()
{
    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw0");
    if (CheckTW == nullptr)
    {
        qDebug("CheckTW is null");
        return;
    }

    if (CheckTW->currentIndex() == IndexWd.at(0))
    {
        if (Commands::GetBd(0, &ChKDV->Bd_block0, sizeof(CheckKDV::Bd0)) == Error::Msg::NoError)
            ChKDV->FillBd0(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
    {
        if (Commands::GetBd(2, &ChKDV->Bd_block2, sizeof(CheckKDV::Bd_2_3)) == Error::Msg::NoError)
            ChKDV->FillBd2(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
    {
        if (Commands::GetBd(3, &ChKDV->Bd_block3, sizeof(CheckKDV::Bd_2_3)) == Error::Msg::NoError)
            ChKDV->FillBd3(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(4))
    {
        if (Commands::GetBd(4, &ChKDV->Bd_block4, sizeof(CheckKDV::Bd_4_6)) == Error::Msg::NoError)
            ChKDV->FillBd4(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(4))
    {
        if (Commands::GetBd(6, &ChKDV->Bd_block6, sizeof(CheckKDV::Bd_4_6)) == Error::Msg::NoError)
            ChKDV->FillBd6(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
    {
        if (Commands::GetBd(8, &ChKDV->Bd_block8, sizeof(CheckKDV::Bd8)) == Error::Msg::NoError)
            ChKDV->FillBd8(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(2))
    {
        if (Commands::GetBd(9, &ChKDV->Bd_block9, sizeof(CheckKDV::Bd9)) == Error::Msg::NoError)
            ChKDV->FillBd9(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
    {
        if (Commands::GetBd(10, &ChKDV->Bd_block10, sizeof(CheckKDV::Bd10)) == Error::Msg::NoError)
            ChKDV->FillBd10(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(11, &ChKDV->Bd_block11, sizeof(CheckKDV::Bd11)) == Error::Msg::NoError)
            ChKDV->FillBd11(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0))
    {
        if (Commands::GetBd(13, &ChKDV->Bd_block13, sizeof(CheckKDV::Bd13)) == Error::Msg::NoError)
            ChKDV->FillBd13(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0))
    {
        if (Commands::GetBd(17, &ChKDV->Bd_block17, sizeof(CheckKDV::Bd17)) == Error::Msg::NoError)
            ChKDV->FillBd17(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(2))
    {
        if (Commands::GetBd(18, &ChKDV->Bd_block18, sizeof(CheckKDV::Bd18)) == Error::Msg::NoError)
            ChKDV->FillBd18(this);
    }
} */

// void CheckKDVDialog::ETHUpdate()
//{
//    updateFloatData();
//}

// void CheckKDVDialog::MBSUpdate()
//{
//}

void CheckKDVDialog::SetWarnColor(int position, bool value)
{
    if ((position >= 0) && (position < 3))
        WDFunc::SetLBLTColor(this, QString::number(1000 + position), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);

    if ((position >= 3) && (position < 6))
        WDFunc::SetLBLTColor(
            this, QString::number(1100 + position - 3), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);

    if ((position >= 7) && (position < 10))
        WDFunc::SetLBLTColor(
            this, QString::number(1000 + position - 7), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);

    if ((position >= 10) && (position < 13))
        WDFunc::SetLBLTColor(
            this, QString::number(2429 + position - 10), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
    if ((position >= 13) && (position < 16))
        WDFunc::SetLBLTColor(
            this, QString::number(2426 + position - 13), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
    if (position == 17)
        WDFunc::SetLBLTColor(this, "2432", (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
}

void CheckKDVDialog::SetAlarmColor(int position, bool value)
{
    if (position < 3)
        WDFunc::SetLBLTColor(this, QString::number(2429 + position), (value) ? Colors::REDCOLOR : Colors::ACONFOCLR);
    else if ((position >= 3) && (position < 6))
        WDFunc::SetLBLTColor(
            this, QString::number(2426 + position - 3), (value) ? Colors::REDCOLOR : Colors::ACONFOCLR);
    else if (position == 6)
        WDFunc::SetLBLTColor(this, QString::number(2432), (value) ? Colors::REDCOLOR : Colors::ACONFOCLR);
}

void CheckKDVDialog::updateFloatData()
{
    QList<DataTypes::SignalsStruct> list;
    DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
    if (!list.isEmpty())
    {
        foreach (DataTypes::SignalsStruct signal, list)
        {
            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
            ChKDV->FillBd(this, QString::number(fwt.sigAdr), WDFunc::StringValueWithCheck(fwt.sigVal, 3));
        }
    }
}

/*void CheckKDVDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
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
} */
