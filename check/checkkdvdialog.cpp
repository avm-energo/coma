#include "checkkdvdialog.h"

#include "../config/config.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../gen/datamanager.h"
#include "../widgets/wd_func.h"
#include "../widgets/etabwidget.h"

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
    setAttribute(Qt::WA_DeleteOnClose);

    m_BdUIList = { { "Основные", Bd1W() }, { "Модель", Bd2W() } , { "Ресурс", Bd3W() }, { "1-я гармоника", Bd4W() }, { "Несимметрия", Bd5W() }};
    SetupUI();

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


UWidget *CheckKDVDialog::Bd1W()
{
    int i;
    UWidget *w = new UWidget;
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

    glyout->addWidget(WDFunc::NewLBL(this, "Температура ННТ, °С"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4500), ValuesFormat, "Температура ННТ обмотки (расчетная), °С"), 1,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Tamb, °С"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4501), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1,
        1);

    glyout->addWidget(WDFunc::NewLBL(this, "Cопротивление термометра, °С"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4502), ValuesFormat,
                          "сопротивление термометра окружающей среды, Ом"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Частота, Гц"), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2400), ValuesFormat, "Частота, Гц"), 1, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Температура микроконтроллера, °С"), 0, 4, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(101), ValuesFormat, "Температура микроконтроллера, °С"), 1, 4, 1,
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

        glyout->addWidget(WDFunc::NewLBL(this, "Ueff " + phase[i] + ", кВ"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1000 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, кВ"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Ieff " + phase[i] + ", А"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1400 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, А"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "P " + pphase[i] + ", кВт"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2420 + i), ValuesFormat,
                              "Истинная активная мощность по фазам и суммарная, кВт"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Q " + pphase[i] + ", кВАр"), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2424 + i), ValuesFormat,
                              "Реактивная мощность по кажущейся полной "
                              "и истинной активной, кВАр"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "S " + pphase[i] + ", кВА"), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2428 + i), ValuesFormat,
                              "Кажущаяся полная мощность по эфф. токам и нпарямжениям, кВА"),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "CosPhi " + phase[i] + ""), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2432 + i), ValuesFormat,
                              "Косинус phi по истинной активной мощности,по фазам и средний "),
            13, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Uлин " + ppphase[i] + ", кВ"), 14, i, 1, 1);
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
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
    w->setFloatBdQuery({ { 101, 2 }, { 1000, 4 }, { 1400, 4 }, { 2400, 1 }, { 2420, 16 }, { 4500, 3 },{ 1220, 4 } });
    return w;
}

UWidget *CheckKDVDialog::Bd2W()
{
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Оставшееся время работы до предупреждения");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    glyout->addWidget(WDFunc::NewLBL(this, "При данной нагрузке, сек"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4010), ValuesFormat,
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

    glyout->addWidget(WDFunc::NewLBL(this, "При данной нагрузке, сек"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4011), ValuesFormat,
                          "Допустимое оставшееся время работы при данной нагрузке, сек"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 105%, сек"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4012), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 105%, сек"),
        1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 110%, сек"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4013), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 110%, сек"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 115%, сек"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4014), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 115%, сек"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 120%, сек"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4015), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 120%, сек"),
        3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 125%, сек"), 2, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4016), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 125%, сек"),
        3, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 130%, сек"), 4, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4017), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 130%, сек"),
        5, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 135%, сек"), 4, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4018), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 135%, сек"),
        5, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 140%, сек"), 4, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4019), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 140%, сек"),
        5, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 145%, сек"), 6, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4020), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 145%, сек"),
        7, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 150%, сек"), 6, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4021), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 150%, сек"),
        7, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 155%, сек"), 6, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4022), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 155%, сек"),
        7, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 160%, сек"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4023), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 160%, сек"),
        9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 165%, сек"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4024), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 165%, сек"),
        9, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 170%, сек"), 8, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4025), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 170%, сек"),
        9, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 175%, сек"), 10, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4026), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 175%, сек"),
        11, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 180%, сек"), 10, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4027), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 180%, сек"),
        11, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 185%, сек"), 10, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4028), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 185%, сек"),
        11, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 190%, сек"), 12, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4029), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 190%, сек"),
        13, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 195%, сек"), 12, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4030), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 195%, сек"),
        13, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 200%, сек"), 12, 2, 1, 1);
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
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
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

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Изоляция");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    glyout->addWidget(WDFunc::NewLBL(this, "Относительная скорость старения изоляции"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4000), ValuesFormat, "Относительная скорость старения изоляции"), 1,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Интегральное старение изоляции, час"), 2, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4001), ValuesFormat, "Интегральное старение изоляции, час"), 3, 0,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Остаточный ресурс изоляции, час"), 4, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4002), ValuesFormat, "Остаточный ресурс изоляции, час"), 5, 0, 1,
        1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    //...................................

    gb = new QGroupBox("Наработка");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Наработка при нормальной нагрузке, час"), 6, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4003), ValuesFormat, "Наработка при нормальной нагрузке, час"), 7,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Наработка в режиме перегрузки, час"), 8, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4004), ValuesFormat, "Наработка в режиме перегрузки, час"), 9, 0, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(this, "Общая наработка, час"), 10, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4005), ValuesFormat, "Общая наработка, час"), 11, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Амплитуда пускового тока при последнем пуске");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Амплитуда, А"), 12, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(5200), ValuesFormat,
                          "Амплитуда пускового тока при последнем пуске, А"),
        13, 0, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время пуска:"), 12, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "IsTime", "dd-MM-yyyy HH:mm:ss"), 13, 1, 1, 4);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
    w->setFloatBdQuery({ { 4003, 3 }, { 5200, 1 } });
    return w;
}

