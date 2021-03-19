#include "confkdvdialog.h"

#include "../config/configkdv.h"
#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/s2.h"
#include "../widgets/ecombobox.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <bitset>

constexpr int textColumn = 0;
constexpr int warnColumn = 1;
constexpr int accColumn = 2;

ConfKDVDialog::ConfKDVDialog(ConfigKDV *ckdv, QWidget *parent) : AbstractConfDialog(parent)
{
    ConfKDV = ckdv;
}

ConfKDVDialog::~ConfKDVDialog()
{
    delete ConfKDV;
}

void ConfKDVDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;

    ConfTW->setObjectName("conftw");
    ConfTW->addTab(analogWidget(), "Аналоговые");
    ConfTW->addTab(settingWidget(), "Уставки");

    ConfTW->addTab(connectionWidget(), "Связь");
    ConfTW->addTab(ConfKDV->KxxConfig()->ModbusWidget(this), "ModBusMaster");
    ConfTW->addTab(ConfKDV->KxxConfig()->VariousWidget(this), "Общее");

    ConfTW->addTab(otherWidget(), "Остальное");

    lyout->addWidget(ConfTW);
    lyout->addWidget(ConfButtons());
    setLayout(lyout);
}

QWidget *ConfKDVDialog::analogWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    QScrollArea *scrollArea = new QScrollArea;
    gridlyout->setAlignment(Qt::AlignVCenter);

    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);

    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

    //    QLabel *lbl = new QLabel("Тип контролируемого оборудования:");
    //    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    //    QStringList cbl = QStringList { "Асинхронный двигатель", "Трансформатор", "Реактор" };
    //    auto *cb = WDFunc::NewCB2(this, nameByValue(BciNumber::Eq_type), cbl);
    //    cb->setMinimumHeight(20);
    //    gridlyout->addWidget(cb, row, 2, 1, 3);
    //    row++;

    //    lbl = new QLabel("Вид охлаждения:");
    //    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    //    cbl = QStringList { "Естественное", "Принудительное" };
    //    cb = WDFunc::NewCB2(this, nameByValue(BciNumber::Cool_type), cbl);
    //    cb->setMinimumHeight(20);
    //    gridlyout->addWidget(cb, row, 2, 1, 3);
    //    row++;

    //    lbl = new QLabel("Материал обмотки:");
    //    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    //    cbl = QStringList { "Медь", "Алюминий" };
    //    cb = WDFunc::NewCB2(this, nameByValue(BciNumber::W_mat), cbl);
    //    cb->setMinimumHeight(20);
    //    gridlyout->addWidget(cb, row, 2, 1, 3);
    //    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Unom1), 0, 10000, 2), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::U2nom), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный первичный ток, кА:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::ITT1nom_KTF_KDV), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный вторичный ток, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::ITT2nom_KTF_KDV), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный ток контролируемой обмотки, À:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Iwnom), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Tamb_nom), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dTNNTnom), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент добавочных потерь:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Kdob), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TauWnom), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальное измеряемое фазное напряжение на входе "
                             "прибора, В эфф (не более 305В):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Umaxm), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальный измеряемый ток на входе "
                             "прибора, А эфф  (не более 33А):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Imax), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Количество датчиков температуры обмоток, "
                             "подключенных по Modbus Master:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TdatNum), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная частота, Гц"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Fnom), 0, 1000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное скольжение"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::nom_slip), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::UVmax), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    scrollArea->setWidget(w);

    return scrollArea;
}

QWidget *ConfKDVDialog::settingWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);

    lyout->addWidget(alarmWidget());
    lyout->addWidget(hystWidget());
    w->setLayout(lyout);
    scrollArea->setWidget(w);

    return scrollArea;
}

QWidget *ConfKDVDialog::alarmWidget()
{
    auto *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Уставки сигнализации", this);

    vlyout->addWidget(commonAlarmWidget());

    vlyout->addWidget(vibrAlarmWidget());

    gb->setLayout(vlyout);
    return gb;
}

