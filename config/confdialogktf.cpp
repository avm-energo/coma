
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

    WDFunc::SetSPBData(this, "Unom1", KTF->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "U2nom", KTF->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "ITT1nom", KTF->Bci_block.ITT1nom);
    WDFunc::SetSPBData(this, "ITT2nom", KTF->Bci_block.ITT2nom);
    WDFunc::SetSPBData(this, "Umaxm", KTF->Bci_block.Umaxm);
    WDFunc::SetSPBData(this, "Imaxm", KTF->Bci_block.Imaxm);
    WDFunc::SetSPBData(this, "Iwnom", KTF->Bci_block.Iwnom);


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

    WDFunc::SPBData(this, "Unom1", KTF->Bci_block.Unom1);
    WDFunc::SPBData(this, "U2nom", KTF->Bci_block.U2nom);
    WDFunc::SPBData(this, "ITT1nom", KTF->Bci_block.ITT1nom);
    WDFunc::SPBData(this, "ITT2nom", KTF->Bci_block.ITT2nom);
    WDFunc::SPBData(this, "Umaxm", KTF->Bci_block.Umaxm);
    WDFunc::SPBData(this, "Imaxm", KTF->Bci_block.Imaxm);
    WDFunc::SPBData(this, "Iwnom", KTF->Bci_block.Iwnom);




}

void ConfDialogKTF::SetupUI()
{

    QString phase[3] = {"Фаза A:","Фаза B:","Фаза C:"};
    //QString S;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QScrollArea *area = new QScrollArea;
    QScrollArea *area2 = new QScrollArea;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *MEKconf = new QWidget;
    QWidget *Leftconf = new QWidget;
    QWidget *time = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
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
    EComboBox *cb = WDFunc::NewCB(this, "spb.8", cbl, paramcolor);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cb, row, 1, 1, 1);
    row++;
    lbl = new QLabel("Материал охлаждения: ");
    glyout->addWidget(lbl,row,0,1,1,Qt::AlignLeft);
    QStringList bl = QStringList() << "медь" << "алюминий";
    EComboBox *cсb = WDFunc::NewCB(this, "spb.9", bl , paramcolor);
    connect(cсb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cсb, row, 1, 1, 1);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент добавочных потерь :"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Kdob", 0, 10000, 0, paramcolor), row,1,1,1);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "TauWnom", 0, 10000, 0, paramcolor), row,1,1,1);
    row++;


    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    analog2->setLayout(vlyout1);




    //.........................................................................



    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    //ConfTW->addTab(MEKconf,"Конфигурирование МЭК-60870-5-104");
    ConfTW->addTab(analog1,"Аналоговые");

    ConfTW->addTab(analog2,"Трансформатор");


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





















































