#include "checkkivdialog.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
//#include "../usb/commands.h"
#include "../gen/datamanager.h"
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

CheckKIVDialog::CheckKIVDialog(QWidget *parent) : AbstractCheckDialog(parent)
{
    //    EParent = parent;
    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 6;
    ChKIV = new CheckKIV;
    Ch = new Check;
    m_stColor.resize(7);
    //    BdNum = 11;
    setAttribute(Qt::WA_DeleteOnClose);

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

void CheckKIVDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKIVDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

void CheckKIVDialog::ChooseValuesToWrite()
{
}
void CheckKIVDialog::SetDefaultValuesToWrite()
{
}
void CheckKIVDialog::PrepareAnalogMeasurements()
{
}

/*void CheckKIVDialog::USBUpdate()
{
    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw0");
    if (CheckTW == nullptr)
    {
        DBGMSG;
        return;
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(BdNum, &ChKIV->Bd_block1, sizeof(CheckKIV::Bd1)) == Error::Msg::NoError)
            ChKIV->FillBdUSB(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(5, &ChKIV->Bd_block5, sizeof(CheckKIV::Bd5)) == Error::Msg::NoError)
            ChKIV->FillBd5(this);
    }

    if (CheckTW->currentIndex() == IndexWd.at(1))
    {
        if (Commands::GetBd(8, &ChKIV->Bd_block8, sizeof(CheckKIV::Bd8)) == Error::Msg::NoError)
            ChKIV->FillBd8(this);
    }
} */

void CheckKIVDialog::setConnections()
{
    //    if ((m_updatesEnabled) && m_timerCounter) // every second tick of the timer
    //    {
    //        getFloatData();
    //        getSPData();
    //        updateFloatData();
    //        updateSPData();
    //        m_timerCounter = !m_timerCounter;
    //    }
    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &CheckKIVDialog::updateFloatData);
    connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, this, &CheckKIVDialog::updateSPData);
}

void CheckKIVDialog::update()
{
    iface()->reqFloats(2400, 7); // Bd5
}

void CheckKIVDialog::updateFloatData(DataTypes::FloatStruct &fl)
{
    if ((m_updatesEnabled) && m_timerCounter) // every second tick of the timer
                                              //    QList<DataTypes::SignalsStruct> list;
        //    DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
        //    if (!list.isEmpty())
        //    {
        //        foreach (DataTypes::SignalsStruct signal, list)
        //        {
        //            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
        ChKIV->FillBd(this, QString::number(fl.sigAdr), WDFunc::StringValueWithCheck(fl.sigVal, 3));
    //        }
    //    }
}

void CheckKIVDialog::updateSPData(DataTypes::SinglePointWithTimeStruct &sp)
{
    //    QList<DataTypes::SignalsStruct> list;
    //    DataManager::getSignals(3011, 3035, DataTypes::SignalTypes::SinglePointWithTime, list);
    //    if (!list.isEmpty())
    //    {
    //        foreach (DataTypes::SignalsStruct signal, list)
    //        {
    //            DataTypes::SinglePointWithTimeStruct sp =
    //            qvariant_cast<DataTypes::SinglePointWithTimeStruct>(signal.data);
    if ((sp.sigAdr >= 3011) && (sp.sigAdr <= 3013))
        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 2011),
            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
                                                                        //        }
    if ((sp.sigAdr >= 3014) && (sp.sigAdr <= 3016))
        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 1914),
            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
                                                                        //            }
    if ((sp.sigAdr >= 3018) && (sp.sigAdr <= 3020))
        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 2018),
            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
    if ((sp.sigAdr >= 3021) && (sp.sigAdr <= 3023))
    {
        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 2021),
            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
        if (sp.sigVal == 0)
            m_stColor[sp.sigAdr - 3021] = 1;
    }
    if ((sp.sigAdr >= 3024) && (sp.sigAdr <= 3026))
    {
        if (sp.sigVal == 1)
        {
            m_stColor[sp.sigAdr - 3024] = 0;
            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 595), Colors::REDCOLOR);
        }
        else
        {
            if (!m_stColor[sp.sigAdr - 3024])
                WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 595), Colors::TABCOLORA1);
        }
    }
    if ((sp.sigAdr >= 3027) && (sp.sigAdr <= 3029))
    {
        if (sp.sigVal == 1)
            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 601), Colors::TABCOLORA1);
        else
        {
            m_stColor[sp.sigAdr - 3024] = 1;
            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 601), Colors::ACONFOCLR);
        }
    }
    if ((sp.sigAdr >= 3030) && (sp.sigAdr < 3033))
    {
        if (sp.sigVal == 1)
        {
            m_stColor[sp.sigAdr - 3027] = 0;
            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 604), Colors::REDCOLOR);
        }
        else
        {
            if (!m_stColor[sp.sigAdr - 3027])
                WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 604), Colors::TABCOLORA1);
        }
    }
    if (sp.sigAdr == 3034)
    {
        if (sp.sigVal == 1)
            WDFunc::SetLBLTColor(this, "2432", Colors::TABCOLORA1);
        else
        {
            m_stColor[6] = 1;
            WDFunc::SetLBLTColor(this, "2432", Colors::ACONFOCLR);
        }
    }
    if (sp.sigAdr == 3035)
    {
        if (sp.sigVal == 1)
        {
            m_stColor[6] = 0;
            WDFunc::SetLBLTColor(this, "2432", Colors::REDCOLOR);
        }
        else
        {
            if (!m_stColor[6])
                WDFunc::SetLBLTColor(this, "2432", Colors::TABCOLORA1);
        }
    }
    //        }
    //    }
}

/*void CheckKIVDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
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
} */

void CheckKIVDialog::SetWarnColor(int position, bool value)
{
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
}

void CheckKIVDialog::getFloatData()
{
}

void CheckKIVDialog::getSPData()
{
}

void CheckKIVDialog::SetAlarmColor(int position, bool value)
{
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
}