QWidget *ConfKDVDialog::commonAlarmWidget()
{

    int row = 0;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QGroupBox *gb = new QGroupBox("Общие", this);
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка скачка напряжения для запуска "
                             "осциллографирования - % от номинала:"),
        row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::DUosc), 0, 10000, 1), row, warnColumn);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::DIosc_ID), 0, 10000, 1), row, warnColumn);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка порога минимального напряжения - % от номинального уровня:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::DUImin_ID), 0, 1000, 1), row, warnColumn);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка порога минимального тока - % от номинального уровня:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Imin), 0, 10000, 1), row, warnColumn);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Предупредительная уставка сигнализации по температуре ННТ в °С:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TNNTpred), 0, 10000, 1), row, warnColumn);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Аварийная уставка сигнализации по температуре ННТ в °С:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TNNTdop), 0, 10000, 1), row, warnColumn);
    row++;
    gb->setLayout(gridlyout);
    return gb;
}

QWidget *ConfKDVDialog::vibrAlarmWidget()
{
    int row = 0;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QGroupBox *gb = new QGroupBox(this);

    gridlyout->addWidget(new QLabel("Предупредительные", this), row, warnColumn);
    gridlyout->addWidget(new QLabel("Аварийные", this), row, accColumn);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка по СКЗ виброускорения, м/с/с:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VibrA_pred), 0, 1000, 1), row, warnColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VibrA_alarm), 0, 1000, 1), row, accColumn);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка по СКЗ виброскорости, мм/с:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VibrV_pred), 0, 10000, 1), row, warnColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VibrV_alarm), 0, 10000, 1), row, accColumn);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка по СКЗ виброперемещения, мкм:"), row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VibrD_pred), 0, 10000, 1), row, warnColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VibrD_alarm), 0, 10000, 1), row, accColumn);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка по скорости роста СКЗ "
                             "виброускорения, м/с/с:"),
        row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VVibrA_pred), 0, 1000, 1), row, warnColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VVibrA_alarm), 0, 1000, 1), row, accColumn);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка по скорости роста СКЗ "
                             "виброскорости, мм/с:"),
        row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VVibrV_pred), 0, 10000, 1), row, warnColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VVibrV_alarm), 0, 10000, 1), row, accColumn);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка по скорости роста СКЗ "
                             "виброперемещения, мкм:"),
        row, textColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VVibrD_pred), 0, 10000, 1), row, warnColumn);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VVibrD_alarm), 0, 10000, 1), row, accColumn);
    gb->setLayout(gridlyout);
    return gb;
}

QWidget *ConfKDVDialog::hystWidget()
{
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QGroupBox *gb = new QGroupBox("Гистерезис");

    int row = 0;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::GTnnt), 0, 10000, 3), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис сигнализации по токовой перегрузке, % от Iwnom:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::GOvc), 0, 10000, 1), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    return gb;
}

QWidget *ConfKDVDialog::otherWidget()
{
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);

    lyout->addWidget(recordWidget());
    lyout->addWidget(motorWidget());
    w->setLayout(lyout);
    scrollArea->setWidget(w);
    return scrollArea;
}

QWidget *ConfKDVDialog::recordWidget()
{
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);

    gridlyout->setColumnStretch(2, 50);
    int row = 0;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::NFiltr_ID), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::T_Data_Rec), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Постоянная времени фильтрации гармоник:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::NHarmFilt_ID), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Запуск осциллограммы:"), row, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewChB2(this, "oscchb.0", "по команде Ц"));
    //    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", Colors::ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB2(this, "oscchb.2", "по резкому изменению"));
    gridlyout->addLayout(hlyout, row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Количество точек осциллограммы на период основной частоты:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::OscPoints), 0, 10000, 0), row, 2, 1, 3);
    QGroupBox *gb = new QGroupBox("Параметры записи");
    gb->setLayout(gridlyout);
    return gb;
}

QWidget *ConfKDVDialog::motorWidget()
{
    QGroupBox *gb = new QGroupBox("Параметры двигателя");
    QGridLayout *gridlyout = new QGridLayout;

    int row = 0;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент передачи датчиков вибрации:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Kvibr), 0, 10000, 2), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Порядковый номер двигателя:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::NumA_KDV), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число пар полюсов (от 1 до 10):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Poles), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число пазов на статоре:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Stator_Slotes), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число стержней ротора:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Rotor_bars), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Тип вибродатчика:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::VibroType), 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Подключенные датчики вибрации:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Sensors), 0, 10000, 0), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    return gb;
}

