#include "checkktf.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QLayout>

CheckKTF::CheckKTF()
{
    // setAttribute(Qt::WA_DeleteOnClose);
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(UCONFCLR) + ";}";
}

CheckKTF::~CheckKTF()
{
}

QWidget *CheckKTF::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "ф. A", "ф. B", "ф. C", "ср." };
    QString pphase[4] = { "ф. A", "ф. B", "ф. C", "сум." };
    QString ppphase[4] = { "AB", "BC", "CA", "ср." };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Общие");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    glyout->addWidget(WDFunc::NewLBL(parent, "Температура ННТ, °С"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(4500), ValuesFormat, "Температура ННТ обмотки (расчетная), °С"), 1,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Tamb, °С"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(4501), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Частота, Гц"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2400), ValuesFormat, "Частота, Гц"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Температура микроконтроллера, °С"), 0, 3, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(101), ValuesFormat, "Температура микроконтроллера, °С"), 1, 3, 1,
        1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ...................................

    gb = new QGroupBox("Электрические параметры");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 4; ++i)
    {
        // QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff " + phase[i] + ", кВ"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1000 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, кВ"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ieff " + phase[i] + ", А"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1400 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, А"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "P " + pphase[i] + ", кВт"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2420 + i), ValuesFormat,
                              "Истинная активная мощность по фазам и суммарная, кВт"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Q " + pphase[i] + ", кВАр"), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2424 + i), ValuesFormat,
                              "Реактивная мощность по кажущейся полной "
                              "и истинной активной, кВАр"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "S " + pphase[i] + ", кВА"), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2428 + i), ValuesFormat,
                              "Кажущаяся полная мощность по эфф. токам и нпарямжениям, кВА"),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "CosPhi " + phase[i] + ""), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2432 + i), ValuesFormat,
                              "Косинус phi по истинной активной мощности,по фазам и средний "),
            13, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Uлин " + ppphase[i] + ", кВ"), 14, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1220 + i), ValuesFormat,
                              "Истинные действующие значения линейных "
                              "напряжений трех фази их среднее, кВ"),
            15, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKTF::Bd2W(QWidget *parent)
{

    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Оставшееся время работы до предупреждения");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    glyout->addWidget(WDFunc::NewLBL(parent, "При данной нагрузке, сек"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4010), ValuesFormat,
                          "Оставшееся время работы до "
                          "предупрежде-ния при данной нагрузке, сек"),
        1, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    //...................................

    gb = new QGroupBox("Допустимое оставшееся время работы");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "При данной нагрузке, сек"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4011), ValuesFormat,
                          "Допустимое оставшееся время работы при данной нагрузке, сек"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 105%, сек"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4012), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 105%, сек"),
        1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 110%, сек"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4013), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 110%, сек"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 115%, сек"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4014), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 115%, сек"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 120%, сек"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4015), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 120%, сек"),
        3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 125%, сек"), 2, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4016), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 125%, сек"),
        3, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 130%, сек"), 4, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4017), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 130%, сек"),
        5, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 135%, сек"), 4, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4018), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 135%, сек"),
        5, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 140%, сек"), 4, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4019), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 140%, сек"),
        5, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 145%, сек"), 6, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4020), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 145%, сек"),
        7, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 150%, сек"), 6, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4021), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 150%, сек"),
        7, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 155%, сек"), 6, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4022), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 155%, сек"),
        7, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 160%, сек"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4023), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 160%, сек"),
        9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 165%, сек"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4024), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 165%, сек"),
        9, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 170%, сек"), 8, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4025), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 170%, сек"),
        9, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 175%, сек"), 10, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4026), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 175%, сек"),
        11, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 180%, сек"), 10, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4027), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 180%, сек"),
        11, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 185%, сек"), 10, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4028), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 185%, сек"),
        11, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 190%, сек"), 12, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4029), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 190%, сек"),
        13, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 195%, сек"), 12, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4030), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 195%, сек"),
        13, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 200%, сек"), 12, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4031), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 200%, сек"),
        13, 2, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKTF::Bd3W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Изоляция");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    glyout->addWidget(WDFunc::NewLBL(parent, "Относительная скорость старения изоляции"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(4000), ValuesFormat, "Относительная скорость старения изоляции"), 1,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Интегральное старение изоляции, час"), 2, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(4001), ValuesFormat, "Интегральное старение изоляции, час"), 3, 0,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Остаточный ресурс изоляции, час"), 4, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(4002), ValuesFormat, "Остаточный ресурс изоляции, час"), 5, 0, 1,
        1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Амплитуда пускового тока при последнем пуске");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "Амплитуда, А"), 6, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(5200), ValuesFormat,
                          "Амплитуда пускового тока при последнем пуске, А"),
        7, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKTF::Bd4W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "ф. A", "ф. B", "ф. C", "ср." };
    QString pphase[4] = { "ф. A", "ф. B", "ф. C", "сум." };
    QString ppphase[4] = { "AB", "BC", "CA", "ср." };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Электрические параметры по 1-й гармонике");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 4; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff " + phase[i] + ", кВ"), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1004 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз "
                              "по 1-й гармонике и их среднее, кВ"),
            1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ieff " + phase[i] + ", А"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1404 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз "
                              "по 1-й гармонике и их среднее, А"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "P " + pphase[i] + ", кВт"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2450 + i), ValuesFormat,
                              "Активная мощность по 1-й гармонике, по фазам и суммарная, кВт"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Q " + pphase[i] + ", кВАр"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2454 + i), ValuesFormat,
                              "Реактивня мощность по 1-й гармонике, кВАр"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "S " + pphase[i] + ", кВА"), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2458 + i), ValuesFormat,
                              "Полная мощность по 1-й гармонике, кВА"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "CosPhi " + phase[i] + ""), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2462 + i), ValuesFormat,
                              "Косинус phi по 1-й гармонике,по фазам и средний "),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Uлин " + ppphase[i] + ", кВ"), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1224 + i), ValuesFormat,
                              "Действующие значения 1-й гармоники линейных "
                              "напряжений трех фази их среднее, кВ"),
            13, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKTF::Bd5W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф. A", "ф. B", "ф. C" };
    // QString pphase[4] = {"ф. A","ф. B","ф. C","сум."};
    // QString ppphase[4] = {"AB","BC","CA","ср."};

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Данные по напряжению");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "U0, кВ"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1020), ValuesFormat,
                          "Напряжение нулевой последовательности гр.1, кВ"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1, кВ"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1021), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2, кВ"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1022), ValuesFormat, "Частота, Гц"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "KunsimU0"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1023), ValuesFormat,
                          "Коэффициент несимметрии напряжения по "
                          "обратной последовательности гр.1, %"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "KunsimU2"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1024), ValuesFormat,
                          "Коэффициент несимметрии напряжения по "
                          "нулевой последовательности гр.1, %"),
        3, 1, 1, 1);
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(parent, "THD" + phase[i] + ""), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1025 + i), ValuesFormat,
                              "Общий коэффициент гарм. искажений напряжения фазы " + phase[i] + " гр.1"),
            5, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Данные по току");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "I0, кВ"), 6, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1420), ValuesFormat, "Ток нулевой последовательности гр.1, A"), 7,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I1, кВ"), 6, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1421), ValuesFormat, "Ток прямой последовательности гр.1, А"), 7, 1,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I2, А"), 6, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", QString::number(1422), ValuesFormat, "Ток обратной последовательности гр.1, А"), 7,
        2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "KunsimI0"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1423), ValuesFormat,
                          "Коэффициент несимметрии тока по обратной последовательности гр.1,%"),
        9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "KunsimI2"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1424), ValuesFormat,
                          "Коэффициент несимметрии тока по нулевой последовательности гр.1,%"),
        9, 1, 1, 1);
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(parent, "THD" + phase[i] + ""), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1425 + i), ValuesFormat,
                              "Общий коэффициент гарм. искажений тока фазы " + phase[i] + " гр.1"),
            11, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void CheckKTF::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetLBLText(parent, Name, Value);
}

