#include "checkktfdialog.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../widgets/etabwidget.h"
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

CheckKTFDialog::CheckKTFDialog(QWidget *parent) : AbstractCheckDialog(parent)
{
    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
    setStyleSheet(tmps);
    QStringList sl;

    m_BdUIList = { { "Основные", Bd1W() }, { "Модель", Bd2W() }, { "Ресурс", Bd3W() }, { "1-я гармоника", Bd4W() },
        { "Несимметрия", Bd5W() } };
    SetupUI();

    Timer->setInterval(ANMEASINT);
}

CheckKTFDialog::~CheckKTFDialog()
{
}

void CheckKTFDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKTFDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

UWidget *CheckKTFDialog::Bd1W()
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
        WDFunc::NewLBLT(this, "", QString::number(4500), ValuesFormat, "Температура ННТ обмотки (расчетная), °С"), 1, 0,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Tamb, °С"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4501), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Cопротивление термометра, Ом"), 0, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4502), ValuesFormat, "сопротивление термометра окружающей среды, Ом"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Частота, Гц"), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2400), ValuesFormat, "Частота, Гц"), 1, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Температура микроконтроллера, °С"), 0, 4, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(101), ValuesFormat, "Температура микроконтроллера, °С"), 1, 4, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    //....................................................
    gb = new QGroupBox("Показания датчиков температуры обмоток, °С");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Датчик №1"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4503), ValuesFormat,
                          "Температура обмотки, измеренная встроен-ным датчиком №1, °С"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Датчик №2"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4504), ValuesFormat,
                          "Температура обмотки, измеренная встроен-ным датчиком №2, °С"),
        3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Датчик №3"), 2, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4505), ValuesFormat,
                          "Температура обмотки, измеренная встроен-ным датчиком №3, °С"),
        3, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Датчик №4"), 2, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4506), ValuesFormat,
                          "Температура обмотки, измеренная встроен-ным датчиком №4, °С"),
        3, 3, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ...................................

    gb = new QGroupBox("Электрические параметры");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    int str = 0, str2 = 1;
    for (i = 0; i < 4; ++i)
    {
        int row = 4;
        str2 = str + 1;
        glyout->addWidget(WDFunc::NewLBL(this, "Ueff " + phase[i] + ", кВ"), row, str, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1000 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, кВ"),
            row, str2, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "Ieff " + phase[i] + ", А"), row, str, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1400 + i), ValuesFormat,
                              "Истинные действующие значения сигналов трех фаз и их среднее, А"),
            row, str2, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "P " + pphase[i] + ", кВт"), row, str, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2420 + i), ValuesFormat,
                              "Истинная активная мощность по фазам и суммарная, кВт"),
            row, str2, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "Q " + pphase[i] + ", кВАр"), row, str, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2424 + i), ValuesFormat,
                              "Реактивная мощность по кажущейся полной "
                              "и истинной активной, кВАр"),
            row, str2, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "S " + pphase[i] + ", кВА"), row, str, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2428 + i), ValuesFormat,
                              "Кажущаяся полная мощность по эфф. токам и нпарямжениям, кВА"),
            row, str2, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "CosPhi " + phase[i] + ""), row, str, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2432 + i), ValuesFormat,
                              "Косинус phi по истинной активной мощности,по фазам и средний "),
            row, str2, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "Uлин " + ppphase[i] + ", кВ"), row, str, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(1220 + i), ValuesFormat,
                              "Истинные действующие значения линейных "
                              "напряжений трех фази их среднее, кВ"),
            row, str2, 1, 1);
        str = str + 2;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}");
    w->setFloatBdQuery(
        { { 101, 4 }, { 1000, 38 }, { 1400, 38 }, { 1220, 16 }, { 2400, 14 }, { 2420, 64 }, { 4500, 14 } });
    //   w->setSpBdQuery({ { 3011, 25 } });

    return w;
}

