#include "confkdvdialog.h"

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
#include <QScrollBar>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

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
    ConfTW->addTab(setWidget(), "Уставки");

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
    QScrollArea *Analog_area = new QScrollArea;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QFont font;

    Analog_area->setFrameShape(QFrame::NoFrame);
    Analog_area->setWidgetResizable(true);

    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    QLabel *lbl = new QLabel("Тип контролируемого оборудования:");
    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList { "Асинхронный двигатель", "Трансформатор", "Реактор" };
    auto *cb = WDFunc::NewCB2(this, "Eq_type", cbl);
    cb->setMinimumHeight(20);
    gridlyout->addWidget(cb, row, 2, 1, 3);
    row++;

    lbl = new QLabel("Вид охлаждения:");
    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "Естественное", "Принудительное" };
    cb = WDFunc::NewCB2(this, "Cool_type", cbl);
    cb->setMinimumHeight(20);
    gridlyout->addWidget(cb, row, 2, 1, 3);
    row++;

    lbl = new QLabel("Материал обмотки:");
    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "Медь", "Алюминий" };
    cb = WDFunc::NewCB2(this, "W_mat", cbl);
    cb->setMinimumHeight(20);
    gridlyout->addWidget(cb, row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Unom1", 0, 10000, 2), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "U2nom", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный первичный ток, кА:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT1nom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный вторичный ток, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT2nom", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный ток контролируемой обмотки, À:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Iwnom", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Tamb_nom", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dTNNTnom", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент добавочных потерь:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Kdob", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TauWnom", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальное измеряемое фазное напряжение на входе "
                             "прибора, В эфф (не более 305В):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Umax", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальный измеряемый ток на входе "
                             "прибора, А эфф  (не более 33А):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imax", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Количество датчиков температуры обмоток, "
                             "подключенных по Modbus Master:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TdatNum", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная частота, Гц"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Fnom", 0, 1000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное скольжение"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "nom_slip", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "UVmax", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    Analog_area->setWidget(w);

    return Analog_area;
}

QWidget *ConfKDVDialog::setWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    ;
    QFont font;
    int row = 0;

    QGroupBox *gb = new QGroupBox("Уставки сигнализации");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка скачка напряжения для запуска "
                             "осциллографирования - % от номинала:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "DUosc", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "DIosc", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка порога минимального напряжения - % от номинального уровня:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "DUImin", 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка порога минимального тока - % от номинального уровня:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imin", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Предельно допустимая температура ННТ в°С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TNNTdop", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TNNTpred", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Предупредительная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrA_pred", 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Предупредительная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrV_pred", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Предупредительная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrD_pred", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Аварийная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrA_alarm", 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Аварийная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrV_alarm", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Аварийная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrD_alarm", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Предупредительная уставка по скорости роста СКЗ "
                             "виброускорения, м/с/с:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrA_pred", 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Предупредительная уставка по скорости роста СКЗ "
                             "виброскорости, мм/с:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrV_pred", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Предупредительная уставка по скорости роста СКЗ "
                             "виброперемещения, мкм:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrD_pred", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Аварийная уставка по скорости роста СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrA_alarm", 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Аварийная уставка по скорости роста СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrV_alarm", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Аварийная уставка по скорости роста СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrD_alarm", 0, 10000, 1), row, 2, 1, 3);

    lyout->addLayout(gridlyout);
    gb->setLayout(lyout);
    lyout->addWidget(gb);

    //.....................................................................
    gb = new QGroupBox("Гистерезис");

    // vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "GTnnt", 0, 10000, 3), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис сигнализации по токовой перегрузке, % от Iwnom:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "GOvc", 0, 10000, 1), row, 2, 1, 3);

    //    gb->setLayout(gridlyout);
    //    lyout->addWidget(gb);
    //    w->setLayout(lyout);
    //    scrollArea->setWidget(w);

    //    return scrollArea;

    gb->setLayout(gridlyout);
    scrollArea->setWidget(w);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKDVDialog::otherWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QFont font;
    gridlyout->setColumnStretch(2, 50);
    int row = 0;

    QGroupBox *gb = new QGroupBox("Параметры записи");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "NFiltr", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "T_Data_Rec", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Постоянная времени фильтрации гармоник:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "NHarmFilt", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Запуск осциллограммы:"), row, 1, 1, 1);
    hlyout->addWidget(WDFunc::NewChB2(this, "oscchb.0", "по команде Ц"));
    //    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", Colors::ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB2(this, "oscchb.2", "по резкому изменению"));
    gridlyout->addLayout(hlyout, row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Количество точек осциллограммы на период основной частоты:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "OscPoints", 0, 10000, 0), row, 2, 1, 3);

    lyout->addLayout(gridlyout);
    gb->setLayout(lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Параметры двигателя");

    lyout = new QVBoxLayout;
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент передачи датчиков вибрации:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Kvibr", 0, 10000, 2), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Порядковый номер двигателя:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "NumA", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число пар полюсов (от 1 до 10):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Poles", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число пазов на статоре:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Stator_Slotes", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число стержней ротора:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Rotor_bars", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Тип вибродатчика:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibroType", 0, 10000, 0), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Подключенные датчики вибрации:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Sensors", 0, 10000, 0), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
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