QWidget *ConfKDVDialog::connectionWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;

    QGridLayout *gridlyout = new QGridLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");

    gridlyout->addWidget(ConfKDV->MainConfig()->MainWidget(this), 0, 0, 1, 1);
    gridlyout->addWidget(ConfKDV->KxxConfig()->ComParam(this), 0, 1, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка времени");

    vlyout->addWidget(ConfKDV->MainConfig()->TimeWidget(this));

    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

void ConfKDVDialog::Fill()
{

    ConfKDV->MainConfig()->Fill();
    ConfKDV->KxxConfig()->Fill();
    FillKdv();
}

void ConfKDVDialog::FillBack() const
{

    ConfKDV->MainConfig()->FillBack();
    ConfKDV->KxxConfig()->FillBack();
    FillBackKdv();
}

void ConfKDVDialog::FillKdv()
{

    using namespace DataTypes;

    // SetCBIndex<DWORD>(this, BciNumber::Eq_type);
    //    SetCBIndex<DWORD>(this, BciNumber::Cool_type);
    //    SetCBIndex<DWORD>(this, BciNumber::W_mat);

    WDFunc::SetChBData(this, "oscchb.0", S2::getRecord(BciNumber::DDOsc_ID).value<DWORD>() & 0x0001);

    WDFunc::SetChBData(this, "oscchb.2", S2::getRecord(BciNumber::DDOsc_ID).value<DWORD>() & 0x0004);

    SetSPBData<DWORD>(this, BciNumber::NFiltr_ID);
    SetSPBData<DWORD>(this, BciNumber::NHarmFilt_ID);

    SetSPBData<float>(this, BciNumber::Unom1);
    SetSPBData<float>(this, BciNumber::U2nom);
    SetSPBData<float>(this, BciNumber::ITT1nom_KTF_KDV);
    SetSPBData<float>(this, BciNumber::ITT2nom_KTF_KDV);
    SetSPBData<float>(this, BciNumber::Iwnom);
    SetSPBData<float>(this, BciNumber::DUosc);
    SetSPBData<float>(this, BciNumber::DIosc_ID);

    SetSPBData<float>(this, BciNumber::DUImin_ID);
    SetSPBData<float>(this, BciNumber::Imin);
    SetSPBData<float>(this, BciNumber::TNNTdop);
    SetSPBData<float>(this, BciNumber::TNNTpred);
    SetSPBData<float>(this, BciNumber::Tamb_nom);
    SetSPBData<float>(this, BciNumber::dTNNTnom);
    SetSPBData<float>(this, BciNumber::Kdob);
    SetSPBData<float>(this, BciNumber::TauWnom);
    SetSPBData<float>(this, BciNumber::Umaxm); //проверить конец

    SetSPBData<float>(this, BciNumber::Imaxm); //проверить конец
    SetSPBData<float>(this, BciNumber::GTnnt);
    SetSPBData<float>(this, BciNumber::GOvc);
    SetSPBData<float>(this, BciNumber::Fnom);
    SetSPBData<float>(this, BciNumber::nom_slip);
    SetSPBData<float>(this, BciNumber::UVmax);
    SetSPBData<float>(this, BciNumber::Kvibr);

    SetSPBData<float>(this, BciNumber::VibrA_pred);
    SetSPBData<float>(this, BciNumber::VibrV_pred);
    SetSPBData<float>(this, BciNumber::VibrD_pred);
    SetSPBData<float>(this, BciNumber::VibrA_alarm);
    SetSPBData<float>(this, BciNumber::VibrV_alarm);
    SetSPBData<float>(this, BciNumber::VibrD_alarm);
    SetSPBData<float>(this, BciNumber::VVibrA_pred);
    SetSPBData<float>(this, BciNumber::VVibrV_pred);
    SetSPBData<float>(this, BciNumber::VVibrD_pred);
    SetSPBData<float>(this, BciNumber::VVibrA_alarm);
    SetSPBData<float>(this, BciNumber::VVibrV_alarm);
    SetSPBData<float>(this, BciNumber::VVibrD_alarm);

    SetSPBData<DWORD>(this, BciNumber::NumA_KDV);
    SetSPBData<DWORD>(this, BciNumber::Poles);
    SetSPBData<DWORD>(this, BciNumber::Stator_Slotes);
    SetSPBData<DWORD>(this, BciNumber::Rotor_bars);
    SetSPBData<DWORD>(this, BciNumber::VibroType);
    SetSPBData<DWORD>(this, BciNumber::Sensors);
    SetSPBData<DWORD>(this, BciNumber::T_Data_Rec);
    SetSPBData<DWORD>(this, BciNumber::OscPoints);
    SetSPBData<DWORD>(this, BciNumber::TdatNum);
}

void ConfKDVDialog::FillBackKdv() const
{

    using namespace DataTypes;

    SPBDataS2<DWORD>(this, BciNumber::NFiltr_ID);
    SPBDataS2<DWORD>(this, BciNumber::NHarmFilt_ID);
    SPBDataS2<float>(this, BciNumber::Unom1);
    SPBDataS2<float>(this, BciNumber::U2nom);
    SPBDataS2<float>(this, BciNumber::ITT1nom_KTF_KDV);
    SPBDataS2<float>(this, BciNumber::ITT2nom_KTF_KDV);
    SPBDataS2<float>(this, BciNumber::Iwnom);
    SPBDataS2<float>(this, BciNumber::DUosc);
    SPBDataS2<float>(this, BciNumber::DIosc_ID);
    SPBDataS2<float>(this, BciNumber::DUImin_ID);
    SPBDataS2<float>(this, BciNumber::Imin);
    SPBDataS2<float>(this, BciNumber::TNNTdop);
    SPBDataS2<float>(this, BciNumber::TNNTpred);
    SPBDataS2<float>(this, BciNumber::Tamb_nom);
    SPBDataS2<float>(this, BciNumber::dTNNTnom);
    SPBDataS2<float>(this, BciNumber::Kdob);
    SPBDataS2<float>(this, BciNumber::TauWnom);
    SPBDataS2<float>(this, BciNumber::Umaxm);

    SPBDataS2<float>(this, BciNumber::Imaxm); //проверить конец
    SPBDataS2<float>(this, BciNumber::GTnnt);
    SPBDataS2<float>(this, BciNumber::GOvc);
    SPBDataS2<float>(this, BciNumber::Fnom);
    SPBDataS2<float>(this, BciNumber::nom_slip);
    SPBDataS2<float>(this, BciNumber::UVmax);
    SPBDataS2<float>(this, BciNumber::Kvibr);

    SPBDataS2<float>(this, BciNumber::VibrA_pred);
    SPBDataS2<float>(this, BciNumber::VibrV_pred);
    SPBDataS2<float>(this, BciNumber::VibrD_pred);
    SPBDataS2<float>(this, BciNumber::VibrA_alarm);
    SPBDataS2<float>(this, BciNumber::VibrV_alarm);
    SPBDataS2<float>(this, BciNumber::VibrD_alarm);
    SPBDataS2<float>(this, BciNumber::VVibrA_pred);
    SPBDataS2<float>(this, BciNumber::VVibrV_pred);
    SPBDataS2<float>(this, BciNumber::VVibrD_pred);
    SPBDataS2<float>(this, BciNumber::VVibrA_alarm);
    SPBDataS2<float>(this, BciNumber::VVibrV_alarm);
    SPBDataS2<float>(this, BciNumber::VVibrD_alarm);

    SPBDataS2<DWORD>(this, BciNumber::NumA_KDV);
    SPBDataS2<DWORD>(this, BciNumber::Poles);
    SPBDataS2<DWORD>(this, BciNumber::Stator_Slotes);
    SPBDataS2<DWORD>(this, BciNumber::Rotor_bars);
    SPBDataS2<DWORD>(this, BciNumber::VibroType);
    SPBDataS2<DWORD>(this, BciNumber::Sensors);
    SPBDataS2<DWORD>(this, BciNumber::T_Data_Rec);
    SPBDataS2<DWORD>(this, BciNumber::OscPoints);
    SPBDataS2<DWORD>(this, BciNumber::TdatNum);
}

void ConfKDVDialog::CheckConf() const
{
}

void ConfKDVDialog::SetDefConf()
{
    ConfKDV->SetDefConf();
    Fill();
}