UWidget *CheckKTFDialog::Bd2W()
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

    glyout->addWidget(WDFunc::NewLBL(this, "При данной нагрузке, мин"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4010), ValuesFormat,
                          "Оставшееся время работы до "
                          "предупрежде-ния при данной нагрузке, мин"),
        1, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    //...................................

    gb = new QGroupBox("Допустимое оставшееся время работы");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "При данной нагрузке, мин"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4011), ValuesFormat,
                          "Допустимое оставшееся время работы при данной нагрузке, мин"),
        1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 105%, мин"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4012), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 105%, мин"),
        1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 110%, мин"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4013), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 110%, мин"),
        1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 115%, мин"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4014), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 115%, мин"),
        3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 120%, мин"), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4015), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 120%, мин"),
        3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 125%, мин"), 2, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4016), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 125%, мин"),
        3, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 130%, мин"), 4, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4017), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 130%, мин"),
        5, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 135%, мин"), 4, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4018), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 135%, мин"),
        5, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 140%, мин"), 4, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4019), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 140%, мин"),
        5, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 145%, мин"), 6, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4020), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 145%, мин"),
        7, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 150%, мин"), 6, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4021), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 150%, мин"),
        7, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 155%, мин"), 6, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4022), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 155%, мин"),
        7, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 160%, мин"), 8, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4023), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 160%, мин"),
        9, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 165%, мин"), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4024), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 165%, мин"),
        9, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 170%, мин"), 8, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4025), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 170%, мин"),
        9, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 175%, мин"), 10, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4026), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 175%, мин"),
        11, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 180%, мин"), 10, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4027), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 180%, мин"),
        11, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 185%, мин"), 10, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4028), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 185%, мин"),
        11, 2, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 190%, мин"), 12, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4029), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 190%, мин"),
        13, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 195%, мин"), 12, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4030), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 195%, мин"),
        13, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "При нагрузке 200%, мин"), 12, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(4031), ValuesFormat,
                          "Допустимое оставшееся время работы при нагрузке 200%, мин"),
        13, 2, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 4010, 44 } });

    return w;
}

UWidget *CheckKTFDialog::Bd3W()
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
        WDFunc::NewLBLT(this, "", QString::number(4001), ValuesFormat, "Интегральное старение изоляции, час"), 3, 0, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(this, "Остаточный ресурс изоляции, час"), 4, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(4002), ValuesFormat, "Остаточный ресурс изоляции, час"), 5, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Амплитуда пускового тока при последнем пуске");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Амплитуда, А"), 6, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(5200), ValuesFormat,
                          "Амплитуда пускового тока при последнем пуске, А"),
        7, 0, 1, 1);

    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время пуска:"), 6, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "IsTime", "dd-MM-yyyy HH:mm:ss"), 7, 1, 1, 4);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    // тип time  в процессе решения
    w->setFloatBdQuery({ { 4000, 6 } });

    return w;
}

UWidget *CheckKTFDialog::Bd4W()
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
        // проверять
        glyout->addWidget(WDFunc::NewLBL(this, "P " + pphase[i] + ", кВт"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2450 + i), ValuesFormat,
                              "Активная мощность по 1-й гармонике, по фазам и суммарная, кВт"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Q " + pphase[i] + ", кВАр"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(2454 + i), ValuesFormat,
                              "Реактивня мощность по 1-й гармонике, кВАр"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "S " + pphase[i] + ", кВА"), 8, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(2458 + i), ValuesFormat, "Полная мощность по 1-й гармонике, кВА"),
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
    // 2450 посмотреть на документацию, которая в разработке на 2420
    w->setFloatBdQuery({ { 1000, 38 }, { 1400, 38 }, { 2420, 64 }, { 1220, 16 } });

    return w;
}

UWidget *CheckKTFDialog::Bd5W()
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф. A", "ф. B", "ф. C" };
    // QString pphase[4] = {"ф. A","ф. B","ф. C","сум."};
    // QString ppphase[4] = {"AB","BC","CA","ср."};

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
        WDFunc::NewLBLT(this, "", QString::number(1021), ValuesFormat, "Температура окружающей среды, °С"), 1, 1, 1, 1);
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
        WDFunc::NewLBLT(this, "", QString::number(1420), ValuesFormat, "Ток нулевой последовательности гр.1, A"), 7, 0,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "I1, кВ"), 6, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1421), ValuesFormat, "Ток прямой последовательности гр.1, А"), 7, 1,
        1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "I2, А"), 6, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(this, "", QString::number(1422), ValuesFormat, "Ток обратной последовательности гр.1, А"), 7, 2,
        1, 1);
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
    w->setFloatBdQuery({ { 1020, 16 }, { 1000, 38 }, { 1400, 38 } });
    return w;
}
// void CheckKTFDialog::ChooseValuesToWrite() { }
// void CheckKTFDialog::SetDefaultValuesToWrite() { }
// void CheckKTFDialog::PrepareAnalogMeasurements() { }