// void ConfKDVDialog::SetupUI()
//{
//    QVBoxLayout *vlyout1 = new QVBoxLayout;
//    QVBoxLayout *vlyout2 = new QVBoxLayout;
//    ///
//    QGridLayout *gridlyout = new QGridLayout;
//    QScrollArea *Analog_area = new QScrollArea;
//    QScrollArea *area2 = new QScrollArea;
//    QScrollArea *scrArea = new QScrollArea;
//    QWidget *analog1 = new QWidget;
//    QWidget *analog2 = new QWidget;
//    QWidget *extraconf = new QWidget;
//    QWidget *MEKconf = new QWidget;
//    QWidget *Leftconf = new QWidget;
//    QWidget *time = new QWidget;
//    QWidget *link = new QWidget;

//    area2->setFrameShape(QFrame::NoFrame);
//    area2->setWidgetResizable(true);

//    Analog_area->setFrameShape(QFrame::NoFrame);
//    Analog_area->setWidgetResizable(true);

//    scrArea->setFrameShape(QFrame::NoFrame);
//    scrArea->setWidgetResizable(true);

//    int row = 0;
//    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

//    QLabel *lbl = new QLabel("Тип контролируемого оборудования:");
//    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
//    QStringList cbl = QStringList { "Асинхронный двигатель", "Трансформатор", "Реактор" };
//    auto *cb = WDFunc::NewCB2(this, "Eq_type", cbl);
//    cb->setMinimumHeight(20);
//    gridlyout->addWidget(cb, row, 2, 1, 3);
//    row++;

//    lbl = new QLabel("Вид охлаждения:");
//    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
//    cbl = QStringList { "Естественное", "Принудительное" };
//    cb = WDFunc::NewCB2(this, "Cool_type", cbl);
//    cb->setMinimumHeight(20);
//    gridlyout->addWidget(cb, row, 2, 1, 3);
//    row++;

//    lbl = new QLabel("Материал обмотки:");
//    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
//    cbl = QStringList { "Медь", "Алюминий" };
//    cb = WDFunc::NewCB2(this, "W_mat", cbl);
//    cb->setMinimumHeight(20);
//    gridlyout->addWidget(cb, row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Unom1", 0, 10000, 2), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "U2nom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный первичный ток, кА:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT1nom", 0, 10000, 0), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный вторичный ток, А:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT2nom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный ток контролируемой обмотки, À:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Iwnom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Tamb_nom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "dTNNTnom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент добавочных потерь:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Kdob", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "TauWnom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Максимальное измеряемое фазное напряжение на входе "
//                             "прибора, В эфф (не более 305В):"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Umax", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Максимальный измеряемый ток на входе "
//                             "прибора, А эфф  (не более 33А):"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imax", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Количество датчиков температуры обмоток, "
//                             "подключенных по Modbus Master:"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "TdatNum", 0, 10000, 0), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная частота, Гц"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Fnom", 0, 1000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное скольжение"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "nom_slip", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "UVmax", 0, 10000, 0), row, 2, 1, 3);
//    row++;

//    vlyout2->addLayout(gridlyout);
//    gb->setLayout(vlyout2);
//    vlyout1->addWidget(gb);

//    analog1->setLayout(vlyout1);
//    Analog_area->setWidget(analog1);
//    //.....................................................................
///
//    gb = new QGroupBox("Уставки сигнализации");

