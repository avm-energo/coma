#include "checkktfharmonicdialog.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../module/modules.h"
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

CheckKTFHarmonicDialog::CheckKTFHarmonicDialog(QWidget *parent) : AbstractCheckDialog(parent)

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

void CheckKTFHarmonicDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKTFHarmonicDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}

UWidget *CheckKTFHarmonicDialog::BdIWidget(int valueA, int valueB, int valueC, int min, int max)
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
                                  h + "-я гармоника тока в фазе" + phase[i] + "в % от 1-й", true),
                k, i, 1, 1);
            j++;
        }
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 3020, 122 }, { 3146, 122 }, { 3420, 122 } });
    return w;
}

UWidget *CheckKTFHarmonicDialog::BdUWidget(unsigned int value, int min, int max)
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
            glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number((value + j) + i * 61), ValuesFormat,
                                  h + "-я гармоника напряжения в фазе" + phase[i] + "в % от 1-й", true),
                k, i, 1, 1);
            j++;
        }
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { value, 122 } });
    return w;
}

// void CheckKTFHarmonicDialog::ChooseValuesToWrite() { }
// void CheckKTFHarmonicDialog::SetDefaultValuesToWrite() { }
// void CheckKTFHarmonicDialog::PrepareAnalogMeasurements() { }

// void CheckKTFHarmonicDialog::USBUpdate()
//{
//    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw0");
//    if (CheckTW == nullptr)
//    {
//        qDebug("CheckTW is null");
//        return;
//    }

//    for (int i = 0; i < 5; i++)
//    {
//        if (CheckTW->currentIndex() == IndexWd.at(i))
//        {
//            if (Commands::GetBd(5, &ChHarmKTF->Bd_block5, sizeof(CheckHarmonicKTF::Bd5)) == Error::Msg::NoError)
//                ChHarmKTF->FillBd5(this);
//        }
//    }

//    for (int i = 6; i < 12; i++)
//    {
//        if (CheckTW->currentIndex() == IndexWd.at(i))
//        {
//            if (Commands::GetBd(7, &ChHarmKTF->Bd_block7, sizeof(CheckHarmonicKTF::Bd7)) == Error::Msg::NoError)
//                ChHarmKTF->FillBd7(this);
//        }
//    }
//}

// void CheckKTFHarmonicDialog::ETHUpdate()
//{
//    updateFloatData();
//}

// void CheckKTFHarmonicDialog::MBSUpdate()
//{
//}

// void CheckKTFHarmonicDialog::reqUpdate()
//{
//    updateFloatData();
//}

// void CheckKTFHarmonicDialog::updateFloatData()
//{
//    QList<DataTypes::SignalsStruct> list;
//    DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
//    if (!list.isEmpty())
//    {
//        foreach (DataTypes::SignalsStruct signal, list)
//        {
//            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
//            ChHarmKTF->FillBd(this, QString::number(fwt.sigAdr), WDFunc::StringValueWithCheck(fwt.sigVal, 3));
//        }
//    }
//}

// void CheckKTFHarmonicDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
//{

//    int i = 0;
//    for (i = 0; i < Signal.size(); ++i)
//    {
//        // sig = *(Signal+i);
//        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
//            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
//            ChHarmKTF->FillBd(
//                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal,
//                3));
//        else
//            ChHarmKTF->FillBd(
//                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
//    }
//}
// void CheckKTFHarmonicDialog::SetWarnColor(int position, bool value)
//{
//    Q_UNUSED(position)
//    Q_UNUSED(value)
//}

// void CheckKTFHarmonicDialog::SetAlarmColor(int position, bool value)
//{
//    Q_UNUSED(position)
//    Q_UNUSED(value)
//}