UWidget *CheckKDVDialog::Bd4W()
{
    int i;
     UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "ф. A", "ф. B", "ф. C", "ср." };
    QString pphase[4] = { "ф. A", "ф. B", "ф. C", "сум." };
    QString ppphase[4] = { "AB", "BC", "CA", "ср." };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox;
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Электрические параметры по 1-й гармонике");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 4; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(this, "Ueff " + phase[i] + ", кВ"), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1004 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз "
                              "по 1-й гармонике и их среднее, кВ"),
            1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Ieff " + phase[i] + ", А"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1404 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз "
                              "по 1-й гармонике и их среднее, А"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "P " + pphase[i] + ", кВт"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2450 + i), ValuesFormat,
                              "Активная мощность по 1-й гармонике, по фазам и суммарная, кВт"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Q " + pphase[i] + ", кВАр"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2454 + i), ValuesFormat,
                              "Реактивня мощность по 1-й гармонике, кВАр"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "S " + pphase[i] + ", кВА"), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2458 + i), ValuesFormat,
                              "Полная мощность по 1-й гармонике, кВА"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "CosPhi " + phase[i] + ""), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2462 + i), ValuesFormat,
                              "Косинус phi по 1-й гармонике,по фазам и средний "),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Uлин " + ppphase[i] + ", кВ"), 12, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1224 + i), ValuesFormat,
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
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
     w->setFloatBdQuery({{ 1004, 4 }, { 1404, 4 }, { 2450, 16 }, { 1224, 4 } });
    return w;
}