//    vlyout1 = new QVBoxLayout;
//    vlyout2 = new QVBoxLayout;
//    gridlyout = new QGridLayout;
//    row = 0;

//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Уставка скачка напряжения для запуска "
//                             "осциллографирования - % от номинала:"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "DUosc", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "DIosc", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Уставка порога минимального напряжения - % от номинального уровня:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "DUImin", 0, 1000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Уставка порога минимального тока - % от номинального уровня:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imin", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Предельно допустимая температура ННТ в°С:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "TNNTdop", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "TNNTpred", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Предупредительная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrA_pred", 0, 1000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Предупредительная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1,
//    1); gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrV_pred", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Предупредительная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrD_pred", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Аварийная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrA_alarm", 0, 1000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Аварийная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrV_alarm", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Аварийная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibrD_alarm", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Предупредительная уставка по скорости роста СКЗ "
//                             "виброускорения, м/с/с:"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrA_pred", 0, 1000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Предупредительная уставка по скорости роста СКЗ "
//                             "виброскорости, мм/с:"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrV_pred", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Предупредительная уставка по скорости роста СКЗ "
//                             "виброперемещения, мкм:"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrD_pred", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Аварийная уставка по скорости роста СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrA_alarm", 0, 1000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Аварийная уставка по скорости роста СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrV_alarm", 0, 10000, 1), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Аварийная уставка по скорости роста СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VVibrD_alarm", 0, 10000, 1), row, 2, 1, 3);

//    vlyout2->addLayout(gridlyout);
//    gb->setLayout(vlyout2);
//    vlyout1->addWidget(gb);

//    //.....................................................................
//    gb = new QGroupBox("Гистерезис");

//    vlyout2 = new QVBoxLayout;
//    gridlyout = new QGridLayout;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "GTnnt", 0, 10000, 3), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Гистерезис сигнализации по токовой перегрузке, % от Iwnom:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "GOvc", 0, 10000, 1), row, 2, 1, 3);

//    vlyout2->addLayout(gridlyout);
//    gb->setLayout(vlyout2);
//    vlyout1->addWidget(gb);

//    analog2->setLayout(vlyout1);
//    area2->setWidget(analog2);

//.....................................................................

//    gb = new QGroupBox("Параметры записи");

//    vlyout2 = new QVBoxLayout;
//    QHBoxLayout *hlyout = new QHBoxLayout;
//    gridlyout = new QGridLayout;
//    vlyout1 = new QVBoxLayout;

//    row = 0;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "NFiltr", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "T_Data_Rec", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Постоянная времени фильтрации гармоник:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "NHarmFilt", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Запуск осциллограммы:"), row, 1, 1, 1);
//    hlyout->addWidget(WDFunc::NewChB2(this, "oscchb.0", "по команде Ц"));
//    //    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", Colors::ACONFWCLR));
//    hlyout->addWidget(WDFunc::NewChB2(this, "oscchb.2", "по резкому изменению"));
//    gridlyout->addLayout(hlyout, row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Количество точек осциллограммы на период основной частоты:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "OscPoints", 0, 10000, 0), row, 2, 1, 3);

//    vlyout2->addLayout(gridlyout);
//    gb->setLayout(vlyout2);
//    vlyout1->addWidget(gb);

//    gb = new QGroupBox("Параметры двигателя");

//    vlyout2 = new QVBoxLayout;
//    gridlyout = new QGridLayout;

//    row = 0;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент передачи датчиков вибрации:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Kvibr", 0, 10000, 2), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Порядковый номер двигателя:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "NumA", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число пар полюсов (от 1 до 10):"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Poles", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число пазов на статоре:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Stator_Slotes", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Число стержней ротора:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Rotor_bars", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Тип вибродатчика:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "VibroType", 0, 10000, 0), row, 2, 1, 3);

//    row++;
//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Подключенные датчики вибрации:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Sensors", 0, 10000, 0), row, 2, 1, 3);

//    vlyout2->addLayout(gridlyout);
//    gb->setLayout(vlyout2);
//    vlyout1->addWidget(gb);

//    Leftconf->setLayout(vlyout1);
//.....................................................................

