#include "checkkiv.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QLayout>

CheckKIV::CheckKIV()
{
    // setAttribute(Qt::WA_DeleteOnClose);
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(Colors::ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}";
}

CheckKIV::~CheckKIV()
{
}

QWidget *CheckKIV::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    // QHBoxLayout *hlyout = new QHBoxLayout;
    QString phase[3] = { "A", "B", "C" };
    // hlyout->addWidget(WDFunc::NewLBL(parent, "Номер:"), 0);
    // hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value0", ValuesFormat,
    // "Номер"), 0);
    QFont font;
    QGroupBox *gb = new QGroupBox("Общие");
    font.setFamily("Times");
    font.setPointSize(11);
    // setFont(font);
    gb->setFont(font);
    glyout->addWidget(WDFunc::NewLBL(parent, "Температура микроконтроллера, °С"), 0, 0, 1, 1);

    QLabel *lbl = new QLabel;
    lbl->setText("Температура микроконтроллера, °С");
    glyout->addWidget(lbl, 0, 0, 1, 1);

    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(101), ValuesFormat, "Температура микроконтроллера, °С"), 1, 0, 1,
        1);
    // hlyout->addWidget(WDFunc::NewLBL(parent, "                     "));
    glyout->addWidget(WDFunc::NewLBL(parent, "Tamb, °С"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(4501), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Частота, Гц"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "2400", ValuesFormat, "Частота, Гц"), 1, 2, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Параметры вводов");
    gb->setFont(font);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 3; ++i)
    {
        // QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff ф." + phase[i] + ", кВ"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1000 + i), ValuesFormat,
                              "Действующие значения напряжений по 1-й гармонике, кВ"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ieff ф." + phase[i] + ", мА"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1100 + i), ValuesFormat,
                              "Действующие значения токов по 1-й гармонике, мА"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Cbush ф." + phase[i] + ", пФ"), 6, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2420 + i), ValuesFormat, "Ёмкости вводов, пФ"), 7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Tg_d ф." + phase[i] + ", %"), 8, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2423 + i), ValuesFormat, "Тангенсы дельта вводов, %"), 9, i, 1,
            1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dCbush ф." + phase[i] + ", %"), 10, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2426 + i), ValuesFormat, "Изменение емкостей вводов, пФ"), 11,
            i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dTg_d ф." + phase[i] + ", %"), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2429 + i), ValuesFormat,
                              "Изменение тангенсов дельта вводов, %"),
            13, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Параметры небаланса токов");
    gb->setFont(font);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "Iunb, мА"), 14, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(2432), ValuesFormat, "Действующее значение тока небаланса, %"), 15,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Phy_unb, град."), 14, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2433), ValuesFormat,
                          "Угол тока небаланса относительно тока ф.А, град."),
        15, 1, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKIV::Bd2W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "A", "B", "C" };
    QFont font;

    QGroupBox *gb = new QGroupBox("Симметричные составляющие");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "U0, кВ"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1020), ValuesFormat, "Напряжение нулевой последовательности, кВ"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1, кВ"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1021), ValuesFormat, "Напряжение прямой последовательности, кВ"), 1,
        1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2, кВ"), 0, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1022), ValuesFormat, "Напряжение обратной последовательности, кВ"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I0, мА"), 2, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1120), ValuesFormat, "Ток нулевой последовательности, мА"), 3, 0, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I1, мА"), 2, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1121), ValuesFormat, "Ток прямой последовательности, мА"), 3, 1, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I2, мА"), 2, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1122), ValuesFormat, "Ток обратной последовательности, мА"), 3, 2,
        1, 1);
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
        glyout->addWidget(WDFunc::NewLBL(parent, "Phy_U ф." + phase[i] + ", град."), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2401 + i), ValuesFormat,
                              "Угол по напряжению ф." + phase[i] + ", град."),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Phy_I ф." + phase[i] + ", град."), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2404 + i), ValuesFormat,
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

    glyout->addWidget(WDFunc::NewLBL(parent, "Ramb, Ом"), 8, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(4502), ValuesFormat, "Сопротивление датчика, Ом"), 9, 0, 1, 1);
    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKIV::Bd3W(QWidget *parent)
{

    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "A", "B", "C" };
    QFont font;
    QString title;

    QGroupBox *gb = new QGroupBox("Отладка");
    font.setFamily("Times");
    font.setPointSize(11);
    // setFont(font);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "13000"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13000), ValuesFormat, ""), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13001"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13001), ValuesFormat, ""), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13002"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13002), ValuesFormat, ""), 1, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "13003"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13003), ValuesFormat, ""), 3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13004"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13004), ValuesFormat, ""), 3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13005"), 2, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13005), ValuesFormat, ""), 3, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "13006"), 4, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13006), ValuesFormat, ""), 5, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13007"), 4, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13007), ValuesFormat, ""), 5, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13008"), 4, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13008), ValuesFormat, ""), 5, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "13009"), 6, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13009), ValuesFormat, ""), 7, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13010"), 6, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13010), ValuesFormat, ""), 7, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13011"), 6, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13011), ValuesFormat, ""), 7, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "13012"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13012), ValuesFormat, ""), 9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13013"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13013), ValuesFormat, ""), 9, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "13014"), 8, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13014), ValuesFormat, ""), 9, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "13015"), 10, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(13015), ValuesFormat, ""), 11, 0, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void CheckKIV::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetLBLText(parent, Name, Value);
}