UWidget *CheckKDVDialog::Bd5W()
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф. A", "ф. B", "ф. C" };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox;
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Данные по напряжению");
    gb->setFont(ffont);

    glyout->addWidget(WDFunc::NewLBL(this, "U0, кВ"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1020), ValuesFormat,
                          "Напряжение нулевой последовательности гр.1, кВ"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U1, кВ"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1021), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2, кВ"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1022), ValuesFormat, "Частота, Гц"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "KunsimU0"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1023), ValuesFormat,
                          "Коэффициент несимметрии напряжения по "
                          "обратной последовательности гр.1, %"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "KunsimU2"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1024), ValuesFormat,
                          "Коэффициент несимметрии напряжения по "
                          "нулевой последовательности гр.1, %"),
        3, 1, 1, 1);
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(this, "THD" + phase[i] + ""), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1025 + i), ValuesFormat,
                              "Общий коэффициент гарм. искажений напряжения фазы " + phase[i] + " гр.1"),
            5, i, 1, 1);
    }
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(this, "KrF" + phase[i] + ""), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1008 + i), ValuesFormat,
                              "Крест-фактор напряжения по трем фазам" + phase[i] + " гр.1"),
            7, i, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Данные по току");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "I0, кВ"), 6, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1420), ValuesFormat, "Ток нулевой последовательности гр.1, A"), 7,
        0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "I1, кВ"), 6, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1421), ValuesFormat, "Ток прямой последовательности гр.1, А"), 7, 1,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "I2, А"), 6, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1422), ValuesFormat, "Ток обратной последовательности гр.1, А"), 7,
        2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "KunsimI0"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1423), ValuesFormat,
                          "Коэффициент несимметрии тока по обратной последовательности гр.1,%"),
        9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "KunsimI2"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1424), ValuesFormat,
                          "Коэффициент несимметрии тока по нулевой последовательности гр.1,%"),
        9, 1, 1, 1);
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(this, "THD" + phase[i] + ""), 10, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1425 + i), ValuesFormat,
                              "Общий коэффициент гарм. искажений тока фазы " + phase[i] + " гр.1"),
            11, i, 1, 1);
    }
    for (i = 0; i < 3; ++i)
    {

        glyout->addWidget(WDFunc::NewLBL(this, "KrF" + phase[i] + ""), 12, i, 1, 1);
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
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
    w->setFloatBdQuery({ { 1020, 8 }, { 1008, 3 }, { 1420, 8 }, { 1408, 3 }});
    return w;
}
// void CheckKDVDialog::ChooseValuesToWrite()
//{
//}
// void CheckKDVDialog::SetDefaultValuesToWrite()
//{
//}
// void CheckKDVDialog::PrepareAnalogMeasurements()
//{
//}

// void CheckKDVDialog::setConnections()
//{
//    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &CheckKDVDialog::updateFloatData);
//}

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

// void CheckKDVDialog::SetWarnColor(int position, bool value)
//{
//    if ((position >= 0) && (position < 3))
//        WDFunc::SetLBLTColor(this, QString::number(1000 + position), (value) ? Colors::TABCOLORA1 :
//        Colors::ACONFOCLR);

//    if ((position >= 3) && (position < 6))
//        WDFunc::SetLBLTColor(
//            this, QString::number(1100 + position - 3), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);

//    if ((position >= 7) && (position < 10))
//        WDFunc::SetLBLTColor(
//            this, QString::number(1000 + position - 7), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);

//    if ((position >= 10) && (position < 13))
//        WDFunc::SetLBLTColor(
//            this, QString::number(2429 + position - 10), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
//    if ((position >= 13) && (position < 16))
//        WDFunc::SetLBLTColor(
//            this, QString::number(2426 + position - 13), (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
//    if (position == 17)
//        WDFunc::SetLBLTColor(this, "2432", (value) ? Colors::TABCOLORA1 : Colors::ACONFOCLR);
//}

// void CheckKDVDialog::SetAlarmColor(int position, bool value)
//{
//    if (position < 3)
//        WDFunc::SetLBLTColor(this, QString::number(2429 + position), (value) ? Colors::REDCOLOR : Colors::ACONFOCLR);
//    else if ((position >= 3) && (position < 6))
//        WDFunc::SetLBLTColor(
//            this, QString::number(2426 + position - 3), (value) ? Colors::REDCOLOR : Colors::ACONFOCLR);
//    else if (position == 6)
//        WDFunc::SetLBLTColor(this, QString::number(2432), (value) ? Colors::REDCOLOR : Colors::ACONFOCLR);
//}

// void CheckKDVDialog::updateFloatData(DataTypes::FloatStruct &floatsignal)
//{
//    if (updatesEnabled())
//        ChKDV->FillBd(this, QString::number(floatsignal.sigAdr), WDFunc::StringValueWithCheck(floatsignal.sigVal, 3));
//    //    {
//    //        QList<DataTypes::SignalsStruct> list;
//    //        DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
//    //        if (!list.isEmpty())
//    //        {
//    //            foreach (DataTypes::SignalsStruct signal, list)
//    //            {
//    //                DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
//    //                ChKDV->FillBd(this, QString::number(fwt.sigAdr), WDFunc::StringValueWithCheck(fwt.sigVal, 3));
//    //            }
//    //        }
//    //    }
//}

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