//    row = 0;
//    vlyout1 = new QVBoxLayout;
//    gb = new QGroupBox;
//    gridlyout = new QGridLayout;
//    vlyout2 = new QVBoxLayout;

//    gb->setTitle("Настройки протокола МЭК-60870-5-104");

//    //    gridlyout->addWidget(Conf->SetupMainBlk(this), 0, 0, 1, 1);
//    //    gridlyout->addWidget(ConfKxx->SetupComParam(this), 0, 1, 1, 1);

//    vlyout2->addLayout(gridlyout);
//    gb->setLayout(vlyout2);
//    vlyout1->addWidget(gb);

//    gb = new QGroupBox("Настройка времени");
//    vlyout2 = new QVBoxLayout;

//    //    vlyout2->addWidget(Conf->SetupTime(this));

//    gb->setLayout(vlyout2);
//    vlyout1->addWidget(gb);

//    link->setLayout(vlyout1);
//    scrArea->setWidget(link);

//    //....................................................................

//    QVBoxLayout *lyout = new QVBoxLayout;
//    QTabWidget *ConfTW = new QTabWidget;
//    ConfTW->setObjectName("conftw");

//    ConfTW->addTab(Analog_area, "Аналоговые");

//    ConfTW->addTab(area2, "Уставки");

//    ConfTW->addTab(scrArea, "Связь");
//    //    ConfTW->addTab(ConfKxx->SetupModBus(this), "ModBus");
//    //    ConfTW->addTab(ConfKxx->SetupBl(this), "Общее");

//    ConfTW->addTab(Leftconf, "Остальное");
//    lyout->addWidget(ConfTW);

//    QWidget *wdgt = ConfButtons();
//    lyout->addWidget(wdgt);
//    setLayout(lyout);
//}

void ConfKDVDialog::Fill()
{

    ConfKDV->MainConfig()->Fill();
    ConfKDV->KxxConfig()->Fill();
    FillKdv();
}

void ConfKDVDialog::FillBack()
{

    ConfKDV->MainConfig()->FillBack();
    ConfKDV->KxxConfig()->FillBack();
    FillBackKdv();
}

