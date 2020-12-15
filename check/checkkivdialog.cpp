#include "checkkivdialog.h"

#include "../config/config.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
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

CheckKIVDialog::CheckKIVDialog(QWidget *parent) : AbstractCheckDialog(parent)
{
    //    EParent = parent;
    // QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    // setStyleSheet(tmps);
    QStringList sl;
    //    BdNum = 6;
    //    ChKIV = new CheckKIV;
    //    Ch = new Check;
    //    m_stColor.resize(7);
    //    BdNum = 11;
    setAttribute(Qt::WA_DeleteOnClose);

    m_BdUIList = { { "Основные", Bd1W() }, { "Дополнительные", Bd2W() } };
    m_BdUIList.first().widget->setUpdatesEnabled();

    Timer->setInterval(ANMEASINT);
}

CheckKIVDialog::~CheckKIVDialog()
{
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

UWidget *CheckKIVDialog::Bd1W()
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    // QHBoxLayout *hlyout = new QHBoxLayout;
    QString phase { "ABC" };
    // hlyout->addWidget(WDFunc::NewLBL(this, "Номер:"), 0);
    // hlyout->addWidget(WDFunc::NewLBLT(this, "", "value0", ValuesFormat,
    // "Номер"), 0);
    QFont font;
    QGroupBox *gb = new QGroupBox("Общие");
    font.setFamily("Times");
    font.setPointSize(11);
    // setFont(font);
    gb->setFont(font);
    glyout->addWidget(WDFunc::NewLBL(this, "Температура микроконтроллера, °С"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "101", ValuesFormat, "Температура микроконтроллера, °С"), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Tamb, °С"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "4501", ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Частота, Гц"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "2400", ValuesFormat, "Частота, Гц"), 1, 2, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Параметры вводов");
    gb->setFont(font);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Ueff ф." + phase[i] + ", кВ"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1000 + i), ValuesFormat,
                              "Действующие значения напряжений по 1-й гармонике, кВ"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Ieff ф." + phase[i] + ", мА"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1100 + i), ValuesFormat,
                              "Действующие значения токов по 1-й гармонике, мА"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Cbush ф." + phase[i] + ", пФ"), 6, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2420 + i), ValuesFormat, "Ёмкости вводов, пФ"), 7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Tg_d ф." + phase[i] + ", %"), 8, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2423 + i), ValuesFormat, "Тангенсы дельта вводов, %"), 9, i, 1,
            1);
        glyout->addWidget(WDFunc::NewLBL(this, "dCbush ф." + QString(phase.at(i)) + ", %"), 10, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2426 + i), ValuesFormat, "Изменение емкостей вводов, пФ"), 11, i,
            1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "dTg_d ф." + phase[i] + ", %"), 12, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2429 + i), ValuesFormat, "Изменение тангенсов дельта вводов, %"),
            13, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Параметры небаланса токов");
    gb->setFont(font);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Iunb, мА"), 14, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "2432", ValuesFormat, "Действующее значение тока небаланса, %"), 15, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy_unb, град."), 14, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "2433", ValuesFormat, "Угол тока небаланса относительно тока ф.А, град."), 15, 1, 1,
        1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    //    w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
    w->setFloatBdQuery({ { 101, 2 }, { 1000, 16 }, { 1100, 16 }, { 2400, 7 }, { 2420, 14 }, { 4501, 2 } });
    w->setSpBdQuery({ { 3011, 25 } });

    QMap<int, QList<UWidget::HighlightWarnAlarmStruct>> hmap
        = { { 3011, { { 1000, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3012, { { 1001, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3013, { { 1002, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3014, { { 1100, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3015, { { 1101, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3016, { { 1102, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3018, { { 1000, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3019, { { 1001, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3020, { { 1002, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3021, { { 2429, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3022, { { 2430, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3023, { { 2431, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3024, { { 2429, Colors::ColorsMap.value(Colors::Red) } } },
              { 3025, { { 2430, Colors::ColorsMap.value(Colors::Red) } } },
              { 3026, { { 2431, Colors::ColorsMap.value(Colors::Red) } } },
              { 3027, { { 2426, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3028, { { 2427, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3029, { { 2428, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3030, { { 2426, Colors::ColorsMap.value(Colors::Red) } } },
              { 3031, { { 2427, Colors::ColorsMap.value(Colors::Red) } } },
              { 3032, { { 2428, Colors::ColorsMap.value(Colors::Red) } } },
              { 3034, { { 2432, Colors::ColorsMap.value(Colors::AConfO) } } },
              { 3035, { { 2432, Colors::ColorsMap.value(Colors::Red) } } } };

    w->setHighlightMap(hmap);
    return w;
}

UWidget *CheckKIVDialog::Bd2W()
{
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase { "ABC" };
    QFont font;

    QGroupBox *gb = new QGroupBox("Симметричные составляющие");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "U0, кВ"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1020", ValuesFormat, "Напряжение нулевой последовательности, кВ"), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U1, кВ"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1021", ValuesFormat, "Напряжение прямой последовательности, кВ"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2, кВ"), 0, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1022", ValuesFormat, "Напряжение обратной последовательности, кВ"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "I0, мА"), 2, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1120", ValuesFormat, "Ток нулевой последовательности, мА"), 3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "I1, мА"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "1121", ValuesFormat, "Ток прямой последовательности, мА"), 3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "I2, мА"), 2, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1122", ValuesFormat, "Ток обратной последовательности, мА"), 3, 2, 1, 1);
    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Угловые значения");
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    gb->setFont(font);

    int i;
    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Phy_U ф." + phase[i] + ", град."), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2401 + i), ValuesFormat,
                              "Угол по напряжению ф." + phase[i] + ", град."),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Phy_I ф." + phase[i] + ", град."), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2404 + i), ValuesFormat,
                              "Угол по току ф." + phase[i] + ", град."),
            7, i, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Температура");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Ramb, Ом"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "4502", ValuesFormat, "Сопротивление датчика, Ом"), 9, 0, 1, 1);
    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    //   w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
    w->setFloatBdQuery({ { 1000, 16 }, { 1100, 16 }, { 4501, 2 } });
    return w;
}

// void CheckKIVDialog::setConnections()
//{
//    //    if ((m_updatesEnabled) && m_timerCounter) // every second tick of the timer
//    //    {
//    //        getFloatData();
//    //        getSPData();
//    //        updateFloatData();
//    //        updateSPData();
//    //        m_timerCounter = !m_timerCounter;
//    //    }
//    //    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &CheckKIVDialog::updateFloatData);
//    //    connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, this,
//    &CheckKIVDialog::updateSPData);
//}

// void CheckKIVDialog::reqUpdate()
//{
//    foreach (BdQuery query, FloatBdQueryList)
//        iface()->reqFloats(query.sigAdr, query.sigQuantity);
//    foreach (BdQuery query, SpBdQueryList)
//        iface()->reqAlarms(query.sigAdr, query.sigQuantity);
//}

// void CheckKIVDialog::updateFloatData(DataTypes::FloatStruct &fl)
//{
//    if ((m_updatesEnabled) && m_timerCounter) // every second tick of the timer
//                                              //    QList<DataTypes::SignalsStruct> list;
//        //    DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
//        //    if (!list.isEmpty())
//        //    {
//        //        foreach (DataTypes::SignalsStruct signal, list)
//        //        {
//        //            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
//        WDFunc::SetLBLText(this, QString::number(fl.sigAdr), WDFunc::StringValueWithCheck(fl.sigVal, 3));
//    //        }
//    //    }
//}

// void CheckKIVDialog::updateSPData(DataTypes::SinglePointWithTimeStruct &sp)
//{
//    //    QList<DataTypes::SignalsStruct> list;
//    //    DataManager::getSignals(3011, 3035, DataTypes::SignalTypes::SinglePointWithTime, list);
//    //    if (!list.isEmpty())
//    //    {
//    //        foreach (DataTypes::SignalsStruct signal, list)
//    //        {
//    //            DataTypes::SinglePointWithTimeStruct sp =
//    //            qvariant_cast<DataTypes::SinglePointWithTimeStruct>(signal.data);
//    QList<HighlightWarnAlarmStruct> hstlist = HighlightMap()[sp.sigAdr];
//    foreach (HighlightWarnAlarmStruct hst, hstlist)
//        WDFunc::SetLBLTColor(this, QString::number(hst.fieldnum), (sp.sigVal == 1) ? Colors::TABCOLORA1 : hst.color);
//    //    if ((sp.sigAdr >= 3011) && (sp.sigAdr <= 3013))
//    //        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 2011),
//    //            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
//    //                                                                        //        }
//    //    if ((sp.sigAdr >= 3014) && (sp.sigAdr <= 3016))
//    //        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 1914),
//    //            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
//    //                                                                        //            }
//    //    if ((sp.sigAdr >= 3018) && (sp.sigAdr <= 3020))
//    //        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 2018),
//    //            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
//    //    if ((sp.sigAdr >= 3021) && (sp.sigAdr <= 3023))
//    //    {
//    //        WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 2021),
//    //            (sp.sigVal == 1) ? Colors::TABCOLORA1 : Colors::ACONFOCLR); // Colors::TABCOLORA1
//    //        if (sp.sigVal == 0)
//    //            m_stColor[sp.sigAdr - 3021] = 1;
//    //    }
//    //    if ((sp.sigAdr >= 3024) && (sp.sigAdr <= 3026))
//    //    {
//    //        if (sp.sigVal == 1)
//    //        {
//    //            m_stColor[sp.sigAdr - 3024] = 0;
//    //            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 595), Colors::REDCOLOR);
//    //        }
//    //        else
//    //        {
//    //            if (!m_stColor[sp.sigAdr - 3024])
//    //                WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 595), Colors::TABCOLORA1);
//    //        }
//    //    }
//    //    if ((sp.sigAdr >= 3027) && (sp.sigAdr <= 3029))
//    //    {
//    //        if (sp.sigVal == 1)
//    //            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 601), Colors::TABCOLORA1);
//    //        else
//    //        {
//    //            m_stColor[sp.sigAdr - 3024] = 1;
//    //            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 601), Colors::ACONFOCLR);
//    //        }
//    //    }
//    //    if ((sp.sigAdr >= 3030) && (sp.sigAdr < 3033))
//    //    {
//    //        if (sp.sigVal == 1)
//    //        {
//    //            m_stColor[sp.sigAdr - 3027] = 0;
//    //            WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 604), Colors::REDCOLOR);
//    //        }
//    //        else
//    //        {
//    //            if (!m_stColor[sp.sigAdr - 3027])
//    //                WDFunc::SetLBLTColor(this, QString::number(sp.sigAdr - 604), Colors::TABCOLORA1);
//    //        }
//    //    }
//    //    if (sp.sigAdr == 3034)
//    //    {
//    //        if (sp.sigVal == 1)
//    //            WDFunc::SetLBLTColor(this, "2432", Colors::TABCOLORA1);
//    //        else
//    //        {
//    //            m_stColor[6] = 1;
//    //            WDFunc::SetLBLTColor(this, "2432", Colors::ACONFOCLR);
//    //        }
//    //    }
//    //    if (sp.sigAdr == 3035)
//    //    {
//    //        if (sp.sigVal == 1)
//    //        {
//    //            m_stColor[6] = 0;
//    //            WDFunc::SetLBLTColor(this, "2432", Colors::REDCOLOR);
//    //        }
//    //        else
//    //        {
//    //            if (!m_stColor[6])
//    //                WDFunc::SetLBLTColor(this, "2432", Colors::TABCOLORA1);
//    //        }
//    //    }
//    //    //        }
//    //    //    }
//}

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

// void CheckKIVDialog::SetWarnColor(int position, bool value)
//{
//    if ((position >= 0) && (position < 3))
//    {
//        WDFunc::SetLBLTColor(this, QString::number(1000 + position), (value) ? Colors::TABCOLORA1 :
//        Colors::ACONFOCLR);
//    }

//    if ((position >= 3) && (position < 6))
//    {
//        WDFunc::SetLBLTColor(this, QString::number(1100 + 3), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
//    }

//    if ((position >= 7) && (position < 10))
//    {
//        if (value)
//            WDFunc::SetLBLTColor(this, QString::number(1000 + position - 7), Colors::TABCOLORA1);
//    }

//    if ((position >= 10) && (position < 13))
//    {
//        WDFunc::SetLBLTColor(
//            this, QString::number(2429 + position - 10), (value) ? Colors::ACONFYCLR : Colors::ACONFOCLR);
//    }
//    else if ((position >= 13) && (position < 16))
//    {
//        WDFunc::SetLBLTColor(
//            this, QString::number(2426 + position - 13), (value) ? Colors::ACONFYCLR : Colors::ACONFOCLR);
//    }
//    else if (position == 17)
//    {
//        WDFunc::SetLBLTColor(
//            this, QString::number(2432 + position - 13), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
//    }
//}

// void CheckKIVDialog::getFloatData()
//{
//}

// void CheckKIVDialog::getSPData()
//{
//}

// void CheckKIVDialog::SetAlarmColor(int position, bool value)
//{
//    if (position < 3)
//    {
//        if (value == true)
//            WDFunc::SetLBLTColor(this, QString::number(2429 + position), Colors::REDCOLOR);
//    }
//    else if ((position >= 3) && (position < 6))
//    {
//        if (value == true)
//            WDFunc::SetLBLTColor(this, QString::number(2426 + position - 3), Colors::REDCOLOR);
//    }
//    else if (position == 6)
//    {
//        if (value == true)
//            WDFunc::SetLBLTColor(this, "2432", Colors::REDCOLOR);
//    }
//}