QWidget *CheckKTF::BdUI(int bdnum)
{
    Q_UNUSED(bdnum);
    return nullptr;
}

void CheckKTF::SetDefaultValuesToWrite()
{
}

void CheckKTF::PrepareAnalogMeasurements()
{
}

void CheckKTF::ChooseValuesToWrite()
{
}

void CheckKTF::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}

void CheckKTF::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row);
}

void CheckKTF::RefreshAnalogValues(int bdnum)
{
    Q_UNUSED(bdnum);
}

QWidget *CheckKTF::CustomTab()
{
    QWidget *w = new QWidget;

    return w;
}

void CheckKTF::FillBd13(QWidget *parent)
{

    WDFunc::SetLBLText(parent, QString::number(4500), WDFunc::StringValueWithCheck(Bd_block13.TempWinding, 3));
    WDFunc::SetLBLText(parent, QString::number(4501), WDFunc::StringValueWithCheck(Bd_block13.temperature, 3));
}

void CheckKTF::FillBd0(QWidget *parent)
{
    WDFunc::SetLBLText(parent, QString::number(101), WDFunc::StringValueWithCheck(Bd_block0.Temp, 3));
}

void CheckKTF::FillBd17(QWidget *parent)
{
    WDFunc::SetLBLText(parent, QString::number(2400), WDFunc::StringValueWithCheck(Bd_block17.Frequency, 3));
}