QWidget *CheckKIV::BdUI(int bdnum)
{
    Q_UNUSED(bdnum);
    return nullptr;
}

// void Check_KIV::SetDefaultValuesToWrite()
//{
//}

// void Check_KIV::PrepareAnalogMeasurements()
//{
//}

// void Check_KIV::ChooseValuesToWrite()
//{
//}

// void Check_KIV::WriteToFile(int row, int bdnum)
//{
//    Q_UNUSED(row);
//    Q_UNUSED(bdnum);
//}

// void Check_KIV::PrepareHeadersForFile(int row)
//{
//    Q_UNUSED(row);
//}

// void Check_KIV::RefreshAnalogValues(int bdnum)
//{
//    Q_UNUSED(bdnum);
//}

QWidget *CheckKIV::CustomTab()
{
    QWidget *w = new QWidget;

    return w;
}

void CheckKIV::FillBdUSB(QWidget *parent)
{

    WDFunc::SetLBLText(parent, QString::number(101), WDFunc::StringValueWithCheck(Bd_block1.Tmk, 3));
    WDFunc::SetLBLText(parent, QString::number(102), WDFunc::StringValueWithCheck(Bd_block1.Frequency, 3));
    for (int i = 0; i < 3; i++)
    {

        WDFunc::SetLBLText(parent, QString::number(1000 + i), WDFunc::StringValueWithCheck(Bd_block1.Ueff[i], 3));
        WDFunc::SetLBLText(parent, QString::number(1100 + i), WDFunc::StringValueWithCheck(Bd_block1.Ieff[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2420 + i), WDFunc::StringValueWithCheck(Bd_block1.Cbush[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2423 + i), WDFunc::StringValueWithCheck(Bd_block1.Tg_d[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2426 + i), WDFunc::StringValueWithCheck(Bd_block1.dCbush[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2429 + i), WDFunc::StringValueWithCheck(Bd_block1.dTg_d[i], 3));
    }

    WDFunc::SetLBLText(parent, "1020", WDFunc::StringValueWithCheck(Bd_block1.U0, 3));
    WDFunc::SetLBLText(parent, "1021", WDFunc::StringValueWithCheck(Bd_block1.U1, 3));
    WDFunc::SetLBLText(parent, "1022", WDFunc::StringValueWithCheck(Bd_block1.U2, 3));

    WDFunc::SetLBLText(parent, "1120", WDFunc::StringValueWithCheck(Bd_block1.I0, 3));
    WDFunc::SetLBLText(parent, "1121", WDFunc::StringValueWithCheck(Bd_block1.I1, 3));
    WDFunc::SetLBLText(parent, "1122", WDFunc::StringValueWithCheck(Bd_block1.I2, 3));
    WDFunc::SetLBLText(parent, "2432", WDFunc::StringValueWithCheck(Bd_block1.Iunb, 3));
    WDFunc::SetLBLText(parent, "2433", WDFunc::StringValueWithCheck(Bd_block1.Phy_unb, 3));

    WDFunc::SetLBLText(parent, "4501", WDFunc::StringValueWithCheck(Bd_block1.Tamb, 3));
}

void CheckKIV::FillBd5(QWidget *parent)
{
    WDFunc::SetLBLText(parent, QString::number(2400), WDFunc::StringValueWithCheck(Bd_block5.Frequency, 3));

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(2401 + i), WDFunc::StringValueWithCheck(Bd_block5.phi_next_f[i], 3));
    }
}

void CheckKIV::FillBd8(QWidget *parent)
{

    WDFunc::SetLBLText(parent, "4502", WDFunc::StringValueWithCheck(Bd_block8.resistance, 3));
}
