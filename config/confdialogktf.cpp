
#include <QGroupBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QTimer>
#include <QScrollArea>
#include <QScrollBar>
#include "../widgets/emessagebox.h"
#include "../widgets/ecombobox.h"
#include "../widgets/wd_func.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "confdialogktf.h"
#include "../gen/error.h"

ConfDialogKTF::ConfDialogKTF(QVector<S2::DataRec> *S2Config, QWidget *parent):
     AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = S2Config;
    KTF = new ConfigKTF(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    PrereadConf();
}

ConfDialogKTF::~ConfDialogKTF()
{
}

void ConfDialogKTF::Fill()
{

    int i;

    WDFunc::SetSPBData(this, "Abs_104", KTF->MainBlk.Abs_104);
    WDFunc::SetSPBData(this, "Cycle_104", KTF->MainBlk.Cycle_104);
    WDFunc::SetSPBData(this, "T1_104", KTF->MainBlk.T1_104);
    WDFunc::SetSPBData(this, "T2_104", KTF->MainBlk.T2_104);
    WDFunc::SetSPBData(this, "T3_104", KTF->MainBlk.T3_104);
    WDFunc::SetSPBData(this, "k_104", KTF->MainBlk.k_104);
    WDFunc::SetSPBData(this, "w_104", KTF->MainBlk.w_104);

    int cbidx = ((KTF->MainBlk.Ctype & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "cbb", cbidx);



    WDFunc::SetSPBData(this, "NFiltr_ID", KTF->Bci_block.NFiltr_ID);
    WDFunc::SetSPBData(this, "NHarmFilt_ID", KTF->Bci_block.NHarmFilt_ID);


 //   WDFunc::SetSPBData(this, "DDosc_ID", KTF->Bci_block.DDosc_ID);

    if(KTF->Bci_block.DDosc_ID & 0x04)
    WDFunc::SetChBData(this, "DDosc_ID1", true);
    else
    WDFunc::SetChBData(this, "DDosc_ID1", false);


    if(KTF->Bci_block.DDosc_ID& 0x01)
    WDFunc::SetChBData(this, "DDosc_ID2", true);
    else
    WDFunc::SetChBData(this, "DDosc_ID2", false);



    WDFunc::SetSPBData(this, "Unom1", KTF->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "DUosc", KTF->Bci_block.DUosc);
    WDFunc::SetSPBData(this, "DIosc_ID", KTF->Bci_block.DIosc_ID);
    WDFunc::SetSPBData(this, "DUImin_ID", KTF->Bci_block.DUImin_ID);
    WDFunc::SetSPBData(this, "Imin", KTF->Bci_block.Imin);
    WDFunc::SetSPBData(this, "RTerm", KTF->Bci_block.RTerm);
    WDFunc::SetSPBData(this, "W100", KTF->Bci_block.W100);
    WDFunc::SetSPBData(this, "T_Data_Rec", KTF->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "Trele_pred", KTF->Bci_block.Trele_pred);
    WDFunc::SetSPBData(this, "Trele_alarm", KTF->Bci_block.Trele_alarm);
    WDFunc::SetSPBData(this, "U2nom", KTF->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "ITT1nom", KTF->Bci_block.ITT1nom);
    WDFunc::SetSPBData(this, "ITT2nom", KTF->Bci_block.ITT2nom);
    WDFunc::SetSPBData(this, "MBMaster", KTF->Bci_block.MBMaster);

    //нет с 1061-1064

    cbidx = ((KTF->Bci_block.Cool_type & 0x02) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    cbidx = ((KTF->Bci_block.W_mat & 0x02) ? 1 : 0);
    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    WDFunc::SetSPBData(this, "TNNTdop", KTF->Bci_block.TNNTdop);
    WDFunc::SetSPBData(this, "TNNTpred", KTF->Bci_block.TNNTpred);
    WDFunc::SetSPBData(this, "Tamb_nom", KTF->Bci_block.Tamb_nom);
    WDFunc::SetSPBData(this, "dTNNTnom", KTF->Bci_block.dTNNTnom);
    WDFunc::SetSPBData(this, "Kdob", KTF->Bci_block.Kdob);
    WDFunc::SetSPBData(this, "TauWnom", KTF->Bci_block.TauWnom);
    WDFunc::SetSPBData(this, "Umaxm", KTF->Bci_block.Umaxm);
    WDFunc::SetSPBData(this, "Iwnom", KTF->Bci_block.Iwnom);


    for(int i= 0; i<5; i++)
    {
        if(KTF->Bci_block.OscPoints == Rates.at(i).toUInt())
        cbidx = i;
    }
    WDFunc::SetCBIndex(this, "OscPoints", cbidx);

    WDFunc::SetSPBData(this, "GTnnt", KTF->Bci_block.GTnnt);
    WDFunc::SetSPBData(this, "GOvc", KTF->Bci_block.GOvc);
    WDFunc::SetSPBData(this, "TdatNum", KTF->Bci_block.TdatNum);





}

void ConfDialogKTF::FillBack()
{
    int i;
    int cbidx;
    QString tmps;

    WDFunc::SPBData(this, "Abs_104", KTF->MainBlk.Abs_104);
    WDFunc::SPBData(this, "Cycle_104", KTF->MainBlk.Cycle_104);
    WDFunc::SPBData(this, "T1_104", KTF->MainBlk.T1_104);
    WDFunc::SPBData(this, "T2_104", KTF->MainBlk.T2_104);
    WDFunc::SPBData(this, "T3_104", KTF->MainBlk.T3_104);
    WDFunc::SPBData(this, "k_104", KTF->MainBlk.k_104);
    WDFunc::SPBData(this, "w_104", KTF->MainBlk.w_104);

    cbidx = ((KTF->MainBlk.Ctype & 0x01) ? 1 : 0);
    KTF -> MainBlk.Ctype = (0x00000001 << cbidx) - 1;



    WDFunc::SPBData(this, "NFiltr_ID", KTF->Bci_block.NFiltr_ID);
    WDFunc::SPBData(this, "NHarmFilt_ID", KTF->Bci_block.NHarmFilt_ID);

    WDFunc::SPBData(this, "DDosc_ID", KTF->Bci_block.DDosc_ID);

    WDFunc::ChBData(this, "DDosc_ID1", DDosc);
    if(DDosc)
    {
     KTF->Bci_block.DDosc_ID = 0x04;
    }

    else
    KTF->Bci_block.DDosc_ID = 0;


    WDFunc::ChBData(this, "DDosc_ID2", DDosc );
    if(DDosc)
    {
     KTF->Bci_block.DDosc_ID = KTF->Bci_block.DDosc_ID | 0x01;
    }

    WDFunc::SPBData(this, "Unom1", KTF->Bci_block.Unom1);
    WDFunc::SPBData(this, "DUosc", KTF->Bci_block.DUosc);
    WDFunc::SPBData(this, "DIosc_ID", KTF->Bci_block.DIosc_ID);
    WDFunc::SPBData(this, "DUImin_ID", KTF->Bci_block.DUImin_ID);
    WDFunc::SPBData(this, "Imin", KTF->Bci_block.Imin);
    WDFunc::SPBData(this, "RTerm", KTF->Bci_block.RTerm);
    WDFunc::SPBData(this, "W100", KTF->Bci_block.W100);
    WDFunc::SPBData(this, "T_Data_Rec", KTF->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "Trele_pred", KTF->Bci_block.Trele_pred);
    WDFunc::SPBData(this, "Trele_alarm", KTF->Bci_block.Trele_alarm);
    WDFunc::SPBData(this, "U2nom", KTF->Bci_block.U2nom);
    WDFunc::SPBData(this, "ITT1nom", KTF->Bci_block.ITT1nom);
    WDFunc::SPBData(this, "ITT2nom", KTF->Bci_block.ITT2nom);
    WDFunc::SPBData(this, "MBMaster", KTF->Bci_block.MBMaster);

    //нет с 1061-1064

    cbidx = ((KTF->Bci_block.Cool_type & 0x02) ? 1 : 0);
    WDFunc::CBIndex(this, "Cool_type");
    cbidx = ((KTF->Bci_block.W_mat & 0x02) ? 1 : 0);
    WDFunc::CBIndex(this, "W_mat");

    WDFunc::SPBData(this, "TNNTdop", KTF->Bci_block.TNNTdop);
    WDFunc::SPBData(this, "TNNTpred", KTF->Bci_block.TNNTpred);
    WDFunc::SPBData(this, "Tamb_nom", KTF->Bci_block.Tamb_nom);
    WDFunc::SPBData(this, "dTNNTnom", KTF->Bci_block.dTNNTnom);
    WDFunc::SPBData(this, "Kdob", KTF->Bci_block.Kdob);
    WDFunc::SPBData(this, "TauWnom", KTF->Bci_block.TauWnom);
    WDFunc::SPBData(this, "Umaxm", KTF->Bci_block.Umaxm);
    WDFunc::SPBData(this, "Iwnom", KTF->Bci_block.Iwnom);

    cbidx = WDFunc::CBIndex(this, "OscPoints");
    KTF->Bci_block.OscPoints = (Rates.at(cbidx).toInt());



    WDFunc::SPBData(this, "GTnnt", KTF->Bci_block.GTnnt);
    WDFunc::SPBData(this, "GOvc", KTF->Bci_block.GOvc);
    WDFunc::SPBData(this, "TdatNum", KTF->Bci_block.TdatNum);




}

void ConfDialogKTF::SetupUI()
{

    QString phase[3] = {"Фаза A:","Фаза B:","Фаза C:"};
    //QString S;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QScrollArea *area = new QScrollArea;
    //QScrollArea *area2 = new QScrollArea;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *analog3 = new QWidget;
    QWidget *analog4 = new QWidget;
    QWidget *analog5 = new QWidget;
    QWidget *analog6 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *MEKconf = new QWidget;
    QWidget *Leftconf = new QWidget;
    QWidget *time = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
    analog3->setStyleSheet(tmps);
    analog4->setStyleSheet(tmps);
    analog5->setStyleSheet(tmps);
    analog6->setStyleSheet(tmps);
    extraconf->setStyleSheet(tmps);
    time->setStyleSheet(tmps);
    MEKconf->setStyleSheet(tmps);
    Leftconf->setStyleSheet(tmps);

    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    uint32_t unixtimestamp = 1423062000;

    QDateTime myDateTime;

    myDateTime.setTime_t(unixtimestamp);

    QString paramcolor = MAINWINCLR;
    QFont font;

    QGroupBox *gb = new QGroupBox;

    //.........................................................................

    int row = 0;
    gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    //setFont(font);
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Класс напряжения, кВ:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom1", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение , В:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "U2nom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный первичный ток, А:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "ITT1nom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный вторичный ток , А:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "ITT2nom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Максимальное измеряемое фазное напряжение на входе прибора, В эфф (не более 305В):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Umaxm", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Максимальное измеряемый ток на входе прибора, А эфф (не более 33А) :"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imaxm", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток контролирууемой обмотки, А:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Iwnom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, " Уставка контроля минимума сигналов:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "DUImin", 0, 10000, 1, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума токов:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, paramcolor), row,2,1,3);
    row++;


    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog1->setLayout(vlyout1);

    //.........................................................................

    gb = new QGroupBox();
    gb->setFont(font);
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    row = 0;

    QLabel *lbl = new QLabel();
    lbl = new QLabel("Вид охлаждения: ");
    glyout->addWidget(lbl,row,0,1,1,Qt::AlignLeft);
    QStringList cbl = QStringList() << "естественное" << "принудительное";
    EComboBox *cb = WDFunc::NewCB(this, "Cool_type", cbl, paramcolor);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cb, row, 1, 1, 1);
    row++;
    lbl = new QLabel("Материал охлаждения: ");
    glyout->addWidget(lbl,row,0,1,1,Qt::AlignLeft);
    QStringList bl = QStringList() << "медь" << "алюминий";
    EComboBox *cсb = WDFunc::NewCB(this, "W_mat", bl , paramcolor);
    connect(cсb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cсb, row, 1, 1, 1);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент добавочных потерь :"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Kdob", 0, 10000, 1, paramcolor), row,1,1,1);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "TauWnom", 0, 10000, 0, paramcolor), row,1,1,1);
    row++;


    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog2->setLayout(vlyout1);

    //.........................................................................

    row = 0;
    gb = new QGroupBox();
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Предельно допустимая температура ННТ в°С:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "TNNTdop", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "TNNTpred", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная температура окружающей среды, °С:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Tamb_nom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dTNNTnom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GTnnt", 0, 10000, 1, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по токовой перегрузке, % от :"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GOvc", 0, 10000, 1, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Задержка срабатывания реле предупредительной сигнализации:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Trele_pred", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Задержка срабатывания реле аварийной сигнализации:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Trele_alarm", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog3->setLayout(vlyout1);

    //.........................................................................

    gb = new QGroupBox();
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    vlyout1 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

    glyout->addWidget(WDFunc::NewLBL(this, "Разрешение запуска осциллограммы:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewChB(this,"DDosc_ID1", "по комманде 104"), row,1,1,1);
    glyout->addWidget(WDFunc::NewChB(this,"DDosc_ID2","по резкому изменению"), row,2,1,1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);


    gb = new QGroupBox();
    gb->setFont(font);
   // vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    row = 0;


    //glyout->addWidget(WDFunc::NewLBL(this, "Разрешение запуска осциллограммы :"), row,0,1,1);
  //  glyout->addWidget(WDFunc::NewSPB(this, "DDosc", 0, 10000, 1, paramcolor), row,1,1,1);
   // row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка скачка напряжения для запуска осциллографирования - % от номинала:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "DUosc", 0, 10000, 1, paramcolor), row,1,1,1);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "DIosc_ID", 0, 10000, 1, paramcolor), row,1,1,1);
    row++;

    lbl = new QLabel("Количество точек осциллограммы на период основной частоты: ");
    glyout->addWidget(lbl,row,0,1,1,Qt::AlignLeft);
    QStringList bbl = QStringList()  << "256"<< "128"<< "64"<< "32"<< "16";
    EComboBox *cсbb = WDFunc::NewCB(this, "OscPoints", bbl , paramcolor);
    connect(cсb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cсbb, row, 1, 1, 1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog4->setLayout(vlyout1);


    //.........................................................................

    row = 0;
    gb = new QGroupBox();
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    lbl = new QLabel("Modbus: ");
    glyout->addWidget(lbl,row,0,1,1,Qt::AlignLeft);
    QStringList bbll = QStringList() << "slave" << "master";
    EComboBox *cсbbb = WDFunc::NewCB(this, "MBMaster", bbll , paramcolor);
    connect(cсbbb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cсbbb, row, 1, 1, 1);
    row++;











    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog5->setLayout(vlyout1);

    //.........................................................................

    row = 0;
    gb = new QGroupBox();
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Интервал усреднения данных, период:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "NFiltr_ID", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени фильтрации гармоник, цикл:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "NHarmFilt_ID", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), с:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное сопротивление термометра при 0 град.С:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "RTerm", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Температурный коэффициент термометра:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "W100", 0, 10000, 3, paramcolor), row,2,1,3);
    row++;


    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog6->setLayout(vlyout1);



    //.........................................................................


    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(analog1,"Аналоговые");
    ConfTW->addTab(analog2,"Трансформатор");
    ConfTW->addTab(analog3,"Сигнализация");
    ConfTW->addTab(analog4,"Осцилографирование");
    ConfTW->addTab(analog5,"ModBusMaster");
    ConfTW->addTab(analog6,"Прочее");

    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);


}

void ConfDialogKTF::CheckConf()
{

}

void ConfDialogKTF::SetDefConf()
{
    KTF->SetDefConf();
   Fill();
}

void ConfDialogKTF::Start_Timer()
{
     timerRead->start(1000);
}

void ConfDialogKTF::Stop_Timer()
{
   timerRead->stop();
}

void ConfDialogKTF::Write_PCDate()
{

}

void ConfDialogKTF::Write_Date()
{
    QDateTime myDateTime;
    uint *time = new uint;
    QString qStr;
    WDFunc::LE_read_data(this, "Date", qStr);
    myDateTime = QDateTime::fromString(qStr,"dd-MM-yyyy HH:mm:ss");
    myDateTime.setOffsetFromUtc(0);
    *time = myDateTime.toTime_t();
}

void ConfDialogKTF::Set104(double dbl)
{

}

void ConfDialogKTF::SetCType(int num)
{
  // KTF->MainBlk.Ctype = num;
}




























