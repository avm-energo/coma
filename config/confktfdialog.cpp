#include "confktfdialog.h"

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

ConfKTFDialog::ConfKTFDialog(ConfigKTF *cktf, QWidget *parent) : AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    setStyleSheet(tmps);
    S2Config = cktf->S2Config();
    CKTF = cktf;
    Conf = new ConfDialog(S2Config, Board::GetInstance().typeB(), Board::GetInstance().typeM());
    ConfKxx = new ConfKxxDialog(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
}

ConfKTFDialog::~ConfKTFDialog()
{
}

int ConfKTFDialog::getRCount()
{
    return CKTF->Bci_block.TdatNum;
}

void ConfKTFDialog::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    QScrollArea *area = new QScrollArea;
    QScrollArea *LinkscrArea = new QScrollArea;

    QWidget *analog = new QWidget;
    QWidget *transf = new QWidget;
    QWidget *alarm = new QWidget;
    QWidget *ocsillogram = new QWidget;
    QWidget *link = new QWidget;
    QWidget *other = new QWidget;

    QString tmps = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
    analog->setStyleSheet(tmps);
    transf->setStyleSheet(tmps);
    alarm->setStyleSheet(tmps);
    ocsillogram->setStyleSheet(tmps);
    link->setStyleSheet(tmps);
    other->setStyleSheet(tmps);

    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    LinkscrArea->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    LinkscrArea->setFrameShape(QFrame::NoFrame);
    LinkscrArea->setWidgetResizable(true);

    //  uint32_t unixtimestamp = 1423062000;

    //  QDateTime myDateTime;

    // myDateTime.setTime_t(unixtimestamp);

    QString paramcolor = Colors::MAINWINCLR;
    QFont font;

    //.........................................................................

    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    gridlyout->addWidget(WDFunc::NewLBL(this, "Класс напряжения, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Unom1", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение , В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "U2nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальный первичный ток, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "ITT1nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальный вторичный ток , А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "ITT2nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Количество датчиков температуры обмоток, "
                             "подключенных по Modbus Master:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TdatNum", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Максимальное измеряемое фазное напряжение на входе "
                             "прибора, В эфф (не более 305В):"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Umaxm", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Максимальное измеряемый ток на входе "
                             "прибора, А эфф (не более 33А) :"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Imaxm", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток контролирууемой обмотки, А:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Iwnom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума сигналов:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "DUImin_ID", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума токов:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog->setLayout(vlyout1);

    //.........................................................................

    gb = new QGroupBox();
    gb->setFont(font);
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;
    gridlyout->setColumnStretch(2, 50);
    row = 0;

    QLabel *lbl = new QLabel("Вид охлаждения: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList { "естественное", "принудительное" };
    EComboBox *cb = WDFunc::NewCB(this, "Cool_type", cbl, paramcolor);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;
    lbl = new QLabel("Материал охлаждения: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "медь", "алюминий" };
    cb = WDFunc::NewCB(this, "W_mat", cbl, paramcolor);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Коэффициент добавочных потерь :"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Kdob", 0, 10000, 1, paramcolor), row, 1, 1, 1);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TauWnom", 0, 10000, 0, paramcolor), row, 1, 1, 1);
    row++;

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    transf->setLayout(vlyout1);

    //.........................................................................

    row = 0;
    gb = new QGroupBox();
    gb->setFont(font);
    gridlyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Предельно допустимая температура ННТ в°С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TNNTdop", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "TNNTpred", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Tamb_nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dTNNTnom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "GTnnt", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по токовой перегрузке, % от :"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "GOvc", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    alarm->setLayout(vlyout1);

    //.........................................................................

    gb = new QGroupBox();
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    vlyout1 = new QVBoxLayout;
    gridlyout = new QGridLayout;
    row = 0;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Разрешение запуска осциллограммы:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewChB(this, "DDosc_ID1", "по команде 104"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewChB(this, "DDosc_ID2", "по резкому изменению"), row, 2, 1, 1);
    row++;

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox();
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;
    gridlyout->setColumnStretch(2, 50);
    row = 0;

    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Уставка скачка напряжения для запуска "
                             "осциллографирования - % от номинала:"),
        row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "DUosc", 0, 10000, 1, paramcolor), row, 1, 1, 1);
    row++;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "DIosc_ID", 0, 10000, 1, paramcolor), row, 1, 1, 1);
    row++;

    lbl = new QLabel("Количество точек осциллограммы на период основной частоты: ");
    gridlyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    cbl = QStringList { "256", "128", "64", "32", "16" };
    cb = WDFunc::NewCB(this, "OscPoints", cbl, paramcolor);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    gridlyout->addWidget(cb, row, 1, 1, 1);
    row++;

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    ocsillogram->setLayout(vlyout1);

    //.........................................................................

    row = 0;
    vlyout1 = new QVBoxLayout;
    gb = new QGroupBox();
    gridlyout = new QGridLayout;
    vlyout2 = new QVBoxLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    gb->setFont(font);

    gridlyout->addWidget(Conf->SetupMainBlk(this), 0, 0, 1, 1);
    gridlyout->addWidget(ConfKxx->SetupComParam(this), 0, 1, 1, 1);

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    //    vlyout2->addWidget(Conf->SetupMainBlk(this));
    //    vlyout2->addWidget(ConfKxx->SetupComParam(this));

    //    gb->setLayout(vlyout2);
    //    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройка времени");
    vlyout2 = new QVBoxLayout;
    gb->setFont(font);

    vlyout2->addWidget(Conf->SetupTime(this));

    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    link->setLayout(vlyout1);
    LinkscrArea->setWidget(link);

    //.........................................................................

    row = 0;
    gb = new QGroupBox();
    gb->setFont(font);
    gridlyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Интервал усреднения данных, период:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "NFiltr_ID", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени фильтрации гармоник, цикл:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "NHarmFilt_ID", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), с:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    vlyout2->addLayout(gridlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    other->setLayout(vlyout1);

    //.........................................................................

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::TABCOLOR) + ";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(analog, "Аналоговые");
    ConfTW->addTab(transf, "Трансформатор");
    ConfTW->addTab(alarm, "Сигнализация");
    ConfTW->addTab(ocsillogram, "Осциллографирование");
    ConfTW->addTab(LinkscrArea, "Связь");
    ConfTW->addTab(ConfKxx->SetupModBus(this), "ModBus");
    ConfTW->addTab(ConfKxx->SetupBl(this), "Общее");
    ConfTW->addTab(other, "Прочее");
    lyout->addWidget(ConfTW);
    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfKTFDialog::Fill()
{

    Conf->Fill();

    WDFunc::SetSPBData(this, "NFiltr_ID", CKTF->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "NHarmFilt_ID", CKTF->Bci_block.NHarmFilt);

    if (CKTF->Bci_block.DDosc & 0x04)
        WDFunc::SetChBData(this, "DDosc_ID1", true);
    else
        WDFunc::SetChBData(this, "DDosc_ID1", false);

    if (CKTF->Bci_block.DDosc & 0x01)
        WDFunc::SetChBData(this, "DDosc_ID2", true);
    else
        WDFunc::SetChBData(this, "DDosc_ID2", false);

    WDFunc::SetSPBData(this, "Unom1", CKTF->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "DUosc", CKTF->Bci_block.DUosc);
    WDFunc::SetSPBData(this, "DIosc_ID", CKTF->Bci_block.DIosc);
    WDFunc::SetSPBData(this, "DUImin_ID", CKTF->Bci_block.DUImin);
    WDFunc::SetSPBData(this, "Imin", CKTF->Bci_block.Imin);
    WDFunc::SetSPBData(this, "T_Data_Rec", CKTF->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "U2nom", CKTF->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "ITT1nom", CKTF->Bci_block.ITT1nom);
    WDFunc::SetSPBData(this, "ITT2nom", CKTF->Bci_block.ITT2nom);

    int cbidx = ((CKTF->Bci_block.Cool_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    cbidx = ((CKTF->Bci_block.W_mat & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    WDFunc::SetSPBData(this, "TNNTdop", CKTF->Bci_block.TNNTdop);
    WDFunc::SetSPBData(this, "TNNTpred", CKTF->Bci_block.TNNTpred);
    WDFunc::SetSPBData(this, "Tamb_nom", CKTF->Bci_block.Tamb_nom);
    WDFunc::SetSPBData(this, "dTNNTnom", CKTF->Bci_block.dTNNTnom);
    WDFunc::SetSPBData(this, "Kdob", CKTF->Bci_block.Kdob);
    WDFunc::SetSPBData(this, "TauWnom", CKTF->Bci_block.TauWnom);
    WDFunc::SetSPBData(this, "Umaxm", CKTF->Bci_block.Umax);
    WDFunc::SetSPBData(this, "Imaxm", CKTF->Bci_block.Imax);
    WDFunc::SetSPBData(this, "Iwnom", CKTF->Bci_block.Iwnom);

    for (int i = 0; i < 5; i++)
    {
        if (CKTF->Bci_block.OscPoints == Rates.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(this, "OscPoints", cbidx);

    WDFunc::SetSPBData(this, "GTnnt", CKTF->Bci_block.GTnnt);
    WDFunc::SetSPBData(this, "GOvc", CKTF->Bci_block.GOvc);
    WDFunc::SetSPBData(this, "TdatNum", CKTF->Bci_block.TdatNum);

    ConfKxx->Fill();
}

void ConfKTFDialog::FillBack()
{
    Conf->FillBack();

    int cbidx;

    WDFunc::SPBData(this, "NFiltr_ID", CKTF->Bci_block.NFiltr);
    WDFunc::SPBData(this, "NHarmFilt_ID", CKTF->Bci_block.NHarmFilt);

    WDFunc::ChBData(this, "DDosc_ID1", DDosc);
    if (DDosc)
    {
        CKTF->Bci_block.DDosc = 0x04;
    }

    else
        CKTF->Bci_block.DDosc = 0;

    WDFunc::ChBData(this, "DDosc_ID2", DDosc);
    if (DDosc)
    {
        CKTF->Bci_block.DDosc = CKTF->Bci_block.DDosc | 0x01;
    }

    WDFunc::SPBData(this, "Unom1", CKTF->Bci_block.Unom1);
    WDFunc::SPBData(this, "DUosc", CKTF->Bci_block.DUosc);
    WDFunc::SPBData(this, "DIosc_ID", CKTF->Bci_block.DIosc);
    WDFunc::SPBData(this, "DUImin_ID", CKTF->Bci_block.DUImin);
    WDFunc::SPBData(this, "Imin", CKTF->Bci_block.Imin);
    WDFunc::SPBData(this, "T_Data_Rec", CKTF->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "U2nom", CKTF->Bci_block.U2nom);
    WDFunc::SPBData(this, "ITT1nom", CKTF->Bci_block.ITT1nom);
    WDFunc::SPBData(this, "ITT2nom", CKTF->Bci_block.ITT2nom);

    cbidx = WDFunc::CBIndex(this, "Cool_type");
    CKTF->Bci_block.Cool_type = cbidx;

    cbidx = WDFunc::CBIndex(this, "W_mat");
    CKTF->Bci_block.W_mat = cbidx;

    WDFunc::SPBData(this, "TNNTdop", CKTF->Bci_block.TNNTdop);
    WDFunc::SPBData(this, "TNNTpred", CKTF->Bci_block.TNNTpred);
    WDFunc::SPBData(this, "Tamb_nom", CKTF->Bci_block.Tamb_nom);
    WDFunc::SPBData(this, "dTNNTnom", CKTF->Bci_block.dTNNTnom);
    WDFunc::SPBData(this, "Kdob", CKTF->Bci_block.Kdob);
    WDFunc::SPBData(this, "TauWnom", CKTF->Bci_block.TauWnom);
    WDFunc::SPBData(this, "Umaxm", CKTF->Bci_block.Umax);
    WDFunc::SPBData(this, "Imaxm", CKTF->Bci_block.Imax);
    WDFunc::SPBData(this, "Iwnom", CKTF->Bci_block.Iwnom);

    cbidx = WDFunc::CBIndex(this, "OscPoints");
    CKTF->Bci_block.OscPoints = (Rates.at(cbidx).toInt());

    WDFunc::SPBData(this, "GTnnt", CKTF->Bci_block.GTnnt);
    WDFunc::SPBData(this, "GOvc", CKTF->Bci_block.GOvc);
    WDFunc::SPBData(this, "TdatNum", CKTF->Bci_block.TdatNum);

    ConfKxx->FillBack();
}

void ConfKTFDialog::CheckConf()
{
}

void ConfKTFDialog::SetDefConf()
{
    CKTF->SetDefConf();
    Conf->SetDefConf();
    ConfKxx->SetDefConf();
    Fill();
}

// void ConfKTFDialog::Start_Timer()
//{
//    timerRead->start(1000);
//}

// void ConfKTFDialog::Stop_Timer()
//{
//    timerRead->stop();
//}