// void CheckKTFDialog::USBUpdate()
//{
//    QTabWidget *CheckTW = this->findChild<QTabWidget *>("checktw1");
//    if (CheckTW == nullptr)
//    {
//        qDebug() << "CheckTW is null";
//        return;
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0))
//    {
//        if (Commands::GetBd(13, &ChKTF->Bd_block13, sizeof(CheckKTF::Bd13)) == Error::Msg::NoError)
//            ChKTF->FillBd13(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0))
//    {
//        if (Commands::GetBd(0, &ChKTF->Bd_block0, sizeof(CheckKTF::Bd0)) == Error::Msg::NoError)
//            ChKTF->FillBd0(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0))
//    {
//        if (Commands::GetBd(17, &ChKTF->Bd_block17, sizeof(CheckKTF::Bd17)) == Error::Msg::NoError)
//            ChKTF->FillBd17(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
//    {
//        if (Commands::GetBd(10, &ChKTF->Bd_block10, sizeof(CheckKTF::Bd10)) == Error::Msg::NoError)
//            ChKTF->FillBd10(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3)
//        || CheckTW->currentIndex() == IndexWd.at(4))
//    {
//        if (Commands::GetBd(2, &ChKTF->Bd_block2, sizeof(CheckKTF::Bd2)) == Error::Msg::NoError)
//            ChKTF->FillBd2(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3)
//        || CheckTW->currentIndex() == IndexWd.at(4))
//    {
//        if (Commands::GetBd(3, &ChKTF->Bd_block3, sizeof(CheckKTF::Bd3)) == Error::Msg::NoError)
//            ChKTF->FillBd3(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(0) || CheckTW->currentIndex() == IndexWd.at(3))
//    {
//        if (Commands::GetBd(8, &ChKTF->Bd_block8, sizeof(CheckKTF::Bd8)) == Error::Msg::NoError)
//            ChKTF->FillBd8(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(1))
//    {
//        if (Commands::GetBd(11, &ChKTF->Bd_block11, sizeof(CheckKTF::Bd11)) == Error::Msg::NoError)
//            ChKTF->FillBd11(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(2))
//    {
//        if (Commands::GetBd(9, &ChKTF->Bd_block9, sizeof(CheckKTF::Bd9)) == Error::Msg::NoError)
//            ChKTF->FillBd9(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(2))
//    {
//        if (Commands::GetBd(18, &ChKTF->Bd_block18, sizeof(CheckKTF::Bd18)) == Error::Msg::NoError)
//            ChKTF->FillBd18(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(4))
//    {
//        if (Commands::GetBd(4, &ChKTF->Bd_block4, sizeof(CheckKTF::Bd4)) == Error::Msg::NoError)
//            ChKTF->FillBd4(this);
//    }

//    if (CheckTW->currentIndex() == IndexWd.at(4))
//    {
//        if (Commands::GetBd(6, &ChKTF->Bd_block6, sizeof(CheckKTF::Bd6)) == Error::Msg::NoError)
//            ChKTF->FillBd6(this);
//    }
//}

// void CheckKTFDialog::ETHUpdate()
//{
//    updateFloatData();
//}

// void CheckKTFDialog::MBSUpdate()
//{
//}

// void CheckKTFDialog::updateFloatData()
//{
//    QList<DataTypes::SignalsStruct> list;
//    DataManager::getSignals(0, 99999, DataTypes::SignalTypes::FloatWithTime, list);
//    if (!list.isEmpty())
//    {
//        foreach (DataTypes::SignalsStruct signal, list)
//        {
//            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);
//            ChKTF->FillBd(this, QString::number(fwt.sigAdr), WDFunc::StringValueWithCheck(fwt.sigVal, 3));
//        }
//    }
//}

// void CheckKTFDialog::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
//{

//    // ModBusSignal sig = *new ModBusSignal;
//    int i = 0;
//    for (i = 0; i < Signal.size(); ++i)
//    {
//        // sig = *(Signal+i);
//        if ((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015)))
//            || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
//            ChKTF->FillBd(
//                this, QString::number((Signal.at(i).SigAdr) + 9), WDFunc::StringValueWithCheck(Signal.at(i).flVal,
//                3));
//        else
//            ChKTF->FillBd(
//                this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
//    }
//}

// void CheckKTFDialog::SetWarnColor(int position, bool value)
//{
//    Q_UNUSED(position)
//    Q_UNUSED(value)
//}

// void CheckKTFDialog::reqUpdate()
//{
//    updateFloatData();
//}

// void CheckKTFDialog::SetAlarmColor(int position, bool value)
//{
//    Q_UNUSED(position)
//    Q_UNUSED(value)
//}
