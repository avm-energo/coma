#include "confkdvdialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
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
    QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    setStyleSheet(tmps);
    S2Config = ckdv->S2Config();
    KDV = ckdv;
    Conf = new ConfDialog(S2Config, Board::GetInstance().typeB(), Board::GetInstance().typeM());
    ConfKxx = new ConfKxxDialog(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    PrereadConf();
}

void ConfKDVDialog::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    QScrollArea *Analog_area = new QScrollArea;
    QScrollArea *area2 = new QScrollArea;
    QScrollArea *scrArea = new QScrollArea;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *MEKconf = new QWidget;
    QWidget *Leftconf = new QWidget;
    QWidget *time = new QWidget;
    QWidget *link = new QWidget;
    QString tmps = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
    extraconf->setStyleSheet(tmps);
    time->setStyleSheet(tmps);
    MEKconf->setStyleSheet(tmps);
    Leftconf->setStyleSheet(tmps);
    link->setStyleSheet(tmps);

    area2->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area2->setFrameShape(QFrame::NoFrame);
    area2->setWidgetResizable(true);

    Analog_area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    Analog_area->setFrameShape(QFrame::NoFrame);
    Analog_area->setWidgetResizable(true);

    scrArea->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    scrArea->setFrameShape(QFrame::NoFrame);
    scrArea->setWidgetResizable(true);

    QString paramcolor = Colors::MAINWINCLR;
    QFont font;

    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    // setFont(font);
    gb->setFont(font);

    QLabel *lbl = new QLabel("Тип контролируемого оборудования:");
    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList { "Асинхронный двигатель", "Трансформатор", "Реактор" };
    EComboBox *cb = WDFunc::NewCB(this, "Eq_type", cbl, paramcolor);
    cb->setMinimumHeight(20);
    gridlyout->addWidget(cb, row, 2, 1, 3);
    row++;

    lbl = new QLabel("Вид охлаждения:");
    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "Естественное", "Принудительное" };
    cb = WDFunc::NewCB(this, "Cool_type", cbl, paramcolor);
    cb->setMinimumHeight(20);
    gridlyout->addWidget(cb, row, 2, 1, 3);
    row++;

    lbl = new QLabel("Материал обмотки:");
    gridlyout->addWidget(lbl, row, 1, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "Медь", "Алюминий" };
    cb = WDFunc::NewCB(this, "W_mat", cbl, paramcolor);
    cb->setMinimumHeight(20);
    gridlyout->addWidget(cb, row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Unom1", 0, 10000, 2, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "U2nom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальный первичный ток, кА:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "ITT1nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальный вторичный ток, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "ITT2nom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток контролируемой обмотки, À:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Iwnom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Tamb_nom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dTNNTnom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Коэффициент добавочных потерь:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Kdob", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TauWnom", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Максимальное измеряемое фазное напряжение на входе "
                             "прибора, В эфф (не более 305В):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Umax", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Максимальный измеряемый ток на входе "
                             "прибора, А эфф  (не более 33А):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Imax", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Количество датчиков температуры обмоток, "
                             "подключенных по Modbus Master:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TdatNum", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальная частота, Гц"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Fnom", 0, 1000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальное скольжение"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "nom_slip", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "UVmax", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    analog1->setLayout(vlyout1);
    Analog_area->setWidget(analog1);
    //.....................................................................

    gb = new QGroupBox("Уставки сигнализации");
    gb->setFont(font);
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;
    row = 0;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Уставка скачка напряжения для запуска "
                             "осциллографирования - % от номинала:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "DUosc", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "DIosc", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка порога минимального напряжения - % от номинального уровня:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "DUImin", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка порога минимального тока - % от номинального уровня:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Предельно допустимая температура ННТ в°С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TNNTdop", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TNNTpred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Предупредительная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VibrA_pred", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Предупредительная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VibrV_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Предупредительная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VibrD_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Аварийная уставка по СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VibrA_alarm", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Аварийная уставка по СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VibrV_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Аварийная уставка по СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VibrD_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Предупредительная уставка по скорости роста СКЗ "
                             "виброускорения, м/с/с:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VVibrA_pred", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Предупредительная уставка по скорости роста СКЗ "
                             "виброскорости, мм/с:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VVibrV_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Предупредительная уставка по скорости роста СКЗ "
                             "виброперемещения, мкм:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VVibrD_pred", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Аварийная уставка по скорости роста СКЗ виброускорения, м/с/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VVibrA_alarm", 0, 1000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Аварийная уставка по скорости роста СКЗ виброскорости, мм/с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VVibrV_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Аварийная уставка по скорости роста СКЗ виброперемещения, мкм:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VVibrD_alarm", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    //.....................................................................
    gb = new QGroupBox("Гистерезис");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "GTnnt", 0, 10000, 3, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Гистерезис сигнализации по токовой перегрузке, % от Iwnom:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "GOvc", 0, 10000, 1, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    analog2->setLayout(vlyout1);
    area2->setWidget(analog2);

    //.....................................................................

    gb = new QGroupBox("Параметры записи");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    gridlyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "NFiltr", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени фильтрации гармоник:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "NHarmFilt", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Запуск осциллограммы:"), row, 1, 1, 1);
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.0", "по команде Ц", Colors::ACONFWCLR));
    //    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", Colors::ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", Colors::ACONFWCLR));
    gridlyout->addLayout(hlyout, row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Количество точек осциллограммы на период основной частоты:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "OscPoints", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Параметры двигателя");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Коэффициент передачи датчиков вибрации:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Kvibr", 0, 10000, 2, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Порядковый номер двигателя:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "NumA", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Число пар полюсов (от 1 до 10):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Poles", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Число пазов на статоре:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Stator_Slotes", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Число стержней ротора:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Rotor_bars", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Тип вибродатчика:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "VibroType", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Подключенные датчики вибрации:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Sensors", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    Leftconf->setLayout(vlyout1);
    //.....................................................................

    row = 0;
    vlyout1 = new QVBoxLayout;
    gb = new QGroupBox;
    gridlyout = new QGridLayout;
    vlyout2 = new QVBoxLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    gb->setFont(font);

    gridlyout->addWidget(Conf->SetupMainBlk(this), 0, 0, 1, 1);
    gridlyout->addWidget(ConfKxx->SetupComParam(this), 0, 1, 1, 1);

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройка времени");
    vlyout2 = new QVBoxLayout;
    gb->setFont(font);

    vlyout2->addWidget(Conf->SetupTime(this));

    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    link->setLayout(vlyout1);
    scrArea->setWidget(link);

    //....................................................................

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::TABCOLOR) + ";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(Analog_area, "Аналоговые");

    ConfTW->addTab(area2, "Уставки");

    ConfTW->addTab(scrArea, "Связь");
    ConfTW->addTab(ConfKxx->SetupModBus(this), "ModBus");
    ConfTW->addTab(ConfKxx->SetupBl(this), "Общее");

    ConfTW->addTab(Leftconf, "Остальное");
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfKDVDialog::Fill()
{
    Conf->Fill();

    int cbidx;

    //.........................................................
    cbidx = (KDV->Bci_block.Eq_type & 0x02) ? 2 : ((KDV->Bci_block.Eq_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Eq_type", cbidx);
    cbidx = ((KDV->Bci_block.Cool_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    cbidx = ((KDV->Bci_block.W_mat & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    WDFunc::SetChBData(this, "oscchb.0", (KDV->Bci_block.DDosc & 0x0001));
    //    WDFunc::SetChBData(this, "oscchb.1", (KDV->Bci_block.DDosc & 0x0002));
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
    ConfKxx->Fill();
}

void ConfKDVDialog::FillBack()
{
    Conf->FillBack();

    int cbidx;
    bool tmpb;

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
    KDV->Bci_block.DDosc = 0;
    KDV->Bci_block.DDosc |= (tmpb) ? 0x0001 : 0x0000;
    //    WDFunc::ChBData(this, "oscchb.1", tmpb);
    //    KDV->Bci_block.DDosc |= (tmpb) ? 0x0002 : 0x0000;
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
    ConfKxx->FillBack();
}

void ConfKDVDialog::CheckConf()
{
}

void ConfKDVDialog::SetDefConf()
{
    KDV->SetDefConf();
    Conf->SetDefConf();
    ConfKxx->SetDefConf();
    Fill();
}

void ConfKDVDialog::Start_Timer()
{
    timerRead->start(1000);
}

void ConfKDVDialog::Stop_Timer()
{
    timerRead->stop();
}
