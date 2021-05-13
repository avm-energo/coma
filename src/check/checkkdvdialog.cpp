#include "checkkdvdialog.h"

#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

CheckKDVDialog::CheckKDVDialog(QWidget *parent) : AbstractCheckDialog(parent)
{

    m_BdUIList = { { "Основные", Bd1W() }, { "Модель", Bd2W() }, { "Ресурс", Bd3W() }, { "1-я гармоника", Bd4W() },
        { "Несимметрия", Bd5W() } };
    m_BdUIList.first().widget->setUpdatesEnabled();

    Timer->setInterval(ANMEASINT);
}

void CheckKDVDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKDVDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}

// check !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

UWidget *CheckKDVDialog::Bd1W()
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QStringList phase { "ф. A", "ф. B", "ф. C", "ср." };
    QStringList pphase { "ф. A", "ф. B", "ф. C", "сум." };
    QStringList ppphase { "AB", "BC", "CA", "ср." };

    //...................................

    QGroupBox *gb = new QGroupBox("Общие");

    glyout->addWidget(WDFunc::NewLBL2(this, "Температура ННТ, °С"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4500), ValuesFormat, "Температура ННТ обмотки (расчетная), °С"), 1, 0,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Tamb, °С"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4501), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "Cопротивление термометра, °С"), 0, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4502), ValuesFormat, "сопротивление термометра окружающей среды, Ом"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Частота, Гц"), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2400), ValuesFormat, "Частота, Гц"), 1, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Температура микроконтроллера, °С"), 0, 4, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(101), ValuesFormat, "Температура микроконтроллера, °С"), 1, 4, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ...................................

    gb = new QGroupBox("Электрические параметры");

    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 4; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL2(this, "Ueff " + phase[i] + ", кВ"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1000 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, кВ"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "Ieff " + phase[i] + ", А"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1400 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, А"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "P " + pphase[i] + ", кВт"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2420 + i), ValuesFormat,
                              "Истинная активная мощность по фазам и суммарная, кВт"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "Q " + pphase[i] + ", кВАр"), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2424 + i), ValuesFormat,
                              "Реактивная мощность по кажущейся полной "
                              "и истинной активной, кВАр"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "S " + pphase[i] + ", кВА"), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2428 + i), ValuesFormat,
                              "Кажущаяся полная мощность по эфф. токам и нпарямжениям, кВА"),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "CosPhi " + phase[i] + ""), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2432 + i), ValuesFormat,
                              "Косинус phi по истинной активной мощности,по фазам и средний "),
            13, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "Uлин " + ppphase[i] + ", кВ"), 14, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1220 + i), ValuesFormat,
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
    w->setFloatBdQuery({ { 101, 2 }, { 1000, 11 }, { 1400, 11 }, { 2400, 7 }, { 2420, 32 }, { 4500, 7 }, { 1220, 8 } });
    return w;
}

UWidget *CheckKDVDialog::Bd2W()
{
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QGroupBox *gb = new QGroupBox("Оставшееся время работы до предупреждения");

    glyout->addWidget(WDFunc::NewLBL2(this, "При данной нагрузке, сек"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4010), ValuesFormat,
                          "Оставшееся время работы до "
                          "предупрежде-ния при данной нагрузке, сек"),
        1, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    //...................................

    gb = new QGroupBox("Допустимое оставшееся время работы");

    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(this, "При данной нагрузке, сек"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4011), ValuesFormat,
                          "Допустимое оставшееся время работы при данной нагрузке, сек"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 105%, сек"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4012), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 105%, сек"),
        1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 110%, сек"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4013), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 110%, сек"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 115%, сек"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4014), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 115%, сек"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 120%, сек"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4015), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 120%, сек"),
        3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 125%, сек"), 2, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4016), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 125%, сек"),
        3, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 130%, сек"), 4, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4017), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 130%, сек"),
        5, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 135%, сек"), 4, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4018), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 135%, сек"),
        5, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 140%, сек"), 4, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4019), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 140%, сек"),
        5, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 145%, сек"), 6, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4020), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 145%, сек"),
        7, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 150%, сек"), 6, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4021), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 150%, сек"),
        7, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 155%, сек"), 6, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4022), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 155%, сек"),
        7, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 160%, сек"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4023), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 160%, сек"),
        9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 165%, сек"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4024), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 165%, сек"),
        9, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 170%, сек"), 8, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4025), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 170%, сек"),
        9, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 175%, сек"), 10, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4026), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 175%, сек"),
        11, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 180%, сек"), 10, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4027), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 180%, сек"),
        11, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 185%, сек"), 10, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4028), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 185%, сек"),
        11, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 190%, сек"), 12, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4029), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 190%, сек"),
        13, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 195%, сек"), 12, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4030), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 195%, сек"),
        13, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "При нагрузке 200%, сек"), 12, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4031), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 200%, сек"),
        13, 2, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);

    w->setFloatBdQuery({ { 4010, 22 } });
    return w;
}

