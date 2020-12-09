#include "checkkdvvibrdialog.h"

#include "../config/config.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
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

    setAttribute(Qt::WA_DeleteOnClose);

        m_BdUIList = { { "Вибрации", Bd1W() }, { "Вибрации", Bd2W() } , { "Вибрации", Bd3W() }};
        SetupUI();

    Timer->setInterval(ANMEASINT);
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


UWidget *CheckKDVVibrDialog::Bd1W()
{
    int i;
    int row, column;
    UWidget *w = new UWidget();
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    //    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Среднеквадратичные значения вибросигналов, Вольт");
    gb->setFont(ffont);

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(60 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(63 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({  { 60, 6 }});
    return w;
}

UWidget *CheckKDVVibrDialog::Bd2W()
{
    int i;
    int row, column;
    UWidget *w = new UWidget();
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    //    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Среднеквадратичные значения виброускорений, м/с2");
    gb->setFont(ffont);

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6040 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6043 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Среднеквадратичные значения виброскоростей, мм/с");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6046 + i), ValuesFormat,
                "Среднеквадратичное значение виброскорости по каналу   " + QString::number(1 + i) + ", мм/с"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6049 + i), ValuesFormat,
                "Среднеквадратичное значение виброскорости по каналу " + QString::number(4 + i) + ", мм/с"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Среднеквадратичные значения виброперемещений, мкм");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6052 + i), ValuesFormat,
                "Среднеквадратичное значение виброперемещения по каналу " + QString::number(1 + i) + ", мкм"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6055 + i), ValuesFormat,
                "Среднеквадратичное значение виброперемещения по каналу " + QString::number(4 + i) + ", мкм"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 6040, 18 }});
    return w;
}

UWidget *CheckKDVVibrDialog::Bd3W()
{
    int i;
    int row, column;
    UWidget *w = new UWidget();
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброускорения, м/с2/час");
    gb->setFont(ffont);

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6060 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброускорения по каналу"
                                  + QString::number(1 + i) + ", м/с2/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6063 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброускорения по каналу"
                                  + QString::number(4 + i) + ", м/с2/час"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброскорости, мм/с/час");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6066 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброскорости по каналу "
                                  + QString::number(1 + i) + ", мм/с/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6069 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброскорости по каналу  "
                                  + QString::number(4 + i) + ", мм/с/час"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброперемещения, мкм/час");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6052 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброперемещения по каналу "
                                  + QString::number(1 + i) + ", мкм/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6055 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброперемещения по каналу "
                                  + QString::number(4 + i) + ", мкм/час"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 6060, 18 }});
    return w;
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
