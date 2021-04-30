#include "checkkdvharmonicdialog.h"

#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/modules.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QDebug>
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

    m_BdUIList = { { "Гармоники U 2-11", BdUWidget(1028, 2, 12) }, { "Гармоники U 12-21", BdUWidget(1038, 12, 22) },
        { "Гармоники U 22-31", BdUWidget(1048, 22, 32) }, { "Гармоники U 32-41", BdUWidget(1058, 32, 42) },
        { "Гармоники U 42-51", BdUWidget(1068, 42, 52) }, { "Гармоники U 52-62", BdUWidget(1078, 52, 63) },
        { "Гармоники I 2-11", BdIWidget(1428, 2, 12) }, { "Гармоники I 12-21", BdIWidget(1038, 12, 22) },
        { "Гармоники I 22-31", BdIWidget(1448, 22, 32) }, { "Гармоники I 32-41", BdIWidget(1458, 32, 42) },
        { "Гармоники I 42-51", BdIWidget(1068, 42, 52) }, { "Гармоники I 52-62", BdIWidget(1078, 52, 63) } };
    SetupUI();
    Timer->setInterval(ANMEASINT);
}

// QWidget *CheckKDVHarmonicDialog::BdUI(int bdnum)
//{
//    switch (bdnum)
//    {

//    case 0:
//        return ChHarmKDV->BdUWidget(this,1028,2,12 );
//    case 1:
//        return ChHarmKDV->BdUWidget(this,1038,12,22);
//    case 2:
//        return ChHarmKDV->BdUWidget(this,1048,22,32);
//    case 3:
//        return ChHarmKDV->BdUWidget(this,1058,32,42);
//    case 4:
//        return ChHarmKDV->BdUWidget(this,1068,42,52);
//    case 5:
//        return ChHarmKDV->BdUWidget(this,1078,52,63);
//    case 6:
//        return ChHarmKDV->BdIWidget(this,1428,2,12);
//    case 7:
//        return ChHarmKDV->BdIWidget(this,1438,12,22);
//    case 8:
//        return ChHarmKDV->BdIWidget(this,1448,22,32);
//    case 9:
//        return ChHarmKDV->BdIWidget(this,1458,32,42);
//    case 10:
//        return ChHarmKDV->BdIWidget(this,1468,42,52);
//    case 11:
//        return ChHarmKDV->BdIWidget(this,1478,52,63);

//    default:
//        return new QWidget;
//    }
//}
void CheckKDVHarmonicDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKDVHarmonicDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

UWidget *CheckKDVHarmonicDialog::BdIWidget(unsigned int value, int min, int max)
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox;
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Данные по гармоническому составу тока");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    int j, k, s;
    for (i = 0; i < 3; ++i)
    {
        j = 0;
        s = 0;
        k = 0;
        for (int h = min; h < max; h++)
        {
            if (k > 0)
                s = s + 2;
            k = s + 1;
            glyout->addWidget(WDFunc::NewLBL(this, phase[i] + "Гм. " + QString::number(h)), s, i, 1, 1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number((value + j) + i * 61), ValuesFormat,
                                  QString::number(h) + "-я гармоника тока в фазе" + phase[i] + "в % от 1-й", true),
                k, i, 1, 1);
            j++;
        }
    }

    quint32 val = max - min;

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { value, val } });
    return w;
}

UWidget *CheckKDVHarmonicDialog::BdUWidget(unsigned int value, int min, int max)
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox;
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Данные по гармоническому составу напряжения");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    int j, k, s;
    for (i = 0; i < 3; ++i)
    {
        j = 0;
        s = 0;
        k = 0;
        for (int h = min; h < max; h++)
        {
            if (k > 0)
                s = s + 2;
            k = s + 1;
            glyout->addWidget(WDFunc::NewLBL(this, phase[i] + "Гм. " + QString::number(h)), s, i, 1, 1);
            glyout->addWidget(
                WDFunc::NewLBLT(this, "", QString::number((value + j) + i * 61), ValuesFormat,
                    QString::number(h) + "-я гармоника напряжения в фазе" + phase[i] + "в % от 1-й", true),
                k, i, 1, 1);
            j++;
        }
    }

    quint32 val = max - min;

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { value, val } });
    return w;
}
// void CheckKDVHarmonicDialog::ChooseValuesToWrite() { }
// void CheckKDVHarmonicDialog::SetDefaultValuesToWrite() { }
// void CheckKDVHarmonicDialog::PrepareAnalogMeasurements() { }

// void CheckKDVHarmonicDialog::USBUpdate()
//{
//    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw1");
//    if (CheckTW == nullptr)
//    {
//        qDebug() << "CheckTW is null";
//        return;
//    }

//    for (int i = 0; i < 5; i++)
//    {
//        if (CheckTW->currentIndex() == IndexWd.at(i))
//        {
//            if (Commands::GetBd(5, &ChHarmKDV->Bd_block5, sizeof(CheckHarmonicKDV::Bd_5_7)) == Error::Msg::NoError)
//                ChHarmKDV->FillBd5(this);
//        }
//    }

//    for (int i = 6; i < 12; i++)
//    {
//        if (CheckTW->currentIndex() == IndexWd.at(i))
//        {
//            if (Commands::GetBd(7, &ChHarmKDV->Bd_block7, sizeof(CheckHarmonicKDV::Bd_5_7)) == Error::Msg::NoError)
//                ChHarmKDV->FillBd7(this);
//        }
//    }
//}

// void CheckKDVHarmonicDialog::updateFloatData()
//{
//    QList<DataTypes::SignalsStruct> list;
//    DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
//    if (!list.isEmpty())
//    {
//        foreach (DataTypes::SignalsStruct signal, list)
//        {
//            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
//            ChHarmKDV->FillBd(this, QString::number(fwt.sigAdr), WDFunc::StringValueWithCheck(fwt.sigVal, 3));
//        }
//    }
//}

// void CheckKDVHarmonicDialog::setConnections()
//{
//    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &CheckKDVHarmonicDialog::updateFloatData);
//}

// void CheckKDVHarmonicDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
//{

//    int i = 0;
//    for (i = 0; i < Signal.size(); ++i)
//    {
//        // sig = *(Signal+i);
//        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
//            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
//            ChHarmKDV->FillBd(
//                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal,
//                3));
//        else
//            ChHarmKDV->FillBd(
//                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
//    }
//}
