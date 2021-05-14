#include "checkkivdialog.h"

#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CheckKIVDialog::CheckKIVDialog(QWidget *parent) : AbstractCheckDialog(parent)
{
    QStringList sl;
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

    QString phase { "ABC" };

    QGroupBox *gb = new QGroupBox("Общие");

    glyout->addWidget(WDFunc::NewLBL2(this, "Температура микроконтроллера, °С"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "101", ValuesFormat, "Температура микроконтроллера, °С"), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Tamb, °С"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "4501", ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Частота, Гц"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "2400", ValuesFormat, "Частота, Гц"), 1, 2, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Параметры вводов");
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL2(this, "U ф." + phase[i] + ", кВ"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1000 + i), ValuesFormat,
                              "Фазное действующее напряжение по 1-й гармонике, кВ"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "I ф." + phase[i] + ", мА"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1100 + i), ValuesFormat,
                              "Действующее значение тока утечки ввода по 1-й гармонике, мА"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "C1 ф." + phase[i] + ", пФ"), 6, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2420 + i), ValuesFormat, "Емкость С1 ввода, пФ"), 7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "tgδ ф." + phase[i] + ", %"), 8, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2423 + i), ValuesFormat, "Тангенс дельта ввода, %"), 9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "∆C ф." + phase[i] + ", %"), 10, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2426 + i), ValuesFormat, "Приращение емкости С1 ввода, %"), 11, i,
            1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "∆tgδ ф." + phase[i] + ", %"), 12, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2429 + i), ValuesFormat, "Приращение тангенса дельта ввода, %"),
            13, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Параметры небаланса токов");
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL2(this, "Iunb, %"), 14, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "2432", ValuesFormat, "Действующее значение небаланса токов, %"), 15, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Phi_unb, град."), 14, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "2433", ValuesFormat, "Угол небаланса токов относительно тока ф.А, град."), 15, 1, 1,
        1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 101, 2 }, { 1000, 16 }, { 1100, 16 }, { 2400, 7 }, { 2420, 14 }, { 4501, 2 } });
    w->setSpBdQuery({ { 3011, 25 } });

    //    QMap<int, QList<UWidget::HighlightWarnAlarmStruct>> hmap
    //        = { { 3011, { { 1000, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3012, { { 1001, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3013, { { 1002, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3014, { { 1100, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3015, { { 1101, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3016, { { 1102, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3018, { { 1000, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3019, { { 1001, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3020, { { 1002, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3021, { { 2429, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3022, { { 2430, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3023, { { 2431, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3024, { { 2429, Colors::ColorsMap.value(Colors::Red) } } },
    //              { 3025, { { 2430, Colors::ColorsMap.value(Colors::Red) } } },
    //              { 3026, { { 2431, Colors::ColorsMap.value(Colors::Red) } } },
    //              { 3027, { { 2426, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3028, { { 2427, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3029, { { 2428, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3030, { { 2426, Colors::ColorsMap.value(Colors::Red) } } },
    //              { 3031, { { 2427, Colors::ColorsMap.value(Colors::Red) } } },
    //              { 3032, { { 2428, Colors::ColorsMap.value(Colors::Red) } } },
    //              { 3034, { { 2432, Colors::ColorsMap.value(Colors::AConfO) } } },
    //              { 3035, { { 2432, Colors::ColorsMap.value(Colors::Red) } } } };
    //почему нет 33-го

    //    w->setHighlightMap(hmap);
    return w;
}

UWidget *CheckKIVDialog::Bd2W()
{
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase { "ABC" };

    QGroupBox *gb = new QGroupBox("Симметричные составляющие");

    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL2(this, "U0, кВ"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1011", ValuesFormat, "Напряжение нулевой последовательности, кВ"), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "U1, кВ"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1012", ValuesFormat, "Напряжение прямой последовательности, кВ"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "U2, кВ"), 0, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1013", ValuesFormat, "Напряжение обратной последовательности, кВ"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "I0, мА"), 2, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1111", ValuesFormat, "Ток нулевой последовательности, мА"), 3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "I1, мА"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "1112", ValuesFormat, "Ток прямой последовательности, мА"), 3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "I2, мА"), 2, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", "1113", ValuesFormat, "Ток обратной последовательности, мА"), 3, 2, 1, 1);
    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Угловые значения");
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    int i;
    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL2(this, "Phy_U ф." + phase[i] + ", град."), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2401 + i), ValuesFormat,
                              "Угол по напряжению ф." + phase[i] + ", град."),
            1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "Phy_I ф." + phase[i] + ", град."), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2404 + i), ValuesFormat,
                              "Угол по току ф." + phase[i] + ", град."),
            3, i, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Коэффициенты несимметрии");
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(this, "KuI0, %"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1114), ValuesFormat,
                          "Коэффициент несимметрии тока по нулевой последовательности, %"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "KuI2, %"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1115), ValuesFormat,
                          "Коэффициент несимметрии тока по обратной последовательности, %"),
        1, 1, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "KuU0, %"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1014), ValuesFormat,
                          "Коэффициент несимметрии напряжения по нулевой последовательности, %s"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "KuU2, %"), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1015), ValuesFormat,
                          "Коэффициент несимметрии напряжения по обратной последовательности, %"),
        1, 3, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Температура");
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(this, "Ramb, Ом"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "4502", ValuesFormat, "Сопротивление датчика, Ом"), 9, 0, 1, 1);
    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 1000, 16 }, { 1100, 16 }, { 2400, 7 }, { 4501, 2 } });
    return w;
}
