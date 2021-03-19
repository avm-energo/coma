#include "confktfdialog.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/s2.h"
//#include "../widgets/ecombobox.h"
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

ConfKTFDialog::ConfKTFDialog(ConfigKTF *cktf, QWidget *parent) : AbstractConfDialog(parent)
{
    ConfKTF = cktf;
}

ConfKTFDialog::~ConfKTFDialog()
{
    delete ConfKTF;
}

void ConfKTFDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;

    ConfTW->setObjectName("conftw");
    ConfTW->addTab(analogWidget(), "Аналоговые");
    ConfTW->addTab(transformerWidget(), "Трансформатор");
    ConfTW->addTab(alarmWidget(), "Сигнализация");
    ConfTW->addTab(ocsillogramWidget(), "Осциллографирование");

    ConfTW->addTab(connectionWidget(), "Связь");
    ConfTW->addTab(ConfKTF->KxxConfig()->ModbusWidget(this), "ModBusMaster");
    ConfTW->addTab(ConfKTF->KxxConfig()->VariousWidget(this), "Общее");

    ConfTW->addTab(otherWidget(), "Прочее");

    lyout->addWidget(ConfTW);
    lyout->addWidget(ConfButtons());
    setLayout(lyout);
}

QWidget *ConfKTFDialog::analogWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QFont font;
    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Класс напряжения, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Unom1), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение , В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::U2nom), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный первичный ток, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::ITT1nom_KTF_KDV), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный вторичный ток , А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::ITT2nom_KTF_KDV), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Количество датчиков температуры обмоток, "
                             "подключенных по Modbus Master:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TdatNum), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальное измеряемое фазное напряжение на входе "
                             "прибора, В эфф (не более 305В):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Umaxm), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальное измеряемый ток на входе "
                             "прибора, А эфф (не более 33А) :"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Imaxm), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный ток контролирууемой обмотки, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Iwnom), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка контроля минимума сигналов:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::DUImin_ID), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка контроля минимума токов:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Imin), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKTFDialog::transformerWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QFont font;
    gridlyout->setColumnStretch(2, 50);
    int row = 0;
    QGroupBox *gb = new QGroupBox();
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    QLabel *lbl = new QLabel("Вид охлаждения: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList { "естественное", "принудительное" };
    auto *cb = WDFunc::NewCB2(this, nameByValue(BciNumber::Cool_type), cbl);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;
    lbl = new QLabel("Материал охлаждения: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "медь", "алюминий" };
    cb = WDFunc::NewCB2(this, nameByValue(BciNumber::W_mat), cbl);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент добавочных потерь :"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Kdob), 0, 10000, 1), row, 1, 1, 1);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TauWnom), 0, 10000, 0), row, 1, 1, 1);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKTFDialog::alarmWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QFont font;
    gridlyout->setColumnStretch(2, 50);
    int row = 0;
    QGroupBox *gb = new QGroupBox();
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Предельно допустимая температура ННТ в°С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TNNTdop), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::TNNTpred), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::Tamb_nom), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::dTNNTnom), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::GTnnt), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Гистерезис сигнализации по токовой перегрузке, % от :"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::GOvc), 0, 10000, 1), row, 2, 1, 3);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKTFDialog::ocsillogramWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QFont font;
    gridlyout->setColumnStretch(2, 50);
    int row = 0;
    QGroupBox *gb = new QGroupBox();
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Разрешение запуска осциллограммы:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewChB2(this, nameByValue(BciNumber::DDOsc_ID), "по команде 104"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewChB2(this, nameByValue(BciNumber::DDOsc_ID), "по резкому изменению"), row, 2, 1, 1);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox();
    gb->setFont(font);
    gridlyout = new QGridLayout;
    gridlyout->setColumnStretch(2, 50);
    row = 0;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка скачка напряжения для запуска "
                             "осциллографирования - % от номинала:"),
        row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::DUosc), 0, 10000, 1), row, 1, 1, 1);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::DIosc_ID), 0, 10000, 1), row, 1, 1, 1);
    row++;

    QLabel *lbl = new QLabel("Количество точек осциллограммы на период основной частоты: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList { "256", "128", "64", "32", "16" };
    auto *cb = WDFunc::NewCB2(this, nameByValue(BciNumber::OscPoints), cbl);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKTFDialog::connectionWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;

    QGridLayout *gridlyout = new QGridLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");

    gridlyout->addWidget(ConfKTF->MainConfig()->MainWidget(this), 0, 0, 1, 1);
    gridlyout->addWidget(ConfKTF->KxxConfig()->ComParam(this), 0, 1, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка времени");

    vlyout->addWidget(ConfKTF->MainConfig()->TimeWidget(this));

    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

QWidget *ConfKTFDialog::otherWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);
    QFont font;
    gridlyout->setColumnStretch(2, 50);
    int row = 0;
    QGroupBox *gb = new QGroupBox();
    gb->setFont(font);

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал усреднения данных, период:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::NFiltr_ID), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Постоянная времени фильтрации гармоник, цикл:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::NHarmFilt_ID), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, nameByValue(BciNumber::T_Data_Rec), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

void ConfKTFDialog::Fill()
{

    ConfKTF->MainConfig()->Fill();
    ConfKTF->KxxConfig()->Fill();
    FillKtf();
}

void ConfKTFDialog::FillBack() const
{

    ConfKTF->MainConfig()->FillBack();
    ConfKTF->KxxConfig()->FillBack();
    FillBackKtf();
}