UWidget *CheckKDVDialog::Bd3W()
{
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QGroupBox *gb = new QGroupBox("Изоляция");

    glyout->addWidget(WDFunc::NewLBL2(this, "Относительная скорость старения изоляции"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4700), ValuesFormat, "Относительная скорость старения изоляции"), 1,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Интегральное старение изоляции, час"), 2, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4701), ValuesFormat, "Интегральное старение изоляции, час"), 3, 0, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Остаточный ресурс изоляции, час"), 4, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4702), ValuesFormat, "Остаточный ресурс изоляции, час"), 5, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    //...................................

    gb = new QGroupBox("Наработка");

    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(this, "Наработка при нормальной нагрузке, час"), 6, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4703), ValuesFormat, "Наработка при нормальной нагрузке, час"), 7, 0,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Наработка в режиме перегрузки, час"), 8, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4704), ValuesFormat, "Наработка в режиме перегрузки, час"), 9, 0, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Общая наработка, час"), 10, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4705), ValuesFormat, "Общая наработка, час"), 11, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Амплитуда пускового тока при последнем пуске");
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(this, "Амплитуда, А"), 12, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(5200), ValuesFormat,
                          "Амплитуда пускового тока при последнем пуске, А"),
        13, 0, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "Дата и время пуска:"), 12, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "IsTime", "dd-MM-yyyy HH:mm:ss"), 13, 1, 1, 4);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 4700, 6 } });
    return w;
}

UWidget *CheckKDVDialog::Bd4W()
{
    int i;
    QStringList phase { "ф. A", "ф. B", "ф. C", "ср." };
    QStringList pphase { "ф. A", "ф. B", "ф. C", "сум." };
    QStringList ppphase { "AB", "BC", "CA", "ср." };
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;

    //...................................

    QGroupBox *gb = new QGroupBox("Электрические параметры по 1-й гармонике");

    QGridLayout *glyout = new QGridLayout;
    for (i = 0; i < 4; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL2(this, "Ueff " + phase[i] + ", кВ"), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1004 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз "
                              "по 1-й гармонике и их среднее, кВ"),
            1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "Ieff " + phase[i] + ", А"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1404 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз "
                              "по 1-й гармонике и их среднее, А"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "P " + pphase[i] + ", кВт"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2436 + i), ValuesFormat,
                              "Активная мощность по 1-й гармонике, по фазам и суммарная, кВт"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "Q " + pphase[i] + ", кВАр"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2440 + i), ValuesFormat,
                              "Реактивня мощность по 1-й гармонике, кВАр"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "S " + pphase[i] + ", кВА"), 8, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2444 + i), ValuesFormat, "Полная мощность по 1-й гармонике, кВА"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "CosPhi " + phase[i] + ""), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2448 + i), ValuesFormat,
                              "Косинус phi по 1-й гармонике,по фазам и средний "),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(this, "Uлин " + ppphase[i] + ", кВ"), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1224 + i), ValuesFormat,
                              "Действующие значения 1-й гармоники линейных "
                              "напряжений трех фази их среднее, кВ"),
            13, i, 1, 1);
    }
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 1000, 11 }, { 1400, 11 }, { 2420, 32 }, { 1220, 8 } });
    return w;
}

UWidget *CheckKDVDialog::Bd5W()
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QStringList phase { "ф. A", "ф. B", "ф. C" };

    //...................................

    QGroupBox *gb = new QGroupBox("Данные по напряжению");

    glyout->addWidget(WDFunc::NewLBL2(this, "U0, кВ"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1011), ValuesFormat,
                          "Напряжение нулевой последовательности гр.1, кВ"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "U1, кВ"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1012), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "U2, кВ"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1013), ValuesFormat, "Частота, Гц"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "KunsimU0"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1014), ValuesFormat,
                          "Коэффициент несимметрии напряжения по "
                          "обратной последовательности гр.1, %"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "KunsimU2"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1015), ValuesFormat,
                          "Коэффициент несимметрии напряжения по "
                          "нулевой последовательности гр.1, %"),
        3, 1, 1, 1);
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL2(this, "THD" + phase[i] + ""), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1016 + i), ValuesFormat,
                              "Общий коэффициент гарм. искажений напряжения фазы " + phase[i] + " гр.1"),
            5, i, 1, 1);
    }
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL2(this, "KrF" + phase[i] + ""), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1008 + i), ValuesFormat,
                              "Крест-фактор напряжения по трем фазам" + phase[i] + " гр.1"),
            7, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Данные по току");
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(this, "I0, кВ"), 6, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1411), ValuesFormat, "Ток нулевой последовательности гр.1, A"), 7, 0,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "I1, кВ"), 6, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1412), ValuesFormat, "Ток прямой последовательности гр.1, А"), 7, 1,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "I2, А"), 6, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1413), ValuesFormat, "Ток обратной последовательности гр.1, А"), 7, 2,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "KunsimI0"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1414), ValuesFormat,
                          "Коэффициент несимметрии тока по обратной последовательности гр.1,%"),
        9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "KunsimI2"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1415), ValuesFormat,
                          "Коэффициент несимметрии тока по нулевой последовательности гр.1,%"),
        9, 1, 1, 1);
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL2(this, "THD" + phase[i] + ""), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1416 + i), ValuesFormat,
                              "Общий коэффициент гарм. искажений тока фазы " + phase[i] + " гр.1"),
            11, i, 1, 1);
    }
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL2(this, "KrF" + phase[i] + ""), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1408 + i), ValuesFormat,
                              "Крест-фактор тока по трем фазам" + phase[i] + " гр.1"),
            13, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 1000, 11 }, { 1011, 8 }, { 1400, 11 }, { 1411, 8 } });
    return w;
}
