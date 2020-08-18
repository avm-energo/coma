#include "confdialogkdv.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../widgets/ecombobox.h"
#include "../widgets/emessagebox.h"
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

ConfDialogKDV::ConfDialogKDV(QVector<S2::DataRec> *S2Config, QWidget *parent) : AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: " + QString(ACONFCLR) + ";}";
    setStyleSheet(tmps);
    this->S2Config = S2Config;
    KDV = new ConfigKDV(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    PrereadConf();
}

void ConfDialogKDV::SetupUI()
{
    QString phase[3] = { "Фаза A:", "Фаза B:", "Фаза C:" };

    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QScrollArea *Analog_area = new QScrollArea;
    QScrollArea *area = new QScrollArea;
    QScrollArea *area2 = new QScrollArea;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *MEKconf = new QWidget;
    QWidget *Leftconf = new QWidget;
    QWidget *time = new QWidget;
    QString tmps = "QWidget {background-color: " + QString(ACONFWCLR) + ";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
    extraconf->setStyleSheet(tmps);
    time->setStyleSheet(tmps);
    MEKconf->setStyleSheet(tmps);
    Leftconf->setStyleSheet(tmps);

    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    Analog_area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    Analog_area->setFrameShape(QFrame::NoFrame);
    Analog_area->setWidgetResizable(true);

    QString paramcolor = MAINWINCLR;
    QFont font;

    QGroupBox *gb = new QGroupBox;

    int row = 0;
    gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    // setFont(font);
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    QLabel *lbl = new QLabel("Тип контролируемого оборудования:");
    glyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList() << "Асинхронный двигатель"
                                    << "Трансформатор"
                                    << "Реактор";
    EComboBox *cb = WDFunc::NewCB(this, "Eq_type", cbl, paramcolor);
    cb->setMinimumHeight(20);
    glyout->addWidget(cb, row, 2, 1, 3);
    row++;

    lbl = new QLabel("Вид охлаждения:");
    glyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    cbl = QStringList() << "Естественное"
                        << "Принудительное";
    cb = WDFunc::NewCB(this, "Cool_type", cbl, paramcolor);
    cb->setMinimumHeight(20);
    glyout->addWidget(cb, row, 2, 1, 3);
    row++;

    lbl = new QLabel("Материал обмотки:");
    glyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    cbl = QStringList() << "Медь"
                        << "Алюминий";
    cb = WDFunc::NewCB(this, "W_mat", cbl, paramcolor);
    cb->setMinimumHeight(20);
    glyout->addWidget(cb, row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom1", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "U2nom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный первичный ток, кА:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "ITT1nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный вторичный ток, А:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "ITT2nom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток контролируемой обмотки, À:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Iwnom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Tamb_nom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "dTNNTnom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент добавочных потерь:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Kdob", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "TauWnom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Максимальное измеряемое фазное напряжение на входе прибора, В эфф (не более 305В):"), row,
        1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Umax", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Максимальный измеряемый ток на входе прибора, А эфф  (не более 33А):"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imax", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Количество датчиков температуры обмоток, подключенных по Modbus Master:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "TdatNum", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная частота, Гц"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Fnom", 0, 1000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное скольжение"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "nom_slip", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "UVmax", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    analog1->setLayout(vlyout1);
    Analog_area->setWidget(analog1);
    //.....................................................................

    gb = new QGroupBox("Уставки сигнализации");
    gb->setFont(font);
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Уставка скачка напряжения для запуска осциллографирования - % от номинала:"), row, 1, 1,
        1);
    glyout->addWidget(WDFunc::NewSPB(this, "DUosc", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "DIosc", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Уставка порога минимального напряжения - % от номинального уровня:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "DUImin", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Уставка порога минимального тока - % от номинального уровня:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Предельно допустимая температура ННТ в°С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "TNNTdop", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "TNNTpred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Предупредительная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VibrA_pred", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Предупредительная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VibrV_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Предупредительная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VibrD_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Аварийная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VibrA_alarm", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Аварийная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VibrV_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Аварийная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VibrD_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Предупредительная уставка по скорости роста СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VVibrA_pred", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Предупредительная уставка по скорости роста СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VVibrV_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Предупредительная уставка по скорости роста СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VVibrD_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Аварийная уставка по скорости роста СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VVibrA_alarm", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Аварийная уставка по скорости роста СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VVibrV_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(
        WDFunc::NewLBL(this, "Аварийная уставка по скорости роста СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VVibrD_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    //.....................................................................
    gb = new QGroupBox("Гистерезис");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "GTnnt", 0, 10000, 3, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по токовой перегрузке, % от Iwnom:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "GOvc", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    analog2->setLayout(vlyout1);
    area2->setWidget(analog2);

    //.....................................................................
    row = 0;
    gb = new QGroupBox("Конфигурация 104");
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    gb->setFont(font);
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl, 0, 0, 1, 1, Qt::AlignLeft);
    QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "Abs_104", 0, 65535, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса, с:");
    glyout->addWidget(lbl, 1, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "Cycle_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    // lbl=new QLabel("c");
    // glyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1, с:");
    glyout->addWidget(lbl, 2, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T1_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    // lbl=new QLabel("c");
    // glyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2, с:");
    glyout->addWidget(lbl, 3, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T2_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    // lbl=new QLabel("c");
    // glyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3, с:");
    glyout->addWidget(lbl, 4, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T3_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    // lbl=new QLabel("c");
    // glyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl, 5, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "k_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    // lbl=new QLabel("c");
    // glyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl, 6, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "w_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    row++;

    row += 7;

    QString Str;
    QLocale german(QLocale::German);
    int i;
    for (i = 0; i < 4; i++)
    {
        KDV->Com_param.IP[i] = 0;
        if (i == 3)
            Str.append(german.toString(KDV->Com_param.IP[i]));
        else
            Str.append(german.toString(KDV->Com_param.IP[i]) + ".");
    }

    glyout->addWidget(WDFunc::NewLBL(this, "IP адрес устройства:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "IP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        KDV->Com_param.Mask[i] = 0;
        if (i == 3)
            Str.append(german.toString(KDV->Com_param.Mask[i]));
        else
            Str.append(german.toString(KDV->Com_param.Mask[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Маска:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "Mask_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        KDV->Com_param.GW[i] = 0;
        if (i == 3)
            Str.append(german.toString(KDV->Com_param.GW[i]));
        else
            Str.append(german.toString(KDV->Com_param.GW[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Шлюз:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "GW_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Порт протокола 104:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Port_ID", 0, 10000, 0, paramcolor), row, 1, 1, 1);

    row++;
    glyout->addWidget(WDFunc::NewChB(this, "ISNTP_ID", "Использование SNTP сервера"), row, 0, 1, 1);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        KDV->Com_param.SNTP[i] = 0;
        if (i == 3)
            Str.append(german.toString(KDV->Com_param.SNTP[i]));
        else
            Str.append(german.toString(KDV->Com_param.SNTP[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Адрес SNTP сервера:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "SNTP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ?????????

    gb = new QGroupBox("Настройка времени");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    row++;
    lbl = new QLabel("Тип синхронизации времени:");
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    cbl = QStringList() << "SNTP+PPS"
                        << "SNTP";
    cb = WDFunc::NewCB(this, "spb.8", cbl, paramcolor);
    // cb->setMinimumWidth(70);
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(cb, row, 1, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    gb = new QGroupBox("Настройки ModBus");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость RS485 интерфейса:"), row, 0, 1, 1);
    cbl = QStringList() << "1200"
                        << "2400"
                        << "4800"
                        << "9600"
                        << "19200"
                        << "38400"
                        << "57600"
                        << "115200";
    cb = WDFunc::NewCB(this, "Baud", cbl, paramcolor);
    // cb->setMinimumWidth(80);
    // cb->setMinimumHeight(15);
    glyout->addWidget(cb, row, 1, 1, 1);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Чётность:"), row, 0, 1, 1);
    cbl = QStringList() << "NoParity"
                        << "EvenParity"
                        << "OddParity";
    cb = WDFunc::NewCB(this, "Parity", cbl, paramcolor);
    // cb->setMinimumWidth(80);
    // cb->setMinimumHeight(15);
    glyout->addWidget(cb, row, 1, 1, 1);
    // glyout->addWidget(WDFunc::NewSPB(this, "Parity", 0, 10000, 0, paramcolor), row,2,1,4);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Количество стоповых битов:"), row, 0, 1, 1);
    cbl = QStringList() << "Stop_Bit_1"
                        << "Stop_Bit_2";
    cb = WDFunc::NewCB(this, "StopBit", cbl, paramcolor);
    // cb->setMinimumWidth(80);
    // cb->setMinimumHeight(20);
    glyout->addWidget(cb, row, 1, 1, 1);
    // glyout->addWidget(WDFunc::NewSPB(this, "StopBit", 0, 10000, 0, paramcolor), row,2,1,4);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Адрес устройства для Modbus:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "adrMB", 0, 10000, 0, paramcolor), row, 1, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    extraconf->setLayout(vlyout1);
    area->setWidget(extraconf);

    //.....................................................................

    gb = new QGroupBox("Параметры записи");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;

    row = 0;
    glyout->addWidget(WDFunc::NewLBL(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "NFiltr", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени фильтрации гармоник:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "NHarmFilt", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Запуск осциллограммы:"), row, 1, 1, 1);
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", ACONFWCLR));
    glyout->addLayout(hlyout, row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Количество точек осциллограммы на период основной частоты:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "OscPoints", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Параметры двигателя");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row = 0;
    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент передачи датчиков вибрации:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Kvibr", 0, 10000, 2, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Порядковый номер двигателя:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "NumA", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Число пар полюсов (от 1 до 10):"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Poles", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Число пазов на статоре:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Stator_Slotes", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Число стержней ротора:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Rotor_bars", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Тип вибродатчика:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "VibroType", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Подключенные датчики вибрации:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Sensors", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    Leftconf->setLayout(vlyout1);
    //.....................................................................

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(TABCOLOR) + ";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(Analog_area, "Аналоговые");
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());

    ConfTW->addTab(area2, "Уставки");
    area2->verticalScrollBar()->setValue(area2->verticalScrollBar()->maximum());
    // area2->setSizeIncrement(QSize(1000,1000));
    ConfTW->addTab(area, "Связь");
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());

    ConfTW->addTab(Leftconf, "Остальное");
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialogKDV::Fill()
{
    int i;
    WDFunc::SetSPBData(this, "Abs_104", KDV->MainBlk.Abs_104);
    WDFunc::SetSPBData(this, "Cycle_104", KDV->MainBlk.Cycle_104);
    WDFunc::SetSPBData(this, "T1_104", KDV->MainBlk.T1_104);
    WDFunc::SetSPBData(this, "T2_104", KDV->MainBlk.T2_104);
    WDFunc::SetSPBData(this, "T3_104", KDV->MainBlk.T3_104);
    WDFunc::SetSPBData(this, "k_104", KDV->MainBlk.k_104);
    WDFunc::SetSPBData(this, "w_104", KDV->MainBlk.w_104);

    int cbidx = ((KDV->MainBlk.Ctype & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Ctype", cbidx);
    //.........................................................
    cbidx = (KDV->Bci_block.Eq_type & 0x02) ? 2 : ((KDV->Bci_block.Eq_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Eq_type", cbidx);
    cbidx = ((KDV->Bci_block.Cool_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    cbidx = ((KDV->Bci_block.W_mat & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    WDFunc::SetChBData(this, "oscchb.0", (KDV->Bci_block.DDosc & 0x0001));
    WDFunc::SetChBData(this, "oscchb.1", (KDV->Bci_block.DDosc & 0x0002));
    WDFunc::SetChBData(this, "oscchb.2", (KDV->Bci_block.DDosc & 0x0004));
    WDFunc::SetSPBData(this, "NFiltr", KDV->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "NHarmFilt", KDV->Bci_block.NHarmFilt);

    // WDFunc::SetSPBData(this, "DDOsc", CKDV->Bci_block.DDOsc);
    WDFunc::SetSPBData(this, "Unom1", KDV->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "U2nom", KDV->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "ITT1nom", KDV->Bci_block.ITT1nom);
    WDFunc::SetSPBData(this, "ITT2nom", KDV->Bci_block.ITT2nom);
    WDFunc::SetSPBData(this, "Iwnom", KDV->Bci_block.Iwnom);
    WDFunc::SetSPBData(this, "DUosc", KDV->Bci_block.DUosc);
    WDFunc::SetSPBData(this, "DIosc", KDV->Bci_block.DIosc);
    WDFunc::SetSPBData(this, "DUImin", KDV->Bci_block.DUImin);
    WDFunc::SetSPBData(this, "Imin", KDV->Bci_block.Imin);
    WDFunc::SetSPBData(this, "TNNTdop", KDV->Bci_block.TNNTdop);
    WDFunc::SetSPBData(this, "TNNTpred", KDV->Bci_block.TNNTpred);
    WDFunc::SetSPBData(this, "Tamb_nom", KDV->Bci_block.Tamb_nom);
    WDFunc::SetSPBData(this, "dTNNTnom", KDV->Bci_block.dTNNTnom);
    WDFunc::SetSPBData(this, "Kdob", KDV->Bci_block.Kdob);
    WDFunc::SetSPBData(this, "TauWnom", KDV->Bci_block.TauWnom);
    WDFunc::SetSPBData(this, "Umax", KDV->Bci_block.Umax);
    WDFunc::SetSPBData(this, "Imax", KDV->Bci_block.Imax);
    WDFunc::SetSPBData(this, "GTnnt", KDV->Bci_block.GTnnt);
    WDFunc::SetSPBData(this, "GOvc", KDV->Bci_block.GOvc);

    WDFunc::SetSPBData(this, "Fnom", KDV->Bci_block.Fnom);
    WDFunc::SetSPBData(this, "nom_slip", KDV->Bci_block.nom_slip);
    WDFunc::SetSPBData(this, "UVmax", KDV->Bci_block.UVmax);
    WDFunc::SetSPBData(this, "Kvibr", KDV->Bci_block.Kvibr);
    WDFunc::SetSPBData(this, "VibrA_pred", KDV->Bci_block.VibrA_pred);
    WDFunc::SetSPBData(this, "VibrV_pred", KDV->Bci_block.VibrV_pred);
    WDFunc::SetSPBData(this, "VibrD_pred", KDV->Bci_block.VibrD_pred);
    WDFunc::SetSPBData(this, "VibrA_alarm", KDV->Bci_block.VibrA_alarm);
    WDFunc::SetSPBData(this, "VibrV_alarm", KDV->Bci_block.VibrV_alarm);
    WDFunc::SetSPBData(this, "VibrD_alarm", KDV->Bci_block.VibrD_alarm);
    WDFunc::SetSPBData(this, "VVibrA_pred", KDV->Bci_block.VVibrA_pred);
    WDFunc::SetSPBData(this, "VVibrV_pred", KDV->Bci_block.VVibrV_pred);
    WDFunc::SetSPBData(this, "VVibrD_pred", KDV->Bci_block.VVibrD_pred);
    WDFunc::SetSPBData(this, "VVibrA_alarm", KDV->Bci_block.VVibrA_alarm);
    WDFunc::SetSPBData(this, "VVibrV_alarm", KDV->Bci_block.VVibrV_alarm);
    WDFunc::SetSPBData(this, "VVibrD_alarm", KDV->Bci_block.VVibrD_alarm);
    WDFunc::SetSPBData(this, "NumA", KDV->Bci_block.NumA);
    WDFunc::SetSPBData(this, "Poles", KDV->Bci_block.Poles);
    WDFunc::SetSPBData(this, "Stator_Slotes", KDV->Bci_block.Stator_Slotes);
    WDFunc::SetSPBData(this, "Rotor_bars", KDV->Bci_block.Rotor_bars);
    WDFunc::SetSPBData(this, "VibroType", KDV->Bci_block.VibroType);
    WDFunc::SetSPBData(this, "Sensors", KDV->Bci_block.Sensors);

    WDFunc::SetSPBData(this, "T_Data_Rec", KDV->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "OscPoints", KDV->Bci_block.OscPoints);
    WDFunc::SetSPBData(this, "TdatNum", KDV->Bci_block.TdatNum);

    //.........................................................

    QString StrIP, StrMask, StrSNTP, StrGate;
    QLocale german(QLocale::German);

    for (i = 0; i < 4; i++)
    {

        if (i == 3)
        {
            StrIP.append(german.toString(KDV->Com_param.IP[i]));
            StrMask.append(german.toString(KDV->Com_param.Mask[i]));
            StrGate.append(german.toString(KDV->Com_param.GW[i]));
            StrSNTP.append(german.toString(KDV->Com_param.SNTP[i]));
        }
        else
        {
            StrIP.append(german.toString(KDV->Com_param.IP[i]) + ".");
            StrMask.append(german.toString(KDV->Com_param.Mask[i]) + ".");
            StrGate.append(german.toString(KDV->Com_param.GW[i]) + ".");
            StrSNTP.append(german.toString(KDV->Com_param.SNTP[i]) + ".");
        }
    }

    WDFunc::SetSPBData(this, "Port_ID", KDV->Com_param.Port[0]);

    WDFunc::LE_write_data(this, StrIP, "IP_ID");
    WDFunc::LE_write_data(this, StrSNTP, "SNTP_ID");
    WDFunc::LE_write_data(this, StrGate, "GW_ID");
    WDFunc::LE_write_data(this, StrMask, "Mask_ID");

    for (int i = 0; i < 8; i++)
    {
        if (KDV->Com_param.Baud == Sbaud.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(this, "Baud_ID", cbidx);

    if (KDV->Com_param.Parity > 2)
        cbidx = 0;
    else
        cbidx = KDV->Com_param.Parity;

    WDFunc::SetCBIndex(this, "Parity_ID", cbidx);
    cbidx = ((KDV->Com_param.Stopbit & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Stopbit_ID", cbidx);

    WDFunc::SetSPBData(this, "adrMB_ID", KDV->Com_param.adrMB);

    if (KDV->Com_param.IsNTP)
        WDFunc::SetChBData(this, "ISNTP_ID", true);
    else
        WDFunc::SetChBData(this, "ISNTP_ID", false);
}

void ConfDialogKDV::FillBack()
{
    int i;
    int cbidx;
    QString tmps;
    bool tmpb;

    WDFunc::SPBData(this, "Abs_104", KDV->MainBlk.Abs_104);
    WDFunc::SPBData(this, "Cycle_104", KDV->MainBlk.Cycle_104);
    WDFunc::SPBData(this, "T1_104", KDV->MainBlk.T1_104);
    WDFunc::SPBData(this, "T2_104", KDV->MainBlk.T2_104);
    WDFunc::SPBData(this, "T3_104", KDV->MainBlk.T3_104);
    WDFunc::SPBData(this, "k_104", KDV->MainBlk.k_104);
    WDFunc::SPBData(this, "w_104", KDV->MainBlk.w_104);

    cbidx = ((KDV->MainBlk.Ctype & 0x01) ? 1 : 0);
    KDV->MainBlk.Ctype = (0x00000001 << cbidx) - 1;
    //.........................................................

    cbidx = WDFunc::CBIndex(this, "Eq_type");
    KDV->Bci_block.Eq_type = cbidx;
    cbidx = WDFunc::CBIndex(this, "Cool_type");
    KDV->Bci_block.Cool_type = cbidx;
    cbidx = WDFunc::CBIndex(this, "W_mat");
    KDV->Bci_block.W_mat = cbidx;
    WDFunc::SPBData(this, "NFiltr", KDV->Bci_block.NFiltr);
    WDFunc::SPBData(this, "NHarmFilt", KDV->Bci_block.NHarmFilt);
    // WDFunc::SPBData(this, "DDOsc", CKDV->Bci_block.DDOsc);

    WDFunc::ChBData(this, "oscchb.0", tmpb);
    KDV->Bci_block.DDosc |= (tmpb) ? 0x0001 : 0x0000;
    WDFunc::ChBData(this, "oscchb.1", tmpb);
    KDV->Bci_block.DDosc |= (tmpb) ? 0x0002 : 0x0000;
    WDFunc::ChBData(this, "oscchb.2", tmpb);
    KDV->Bci_block.DDosc |= (tmpb) ? 0x0004 : 0x0000;

    WDFunc::SPBData(this, "Unom1", KDV->Bci_block.Unom1);
    WDFunc::SPBData(this, "U2nom", KDV->Bci_block.U2nom);
    WDFunc::SPBData(this, "ITT1nom", KDV->Bci_block.ITT1nom);
    WDFunc::SPBData(this, "ITT2nom", KDV->Bci_block.ITT2nom);
    WDFunc::SPBData(this, "Iwnom", KDV->Bci_block.Iwnom);
    WDFunc::SPBData(this, "DUosc", KDV->Bci_block.DUosc);
    WDFunc::SPBData(this, "DIosc", KDV->Bci_block.DIosc);
    WDFunc::SPBData(this, "DUImin", KDV->Bci_block.DUImin);
    WDFunc::SPBData(this, "Imin", KDV->Bci_block.Imin);
    WDFunc::SPBData(this, "TNNTdop", KDV->Bci_block.TNNTdop);
    WDFunc::SPBData(this, "TNNTpred", KDV->Bci_block.TNNTpred);
    WDFunc::SPBData(this, "Tamb_nom", KDV->Bci_block.Tamb_nom);
    WDFunc::SPBData(this, "dTNNTnom", KDV->Bci_block.dTNNTnom);
    WDFunc::SPBData(this, "Kdob", KDV->Bci_block.Kdob);
    WDFunc::SPBData(this, "TauWnom", KDV->Bci_block.TauWnom);
    WDFunc::SPBData(this, "Umax", KDV->Bci_block.Umax);
    WDFunc::SPBData(this, "Imax", KDV->Bci_block.Imax);
    WDFunc::SPBData(this, "GTnnt", KDV->Bci_block.GTnnt);
    WDFunc::SPBData(this, "GOvc", KDV->Bci_block.GOvc);
    WDFunc::SPBData(this, "Fnom", KDV->Bci_block.Fnom);
    WDFunc::SPBData(this, "nom_slip", KDV->Bci_block.nom_slip);
    WDFunc::SPBData(this, "UVmax", KDV->Bci_block.UVmax);
    WDFunc::SPBData(this, "Kvibr", KDV->Bci_block.Kvibr);
    WDFunc::SPBData(this, "VibrA_pred", KDV->Bci_block.VibrA_pred);
    WDFunc::SPBData(this, "VibrV_pred", KDV->Bci_block.VibrV_pred);
    WDFunc::SPBData(this, "VibrD_pred", KDV->Bci_block.VibrD_pred);
    WDFunc::SPBData(this, "VibrA_alarm", KDV->Bci_block.VibrA_alarm);
    WDFunc::SPBData(this, "VibrV_alarm", KDV->Bci_block.VibrV_alarm);
    WDFunc::SPBData(this, "VibrD_alarm", KDV->Bci_block.VibrD_alarm);
    WDFunc::SPBData(this, "VVibrA_pred", KDV->Bci_block.VVibrA_pred);
    WDFunc::SPBData(this, "VVibrV_pred", KDV->Bci_block.VVibrV_pred);
    WDFunc::SPBData(this, "VVibrD_pred", KDV->Bci_block.VVibrD_pred);
    WDFunc::SPBData(this, "VVibrA_alarm", KDV->Bci_block.VVibrA_alarm);
    WDFunc::SPBData(this, "VVibrV_alarm", KDV->Bci_block.VVibrV_alarm);
    WDFunc::SPBData(this, "VVibrD_alarm", KDV->Bci_block.VVibrD_alarm);
    WDFunc::SPBData(this, "NumA", KDV->Bci_block.NumA);
    WDFunc::SPBData(this, "Poles", KDV->Bci_block.Poles);
    WDFunc::SPBData(this, "Stator_Slotes", KDV->Bci_block.Stator_Slotes);
    WDFunc::SPBData(this, "Rotor_bars", KDV->Bci_block.Rotor_bars);
    WDFunc::SPBData(this, "VibroType", KDV->Bci_block.VibroType);
    WDFunc::SPBData(this, "Sensors", KDV->Bci_block.Sensors);
    WDFunc::SPBData(this, "T_Data_Rec", KDV->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "OscPoints", KDV->Bci_block.OscPoints);
    WDFunc::SPBData(this, "TdatNum", KDV->Bci_block.TdatNum);
    //.........................................................

    QString StrIP, StrMask, StrSNTP, StrGate;
    QString NameIP = "IP_ID", NameMask = "Mask_ID", NameSNTP = "SNTP_ID", NameGate = "GW_ID";
    QStringList inIP, inMask, inSNTP, inGate;
    QLocale german(QLocale::German);

    WDFunc::LE_read_data(this, NameIP, StrIP);
    WDFunc::LE_read_data(this, NameSNTP, StrSNTP);
    WDFunc::LE_read_data(this, NameGate, StrGate);
    WDFunc::LE_read_data(this, NameMask, StrMask);

    inIP.append(StrIP.split("."));
    inMask.append(StrMask.split("."));
    inSNTP.append(StrSNTP.split("."));
    inGate.append(StrGate.split("."));

    for (i = 0; i < 4; i++)
    {
        KDV->Com_param.IP[i] = inIP.at(i).toInt();
        KDV->Com_param.Mask[i] = inMask.at(i).toInt();
        KDV->Com_param.GW[i] = inGate.at(i).toInt();
        KDV->Com_param.SNTP[i] = inSNTP.at(i).toInt();
    }

    WDFunc::SPBData(this, "Port_ID" + QString::number(0), KDV->Com_param.Port[0]);

    cbidx = WDFunc::CBIndex(this, "Baud_ID");
    KDV->Com_param.Baud = (Sbaud.at(cbidx).toInt());
    cbidx = WDFunc::CBIndex(this, "Parity_ID");
    KDV->Com_param.Parity = cbidx;
    cbidx = WDFunc::CBIndex(this, "Stopbit_ID");
    KDV->Com_param.Stopbit = cbidx;

    WDFunc::SPBData(this, "adrMB_ID", KDV->Com_param.adrMB);

    WDFunc::ChBData(this, "ISNTP_ID", IsNtp);
    if (IsNtp)
        KDV->Com_param.IsNTP = 1;
    else
        KDV->Com_param.IsNTP = 0;
}

void ConfDialogKDV::CheckConf()
{
}

void ConfDialogKDV::SetDefConf()
{
    KDV->SetDefConf();
    Fill();
}

void ConfDialogKDV::Start_Timer()
{
    timerRead->start(1000);
}

void ConfDialogKDV::Stop_Timer()
{
    timerRead->stop();
}

void ConfDialogKDV::Write_PCDate()
{
    QDateTime myDateTime;
    uint time;
    myDateTime = QDateTime::currentDateTimeUtc();
    time = myDateTime.toSecsSinceEpoch();
}

void ConfDialogKDV::Write_Date()
{
    QDateTime myDateTime;
    uint *time = new uint;
    QString qStr;
    WDFunc::LE_read_data(this, "Date", qStr);
    myDateTime = QDateTime::fromString(qStr, "dd-MM-yyyy HH:mm:ss");
    myDateTime.setOffsetFromUtc(0);
    *time = myDateTime.toSecsSinceEpoch();
}

void ConfDialogKDV::Set104(double dbl)
{
    QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 1)
    {
        ERMSG("Некорректные данные 104");
        DBGMSG;
        return;
    }
    bool ok;
    int wnum = sl.at(1).toInt(&ok);
    if (!ok)
    {
        ERMSG("Некорректные данные 104");
        DBGMSG;
        return;
    }
    switch (wnum)
    {
    case 1:
    {
        KDV->MainBlk.Abs_104 = dbl;
        break;
    }
    case 2:
    {
        KDV->MainBlk.Cycle_104 = dbl;
        break;
    }
    case 3:
    {
        KDV->MainBlk.T1_104 = dbl;
        break;
    }
    case 4:
    {
        KDV->MainBlk.T2_104 = dbl;
        break;
    }
    case 5:
    {
        KDV->MainBlk.T3_104 = dbl;
        break;
    }
    case 6:
    {
        KDV->MainBlk.k_104 = dbl;
        break;
    }
    case 7:
    {
        KDV->MainBlk.w_104 = dbl;
        break;
    }
    default:
        break;
    }
}

void ConfDialogKDV::SetCType(int num)
{
    KDV->MainBlk.Ctype = num;
}