void CheckKTF::FillBd10(QWidget *parent)
{
    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(2420 + i), WDFunc::StringValueWithCheck(Bd_block10.PNatf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2424 + i), WDFunc::StringValueWithCheck(Bd_block10.QNatf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2428 + i), WDFunc::StringValueWithCheck(Bd_block10.SNatf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2432 + i), WDFunc::StringValueWithCheck(Bd_block10.CosPhiNat[i], 3));

        WDFunc::SetLBLText(parent, QString::number(2450 + i), WDFunc::StringValueWithCheck(Bd_block10.Pf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2454 + i), WDFunc::StringValueWithCheck(Bd_block10.Qf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2458 + i), WDFunc::StringValueWithCheck(Bd_block10.Sf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2462 + i), WDFunc::StringValueWithCheck(Bd_block10.CosPhi, 3));
    }
}

void CheckKTF::FillBd2(QWidget *parent)
{

    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(
            parent, QString::number(1000 + i), WDFunc::StringValueWithCheck(Bd_block2.IUefNat_filt[i], 3));

        WDFunc::SetLBLText(
            parent, QString::number(1004 + i), WDFunc::StringValueWithCheck(Bd_block2.IUeff_filtered[i], 3));
    }
}

void CheckKTF::FillBd3(QWidget *parent)
{
    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(
            parent, QString::number(1400 + i), WDFunc::StringValueWithCheck(Bd_block3.IUefNat_filt[i], 3));

        WDFunc::SetLBLText(
            parent, QString::number(1404 + i), WDFunc::StringValueWithCheck(Bd_block3.IUeff_filtered[i], 3));
    }
}

void CheckKTF::FillBd4(QWidget *parent)
{

    WDFunc::SetLBLText(parent, QString::number(1020), WDFunc::StringValueWithCheck(Bd_block4.U0, 3));
    WDFunc::SetLBLText(parent, QString::number(1021), WDFunc::StringValueWithCheck(Bd_block4.U1, 3));
    WDFunc::SetLBLText(parent, QString::number(1022), WDFunc::StringValueWithCheck(Bd_block4.U2, 3));
    WDFunc::SetLBLText(parent, QString::number(1023), WDFunc::StringValueWithCheck(Bd_block4.Kunsim0, 3));
    WDFunc::SetLBLText(parent, QString::number(1024), WDFunc::StringValueWithCheck(Bd_block4.Kunsim2, 3));

    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(1025 + i), WDFunc::StringValueWithCheck(Bd_block4.THD[i], 3));
    }
}

void CheckKTF::FillBd6(QWidget *parent)
{
    WDFunc::SetLBLText(parent, QString::number(1420), WDFunc::StringValueWithCheck(Bd_block6.U0, 3));
    WDFunc::SetLBLText(parent, QString::number(1421), WDFunc::StringValueWithCheck(Bd_block6.U1, 3));
    WDFunc::SetLBLText(parent, QString::number(1422), WDFunc::StringValueWithCheck(Bd_block6.U2, 3));
    WDFunc::SetLBLText(parent, QString::number(1423), WDFunc::StringValueWithCheck(Bd_block6.Kunsim0, 3));
    WDFunc::SetLBLText(parent, QString::number(1424), WDFunc::StringValueWithCheck(Bd_block6.Kunsim2, 3));

    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(1425 + i), WDFunc::StringValueWithCheck(Bd_block6.THD[i], 3));
    }
}

//..................

