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
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Unom1", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение , В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "U2nom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный первичный ток, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT1nom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный вторичный ток , А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "ITT2nom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Количество датчиков температуры обмоток, "
                             "подключенных по Modbus Master:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TdatNum", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальное измеряемое фазное напряжение на входе "
                             "прибора, В эфф (не более 305В):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Umaxm", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Максимальное измеряемый ток на входе "
                             "прибора, А эфф (не более 33А) :"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imaxm", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальный ток контролирууемой обмотки, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Iwnom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка контроля минимума сигналов:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "DUImin_ID", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка контроля минимума токов:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imin", 0, 10000, 1), row, 2, 1, 3);
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
    auto *cb = WDFunc::NewCB2(this, "Cool_type", cbl);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;
    lbl = new QLabel("Материал охлаждения: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "медь", "алюминий" };
    cb = WDFunc::NewCB2(this, "W_mat", cbl);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Коэффициент добавочных потерь :"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Kdob", 0, 10000, 1), row, 1, 1, 1);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TauWnom", 0, 10000, 0), row, 1, 1, 1);
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
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TNNTdop", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "TNNTpred", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Tamb_nom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dTNNTnom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "GTnnt", 0, 10000, 1), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Гистерезис сигнализации по токовой перегрузке, % от :"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "GOvc", 0, 10000, 1), row, 2, 1, 3);
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
    gridlyout->addWidget(WDFunc::NewChB2(this, "DDosc_ID1", "по команде 104"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewChB2(this, "DDosc_ID2", "по резкому изменению"), row, 2, 1, 1);
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
    gridlyout->addWidget(WDFunc::NewSPB2(this, "DUosc", 0, 10000, 1), row, 1, 1, 1);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "DIosc_ID", 0, 10000, 1), row, 1, 1, 1);
    row++;

    QLabel *lbl = new QLabel("Количество точек осциллограммы на период основной частоты: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList { "256", "128", "64", "32", "16" };
    auto *cb = WDFunc::NewCB2(this, "OscPoints", cbl);
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
    gridlyout->addWidget(WDFunc::NewSPB2(this, "NFiltr_ID", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Постоянная времени фильтрации гармоник, цикл:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "NHarmFilt_ID", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "T_Data_Rec", 0, 10000, 0), row, 2, 1, 3);
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

    WDFunc::SetSPBData(this, "NFiltr_ID", ConfKTF->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "NHarmFilt_ID", ConfKTF->Bci_block.NHarmFilt);

    if (ConfKTF->Bci_block.DDosc & 0x04)
        WDFunc::SetChBData(this, "DDosc_ID1", true);
    else
        WDFunc::SetChBData(this, "DDosc_ID1", false);

    if (ConfKTF->Bci_block.DDosc & 0x01)
        WDFunc::SetChBData(this, "DDosc_ID2", true);
    else
        WDFunc::SetChBData(this, "DDosc_ID2", false);

    WDFunc::SetSPBData(this, "Unom1", ConfKTF->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "DUosc", ConfKTF->Bci_block.DUosc);
    WDFunc::SetSPBData(this, "DIosc_ID", ConfKTF->Bci_block.DIosc);
    WDFunc::SetSPBData(this, "DUImin_ID", ConfKTF->Bci_block.DUImin);
    WDFunc::SetSPBData(this, "Imin", ConfKTF->Bci_block.Imin);
    WDFunc::SetSPBData(this, "T_Data_Rec", ConfKTF->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "U2nom", ConfKTF->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "ITT1nom", ConfKTF->Bci_block.ITT1nom);
    WDFunc::SetSPBData(this, "ITT2nom", ConfKTF->Bci_block.ITT2nom);

    int cbidx = ((ConfKTF->Bci_block.Cool_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    cbidx = ((ConfKTF->Bci_block.W_mat & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    WDFunc::SetSPBData(this, "TNNTdop", ConfKTF->Bci_block.TNNTdop);
    WDFunc::SetSPBData(this, "TNNTpred", ConfKTF->Bci_block.TNNTpred);
    WDFunc::SetSPBData(this, "Tamb_nom", ConfKTF->Bci_block.Tamb_nom);
    WDFunc::SetSPBData(this, "dTNNTnom", ConfKTF->Bci_block.dTNNTnom);
    WDFunc::SetSPBData(this, "Kdob", ConfKTF->Bci_block.Kdob);
    WDFunc::SetSPBData(this, "TauWnom", ConfKTF->Bci_block.TauWnom);
    WDFunc::SetSPBData(this, "Umaxm", ConfKTF->Bci_block.Umax);
    WDFunc::SetSPBData(this, "Imaxm", ConfKTF->Bci_block.Imax);
    WDFunc::SetSPBData(this, "Iwnom", ConfKTF->Bci_block.Iwnom);

    for (int i = 0; i < 5; i++)
    {
        if (ConfKTF->Bci_block.OscPoints == Rates.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(this, "OscPoints", cbidx);

    WDFunc::SetSPBData(this, "GTnnt", ConfKTF->Bci_block.GTnnt);
    WDFunc::SetSPBData(this, "GOvc", ConfKTF->Bci_block.GOvc);
    WDFunc::SetSPBData(this, "TdatNum", ConfKTF->Bci_block.TdatNum);

    //    ConfKxx->Fill();
}

void ConfKTFDialog::FillBackKtf() const
{

    int cbidx;

    //
    //  NOTE Need to reworked as others
    //

    //  WDFunc::SPBData(this, "NFiltr_ID", ConfKTF->Bci_block.NFiltr);
    //  WDFunc::SPBData(this, "NHarmFilt_ID", ConfKTF->Bci_block.NHarmFilt);

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

    //    WDFunc::SPBData(this, "Unom1", ConfKTF->Bci_block.Unom1);
    //    WDFunc::SPBData(this, "DUosc", ConfKTF->Bci_block.DUosc);
    //    WDFunc::SPBData(this, "DIosc_ID", ConfKTF->Bci_block.DIosc);
    //    WDFunc::SPBData(this, "DUImin_ID", ConfKTF->Bci_block.DUImin);
    //    WDFunc::SPBData(this, "Imin", ConfKTF->Bci_block.Imin);
    //    WDFunc::SPBData(this, "T_Data_Rec", ConfKTF->Bci_block.T_Data_Rec);
    //    WDFunc::SPBData(this, "U2nom", ConfKTF->Bci_block.U2nom);
    //    WDFunc::SPBData(this, "ITT1nom", ConfKTF->Bci_block.ITT1nom);
    //    WDFunc::SPBData(this, "ITT2nom", ConfKTF->Bci_block.ITT2nom);

    //    cbidx = WDFunc::CBIndex(this, "Cool_type");
    //    ConfKTF->Bci_block.Cool_type = cbidx;

    //    cbidx = WDFunc::CBIndex(this, "W_mat");
    //    ConfKTF->Bci_block.W_mat = cbidx;

    //    WDFunc::SPBData(this, "TNNTdop", ConfKTF->Bci_block.TNNTdop);
    //    WDFunc::SPBData(this, "TNNTpred", ConfKTF->Bci_block.TNNTpred);
    //    WDFunc::SPBData(this, "Tamb_nom", ConfKTF->Bci_block.Tamb_nom);
    //    WDFunc::SPBData(this, "dTNNTnom", ConfKTF->Bci_block.dTNNTnom);
    //    WDFunc::SPBData(this, "Kdob", ConfKTF->Bci_block.Kdob);
    //    WDFunc::SPBData(this, "TauWnom", ConfKTF->Bci_block.TauWnom);
    //    WDFunc::SPBData(this, "Umaxm", ConfKTF->Bci_block.Umax);
    //    WDFunc::SPBData(this, "Imaxm", ConfKTF->Bci_block.Imax);
    //    WDFunc::SPBData(this, "Iwnom", ConfKTF->Bci_block.Iwnom);

    //    cbidx = WDFunc::CBIndex(this, "OscPoints");
    //    ConfKTF->Bci_block.OscPoints = (Rates.at(cbidx).toInt());

    //    WDFunc::SPBData(this, "GTnnt", ConfKTF->Bci_block.GTnnt);
    //    WDFunc::SPBData(this, "GOvc", ConfKTF->Bci_block.GOvc);
    //    WDFunc::SPBData(this, "TdatNum", ConfKTF->Bci_block.TdatNum);
}

void ConfKTFDialog::CheckConf() const
{
}

void ConfKTFDialog::SetDefConf()
{
    ConfKTF->SetDefConf();
    Fill();
}