void ConfKDVDialog::FillKdv()
{
    //    Conf->Fill();

    int cbidx;

    //.........................................................
    cbidx = (ConfKDV->Bci_block.Eq_type & 0x02) ? 2 : ((ConfKDV->Bci_block.Eq_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Eq_type", cbidx);
    cbidx = ((ConfKDV->Bci_block.Cool_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    cbidx = ((ConfKDV->Bci_block.W_mat & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    WDFunc::SetChBData(this, "oscchb.0", (ConfKDV->Bci_block.DDosc & 0x0001));
    //    WDFunc::SetChBData(this, "oscchb.1", (KDV->Bci_block.DDosc & 0x0002));
    WDFunc::SetChBData(this, "oscchb.2", (ConfKDV->Bci_block.DDosc & 0x0004));
    WDFunc::SetSPBData(this, "NFiltr", ConfKDV->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "NHarmFilt", ConfKDV->Bci_block.NHarmFilt);

    // WDFunc::SetSPBData(this, "DDOsc", CKDV->Bci_block.DDOsc);
    WDFunc::SetSPBData(this, "Unom1", ConfKDV->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "U2nom", ConfKDV->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "ITT1nom", ConfKDV->Bci_block.ITT1nom);
    WDFunc::SetSPBData(this, "ITT2nom", ConfKDV->Bci_block.ITT2nom);
    WDFunc::SetSPBData(this, "Iwnom", ConfKDV->Bci_block.Iwnom);
    WDFunc::SetSPBData(this, "DUosc", ConfKDV->Bci_block.DUosc);
    WDFunc::SetSPBData(this, "DIosc", ConfKDV->Bci_block.DIosc);
    WDFunc::SetSPBData(this, "DUImin", ConfKDV->Bci_block.DUImin);
    WDFunc::SetSPBData(this, "Imin", ConfKDV->Bci_block.Imin);
    WDFunc::SetSPBData(this, "TNNTdop", ConfKDV->Bci_block.TNNTdop);
    WDFunc::SetSPBData(this, "TNNTpred", ConfKDV->Bci_block.TNNTpred);
    WDFunc::SetSPBData(this, "Tamb_nom", ConfKDV->Bci_block.Tamb_nom);
    WDFunc::SetSPBData(this, "dTNNTnom", ConfKDV->Bci_block.dTNNTnom);
    WDFunc::SetSPBData(this, "Kdob", ConfKDV->Bci_block.Kdob);
    WDFunc::SetSPBData(this, "TauWnom", ConfKDV->Bci_block.TauWnom);
    WDFunc::SetSPBData(this, "Umax", ConfKDV->Bci_block.Umax);
    WDFunc::SetSPBData(this, "Imax", ConfKDV->Bci_block.Imax);
    WDFunc::SetSPBData(this, "GTnnt", ConfKDV->Bci_block.GTnnt);
    WDFunc::SetSPBData(this, "GOvc", ConfKDV->Bci_block.GOvc);

    WDFunc::SetSPBData(this, "Fnom", ConfKDV->Bci_block.Fnom);
    WDFunc::SetSPBData(this, "nom_slip", ConfKDV->Bci_block.nom_slip);
    WDFunc::SetSPBData(this, "UVmax", ConfKDV->Bci_block.UVmax);
    WDFunc::SetSPBData(this, "Kvibr", ConfKDV->Bci_block.Kvibr);
    WDFunc::SetSPBData(this, "VibrA_pred", ConfKDV->Bci_block.VibrA_pred);
    WDFunc::SetSPBData(this, "VibrV_pred", ConfKDV->Bci_block.VibrV_pred);
    WDFunc::SetSPBData(this, "VibrD_pred", ConfKDV->Bci_block.VibrD_pred);
    WDFunc::SetSPBData(this, "VibrA_alarm", ConfKDV->Bci_block.VibrA_alarm);
    WDFunc::SetSPBData(this, "VibrV_alarm", ConfKDV->Bci_block.VibrV_alarm);
    WDFunc::SetSPBData(this, "VibrD_alarm", ConfKDV->Bci_block.VibrD_alarm);
    WDFunc::SetSPBData(this, "VVibrA_pred", ConfKDV->Bci_block.VVibrA_pred);
    WDFunc::SetSPBData(this, "VVibrV_pred", ConfKDV->Bci_block.VVibrV_pred);
    WDFunc::SetSPBData(this, "VVibrD_pred", ConfKDV->Bci_block.VVibrD_pred);
    WDFunc::SetSPBData(this, "VVibrA_alarm", ConfKDV->Bci_block.VVibrA_alarm);
    WDFunc::SetSPBData(this, "VVibrV_alarm", ConfKDV->Bci_block.VVibrV_alarm);
    WDFunc::SetSPBData(this, "VVibrD_alarm", ConfKDV->Bci_block.VVibrD_alarm);
    WDFunc::SetSPBData(this, "NumA", ConfKDV->Bci_block.NumA);
    WDFunc::SetSPBData(this, "Poles", ConfKDV->Bci_block.Poles);
    WDFunc::SetSPBData(this, "Stator_Slotes", ConfKDV->Bci_block.Stator_Slotes);
    WDFunc::SetSPBData(this, "Rotor_bars", ConfKDV->Bci_block.Rotor_bars);
    WDFunc::SetSPBData(this, "VibroType", ConfKDV->Bci_block.VibroType);
    WDFunc::SetSPBData(this, "Sensors", ConfKDV->Bci_block.Sensors);

    WDFunc::SetSPBData(this, "T_Data_Rec", ConfKDV->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "OscPoints", ConfKDV->Bci_block.OscPoints);
    WDFunc::SetSPBData(this, "TdatNum", ConfKDV->Bci_block.TdatNum);

    //.........................................................
    //    ConfKxx->Fill();
}

void ConfKDVDialog::FillBackKdv()
{
    // Conf->FillBack();

    int cbidx;
    bool tmpb;

    //.........................................................

    cbidx = WDFunc::CBIndex(this, "Eq_type");
    ConfKDV->Bci_block.Eq_type = cbidx;
    cbidx = WDFunc::CBIndex(this, "Cool_type");
    ConfKDV->Bci_block.Cool_type = cbidx;
    cbidx = WDFunc::CBIndex(this, "W_mat");
    ConfKDV->Bci_block.W_mat = cbidx;
    WDFunc::SPBData(this, "NFiltr", ConfKDV->Bci_block.NFiltr);
    WDFunc::SPBData(this, "NHarmFilt", ConfKDV->Bci_block.NHarmFilt);
    // WDFunc::SPBData(this, "DDOsc", CKDV->Bci_block.DDOsc);

    WDFunc::ChBData(this, "oscchb.0", tmpb);
    ConfKDV->Bci_block.DDosc = 0;
    ConfKDV->Bci_block.DDosc |= (tmpb) ? 0x0001 : 0x0000;
    //    WDFunc::ChBData(this, "oscchb.1", tmpb);
    //    KDV->Bci_block.DDosc |= (tmpb) ? 0x0002 : 0x0000;
    WDFunc::ChBData(this, "oscchb.2", tmpb);
    ConfKDV->Bci_block.DDosc |= (tmpb) ? 0x0004 : 0x0000;

    WDFunc::SPBData(this, "Unom1", ConfKDV->Bci_block.Unom1);
    WDFunc::SPBData(this, "U2nom", ConfKDV->Bci_block.U2nom);
    WDFunc::SPBData(this, "ITT1nom", ConfKDV->Bci_block.ITT1nom);
    WDFunc::SPBData(this, "ITT2nom", ConfKDV->Bci_block.ITT2nom);
    WDFunc::SPBData(this, "Iwnom", ConfKDV->Bci_block.Iwnom);
    WDFunc::SPBData(this, "DUosc", ConfKDV->Bci_block.DUosc);
    WDFunc::SPBData(this, "DIosc", ConfKDV->Bci_block.DIosc);
    WDFunc::SPBData(this, "DUImin", ConfKDV->Bci_block.DUImin);
    WDFunc::SPBData(this, "Imin", ConfKDV->Bci_block.Imin);
    WDFunc::SPBData(this, "TNNTdop", ConfKDV->Bci_block.TNNTdop);
    WDFunc::SPBData(this, "TNNTpred", ConfKDV->Bci_block.TNNTpred);
    WDFunc::SPBData(this, "Tamb_nom", ConfKDV->Bci_block.Tamb_nom);
    WDFunc::SPBData(this, "dTNNTnom", ConfKDV->Bci_block.dTNNTnom);
    WDFunc::SPBData(this, "Kdob", ConfKDV->Bci_block.Kdob);
    WDFunc::SPBData(this, "TauWnom", ConfKDV->Bci_block.TauWnom);
    WDFunc::SPBData(this, "Umax", ConfKDV->Bci_block.Umax);
    WDFunc::SPBData(this, "Imax", ConfKDV->Bci_block.Imax);
    WDFunc::SPBData(this, "GTnnt", ConfKDV->Bci_block.GTnnt);
    WDFunc::SPBData(this, "GOvc", ConfKDV->Bci_block.GOvc);
    WDFunc::SPBData(this, "Fnom", ConfKDV->Bci_block.Fnom);
    WDFunc::SPBData(this, "nom_slip", ConfKDV->Bci_block.nom_slip);
    WDFunc::SPBData(this, "UVmax", ConfKDV->Bci_block.UVmax);
    WDFunc::SPBData(this, "Kvibr", ConfKDV->Bci_block.Kvibr);
    WDFunc::SPBData(this, "VibrA_pred", ConfKDV->Bci_block.VibrA_pred);
    WDFunc::SPBData(this, "VibrV_pred", ConfKDV->Bci_block.VibrV_pred);
    WDFunc::SPBData(this, "VibrD_pred", ConfKDV->Bci_block.VibrD_pred);
    WDFunc::SPBData(this, "VibrA_alarm", ConfKDV->Bci_block.VibrA_alarm);
    WDFunc::SPBData(this, "VibrV_alarm", ConfKDV->Bci_block.VibrV_alarm);
    WDFunc::SPBData(this, "VibrD_alarm", ConfKDV->Bci_block.VibrD_alarm);
    WDFunc::SPBData(this, "VVibrA_pred", ConfKDV->Bci_block.VVibrA_pred);
    WDFunc::SPBData(this, "VVibrV_pred", ConfKDV->Bci_block.VVibrV_pred);
    WDFunc::SPBData(this, "VVibrD_pred", ConfKDV->Bci_block.VVibrD_pred);
    WDFunc::SPBData(this, "VVibrA_alarm", ConfKDV->Bci_block.VVibrA_alarm);
    WDFunc::SPBData(this, "VVibrV_alarm", ConfKDV->Bci_block.VVibrV_alarm);
    WDFunc::SPBData(this, "VVibrD_alarm", ConfKDV->Bci_block.VVibrD_alarm);
    WDFunc::SPBData(this, "NumA", ConfKDV->Bci_block.NumA);
    WDFunc::SPBData(this, "Poles", ConfKDV->Bci_block.Poles);
    WDFunc::SPBData(this, "Stator_Slotes", ConfKDV->Bci_block.Stator_Slotes);
    WDFunc::SPBData(this, "Rotor_bars", ConfKDV->Bci_block.Rotor_bars);
    WDFunc::SPBData(this, "VibroType", ConfKDV->Bci_block.VibroType);
    WDFunc::SPBData(this, "Sensors", ConfKDV->Bci_block.Sensors);
    WDFunc::SPBData(this, "T_Data_Rec", ConfKDV->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "OscPoints", ConfKDV->Bci_block.OscPoints);
    WDFunc::SPBData(this, "TdatNum", ConfKDV->Bci_block.TdatNum);
    //.........................................................
    //    ConfKxx->FillBack();
}

void ConfKDVDialog::CheckConf()
{
}

// QWidget *ConfKDVDialog::analogWidget()
//{
//    QWidget *widget = new QWidget;
//    QVBoxLayout *lyout = new QVBoxLayout;
//    QGridLayout *gridlyout = new QGridLayout;
//    QScrollArea *scrollArea = new QScrollArea;

//    scrollArea->setFrameShape(QFrame::NoFrame);
//    scrollArea->setWidgetResizable(true);

//    QFont font;

//    int row = 0;
//    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

//    QLabel *lbl = new QLabel("Тип контролируемого оборудования:");
//    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
//    QStringList cbl = QStringList { "Асинхронный двигатель", "Трансформатор", "Реактор" };
//    auto *cb = WDFunc::NewCB2(this, "Eq_type", cbl);
//    cb->setMinimumHeight(20);
//    gridlyout->addWidget(cb, row, 2, 1, 3);
//    row++;

//    lbl = new QLabel("Вид охлаждения:");
//    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
//    cbl = QStringList { "Естественное", "Принудительное" };
//    cb = WDFunc::NewCB2(this, "Cool_type", cbl);
//    cb->setMinimumHeight(20);
//    gridlyout->addWidget(cb, row, 2, 1, 3);
//    row++;

//    lbl = new QLabel("Материал обмотки:");
//    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
//    cbl = QStringList { "Медь", "Алюминий" };
//    cb = WDFunc::NewCB2(this, "W_mat", cbl);
//    cb->setMinimumHeight(20);
//    gridlyout->addWidget(cb, row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Unom1", 0, 10000, 2), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "U2nom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный первичный ток, кА:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT1nom", 0, 10000, 0), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный вторичный ток, А:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT2nom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный ток контролируемой обмотки, À:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Iwnom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Tamb_nom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "dTNNTnom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент добавочных потерь:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Kdob", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "TauWnom", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Максимальное измеряемое фазное напряжение на входе "
//                             "прибора, В эфф (не более 305В):"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Umax", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Максимальный измеряемый ток на входе "
//                             "прибора, А эфф  (не более 33А):"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imax", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this,
//                             "Количество датчиков температуры обмоток, "
//                             "подключенных по Modbus Master:"),
//        row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "TdatNum", 0, 10000, 0), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная частота, Гц"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "Fnom", 0, 1000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное скольжение"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "nom_slip", 0, 10000, 1), row, 2, 1, 3);
//    row++;

//    gridlyout->addWidget(
//        WDFunc::NewLBL2(this, "Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)"), row, 1, 1, 1);
//    gridlyout->addWidget(WDFunc::NewSPB2(this, "UVmax", 0, 10000, 0), row, 2, 1, 3);
//    row++;

//    gb->setLayout(gridlyout);

//    scrollArea->setWidget(widget);
//    lyout->addWidget(gb);

//    widget->setLayout(lyout);
//    return widget;
//}

// void ConfKDVDialog::setConnections()
//{
//    //    connect(&DataManager, &DataManager::floatReceived)
//}

void ConfKDVDialog::SetDefConf()
{
    ConfKDV->SetDefConf();
    Fill();
}