void CheckKTF::FillBd8(QWidget *parent)
{
    for (int j = 0; j < 4; j++)
    {
        WDFunc::SetLBLText(
            parent, QString::number(1220 + j), WDFunc::StringValueWithCheck(Bd_block8.UefNatLin_filt[j], 3));

        WDFunc::SetLBLText(
            parent, QString::number(1224 + j), WDFunc::StringValueWithCheck(Bd_block8.UeffLin_filtered[j], 3));
    }
}

void CheckKTF::FillBd11(QWidget *parent)
{

    WDFunc::SetLBLText(parent, QString::number(4010), WDFunc::StringValueWithCheck(Bd_block11.Tpred, 3));
    WDFunc::SetLBLText(parent, QString::number(4011), WDFunc::StringValueWithCheck(Bd_block11.Tdop, 3));
    WDFunc::SetLBLText(parent, QString::number(4012), WDFunc::StringValueWithCheck(Bd_block11.Tdop105, 3));
    WDFunc::SetLBLText(parent, QString::number(4013), WDFunc::StringValueWithCheck(Bd_block11.Tdop110, 3));
    WDFunc::SetLBLText(parent, QString::number(4014), WDFunc::StringValueWithCheck(Bd_block11.Tdop115, 3));
    WDFunc::SetLBLText(parent, QString::number(4015), WDFunc::StringValueWithCheck(Bd_block11.Tdop120, 3));
    WDFunc::SetLBLText(parent, QString::number(4016), WDFunc::StringValueWithCheck(Bd_block11.Tdop125, 3));
    WDFunc::SetLBLText(parent, QString::number(4017), WDFunc::StringValueWithCheck(Bd_block11.Tdop130, 3));
    WDFunc::SetLBLText(parent, QString::number(4018), WDFunc::StringValueWithCheck(Bd_block11.Tdop135, 3));
    WDFunc::SetLBLText(parent, QString::number(4019), WDFunc::StringValueWithCheck(Bd_block11.Tdop140, 3));
    WDFunc::SetLBLText(parent, QString::number(4020), WDFunc::StringValueWithCheck(Bd_block11.Tdop145, 3));
    WDFunc::SetLBLText(parent, QString::number(4021), WDFunc::StringValueWithCheck(Bd_block11.Tdop150, 3));
    WDFunc::SetLBLText(parent, QString::number(4022), WDFunc::StringValueWithCheck(Bd_block11.Tdop155, 3));
    WDFunc::SetLBLText(parent, QString::number(4023), WDFunc::StringValueWithCheck(Bd_block11.Tdop160, 3));
    WDFunc::SetLBLText(parent, QString::number(4024), WDFunc::StringValueWithCheck(Bd_block11.Tdop165, 3));
    WDFunc::SetLBLText(parent, QString::number(4025), WDFunc::StringValueWithCheck(Bd_block11.Tdop170, 3));
    WDFunc::SetLBLText(parent, QString::number(4026), WDFunc::StringValueWithCheck(Bd_block11.Tdop175, 3));
    WDFunc::SetLBLText(parent, QString::number(4027), WDFunc::StringValueWithCheck(Bd_block11.Tdop180, 3));
    WDFunc::SetLBLText(parent, QString::number(4028), WDFunc::StringValueWithCheck(Bd_block11.Tdop185, 3));
    WDFunc::SetLBLText(parent, QString::number(4029), WDFunc::StringValueWithCheck(Bd_block11.Tdop190, 3));
    WDFunc::SetLBLText(parent, QString::number(4030), WDFunc::StringValueWithCheck(Bd_block11.Tdop195, 3));
    WDFunc::SetLBLText(parent, QString::number(4031), WDFunc::StringValueWithCheck(Bd_block11.Tdop200, 3));
}

void CheckKTF::FillBd9(QWidget *parent)
{
    WDFunc::SetLBLText(parent, QString::number(4000), WDFunc::StringValueWithCheck(Bd_block9.Vst, 3));
    WDFunc::SetLBLText(parent, QString::number(4001), WDFunc::StringValueWithCheck(Bd_block9.Age, 3));
    WDFunc::SetLBLText(parent, QString::number(4002), WDFunc::StringValueWithCheck(Bd_block9.Resurs, 3));
}

void CheckKTF::FillBd18(QWidget *parent)
{
    WDFunc::SetLBLText(parent, QString::number(5200), WDFunc::StringValueWithCheck(Bd_block18.Istart, 3));
}