void ConfKTFDialog::FillKtf()
{

    //    Conf->Fill();
    using namespace DataTypes;

    SetSPBData<float>(this, BciNumber::Unom1);
    SetSPBData<float>(this, BciNumber::U2nom);
    SetSPBData<float>(this, BciNumber::ITT1nom_KTF_KDV);
    SetSPBData<float>(this, BciNumber::ITT2nom_KTF_KDV);
    SetSPBData<DWORD>(this, BciNumber::TdatNum);
    SetSPBData<float>(this, BciNumber::Umaxm);
    SetSPBData<float>(this, BciNumber::Imaxm);
    SetSPBData<float>(this, BciNumber::Iwnom);
    SetSPBData<float>(this, BciNumber::DUImin_ID);
    SetSPBData<float>(this, BciNumber::Imin);
    // SetSPBData<DWORD>(this, BciNumber::Cool_type); потом
    SetSPBData<DWORD>(this, BciNumber::W_mat);
    SetSPBData<float>(this, BciNumber::Kdob);
    SetSPBData<float>(this, BciNumber::TauWnom);
    SetSPBData<float>(this, BciNumber::TNNTdop);
    SetSPBData<float>(this, BciNumber::TNNTpred);
    SetSPBData<float>(this, BciNumber::Tamb_nom);
    SetSPBData<float>(this, BciNumber::dTNNTnom);
    SetSPBData<float>(this, BciNumber::GTnnt);
    SetSPBData<float>(this, BciNumber::GOvc);
    // SetSPBData<DWORD>(this, BciNumber::DDOsc_ID); потом
    SetSPBData<float>(this, BciNumber::DUosc);
    SetSPBData<float>(this, BciNumber::DIosc_ID);
    SetSPBData<DWORD>(this, BciNumber::NFiltr_ID);
    SetSPBData<DWORD>(this, BciNumber::NHarmFilt_ID);
    SetSPBData<DWORD>(this, BciNumber::T_Data_Rec);
    //    SetSPBData<DWORD>(this, BciNumber::OscPoints); потом

    //    if (ConfKTF->Bci_block.DDosc & 0x04)
    //        WDFunc::SetChBData(this, "DDosc_ID1", true);
    //    else
    //        WDFunc::SetChBData(this, "DDosc_ID1", false);

    //    if (ConfKTF->Bci_block.DDosc & 0x01)
    //        WDFunc::SetChBData(this, "DDosc_ID2", true);
    //    else
    //        WDFunc::SetChBData(this, "DDosc_ID2", false);

    //    int cbidx = ((ConfKTF->Bci_block.Cool_type & 0x01) ? 1 : 0);
    //    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    //    cbidx = ((ConfKTF->Bci_block.W_mat & 0x01) ? 1 : 0);
    //    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    //    for (int i = 0; i < 5; i++)
    //    {
    //        if (ConfKTF->Bci_block.OscPoints == Rates.at(i).toUInt())
    //            cbidx = i;
    //    }
    //    WDFunc::SetCBIndex(this, "OscPoints", cbidx);
}

void ConfKTFDialog::FillBackKtf() const
{
    using namespace DataTypes;

    SPBDataS2<float>(this, BciNumber::Unom1);
    SPBDataS2<float>(this, BciNumber::U2nom);
    SPBDataS2<float>(this, BciNumber::ITT1nom_KTF_KDV);
    SPBDataS2<float>(this, BciNumber::ITT2nom_KTF_KDV);
    SPBDataS2<DWORD>(this, BciNumber::TdatNum);
    SPBDataS2<float>(this, BciNumber::Umaxm);
    SPBDataS2<float>(this, BciNumber::Imaxm);
    SPBDataS2<float>(this, BciNumber::Iwnom);
    SPBDataS2<float>(this, BciNumber::DUImin_ID);
    SPBDataS2<float>(this, BciNumber::Imin);
    SPBDataS2<DWORD>(this, BciNumber::W_mat);
    SPBDataS2<float>(this, BciNumber::Kdob);
    SPBDataS2<float>(this, BciNumber::TauWnom);
    SPBDataS2<float>(this, BciNumber::TNNTdop);
    SPBDataS2<float>(this, BciNumber::TNNTpred);
    SPBDataS2<float>(this, BciNumber::Tamb_nom);
    SPBDataS2<float>(this, BciNumber::dTNNTnom);
    SPBDataS2<float>(this, BciNumber::GTnnt);
    SPBDataS2<float>(this, BciNumber::GOvc);
    SPBDataS2<float>(this, BciNumber::DUosc);
    SPBDataS2<float>(this, BciNumber::DIosc_ID);
    SPBDataS2<DWORD>(this, BciNumber::NFiltr_ID);
    SPBDataS2<DWORD>(this, BciNumber::NHarmFilt_ID);
    SPBDataS2<DWORD>(this, BciNumber::T_Data_Rec);

    //    int cbidx;

    //    WDFunc::ChBData(this, "DDosc_ID1", DDosc);
    //    if (DDosc)
    //    {
    //        ConfKTF->Bci_block.DDosc = 0x04;
    //    }

    // else ConfKTF->Bci_block.DDosc = 0;

    //    WDFunc::ChBData(this, "DDosc_ID2", DDosc);
    //    if (DDosc)
    //    {
    //        ConfKTF->Bci_block.DDosc = ConfKTF->Bci_block.DDosc | 0x01;
    //    }

    //    cbidx = WDFunc::CBIndex(this, "Cool_type");
    //    ConfKTF->Bci_block.Cool_type = cbidx;

    //    cbidx = WDFunc::CBIndex(this, "W_mat");
    //    ConfKTF->Bci_block.W_mat = cbidx;

    //    cbidx = WDFunc::CBIndex(this, "OscPoints");
    //    ConfKTF->Bci_block.OscPoints = (Rates.at(cbidx).toInt());
}

void ConfKTFDialog::CheckConf() const
{
}

void ConfKTFDialog::SetDefConf()
{
    ConfKTF->SetDefConf();
    Fill();
}
