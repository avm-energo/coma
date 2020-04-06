#include <QCoreApplication>
#include <QtMath>
#include <QTime>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include "../gen/modulebsi.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../config/config.h"
#include "../gen/error.h"
#include "checktempmodbusdialog.h"

checktempmodbusdialog::checktempmodbusdialog(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QStringList sl;
    connectionStateRTU = false;
    sl = QStringList() << "Основные" << "Дополнительные";//"Датчик температуры";
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
                "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";

    BdUINum = sl.size();

    SetupUI(sl);

    #if PROGSIZE != PROGSIZE_EMUL
    timer->setInterval(ANMEASINT);
   // BdTimer = new QTimer;
   // BdTimer->setInterval(ANMEASINT);
   // connect(BdTimer,SIGNAL(timeout()),this,SLOT(BdTimerTimeout()));
   #endif

}

QWidget *checktempmodbusdialog::BdUI(int bdnum)
{
    switch (bdnum)
    {
    /*case 0:
        return Ch->Bd0W(this);*/
    case 0: // Блок #1
        return Bd1W(this);
    case 1: // Блок #2
        return Bd2W(this);
    case 2: // Блок #3
        return Bd3W(this);

    default:
        return new QWidget;
    }
}

QWidget *checktempmodbusdialog::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString phase[3] = {"A:","B:","C:"};
    //hlyout->addWidget(WDFunc::NewLBL(parent, "Номер:"), 0);
    //hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value0", ValuesFormat, "Номер"), 0);
    hlyout->addWidget(WDFunc::NewLBL(parent, "Температура микроконтроллера, °С:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "101", ValuesFormat, "Температура микроконтроллера, °С"), 0);
    hlyout->addWidget(WDFunc::NewLBL(parent, "Частота:"));
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "102", ValuesFormat, "Частота, Гц"));
    lyout->addLayout(hlyout);
    for (i = 0; i < 3; ++i)
    {
        //QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff ф."+phase[i]),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1000+i), ValuesFormat, \
                                          QString::number(i+3)+".Действующие значения напряжений по 1-й гармонике, кВ"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ieff ф."+phase[i]),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1100+i), ValuesFormat, \
                                          QString::number(i+6)+".Действующие значения токов по 1-й гармонике, мА"),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Cbush ф."+phase[i]),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2420+i), ValuesFormat, \
                                          QString::number(i+9)+".Ёмкости вводов, пФ"),5,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Tg_d ф."+phase[i]),6,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2423+i), ValuesFormat, \
                                          QString::number(i+12)+".Тангенсы дельта вводов"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dCbush ф."+phase[i]),8,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2426+i), ValuesFormat, \
                                          QString::number(i+15)+".Изменение емкостей вводов, пФ"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dTg_d ф."+phase[i]),10,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2429+i), ValuesFormat, \
                                          QString::number(i+18)+".Изменение тангенсов дельта вводов, %"),11,i,1,1);
        /*glyout->addWidget(WDFunc::NewLBL(parent, "KrF ф."+phase[i]),12,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1008+i), ValuesFormat, \
                                          QString::number(i+21)+".Крест фактор"),13,i,1,1);*/
    }

    glyout->addWidget(WDFunc::NewLBL(parent, "Iunb"),12,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2432), ValuesFormat, \
                                      QString::number(21)+".Действующее значение тока небаланса, мА"),13,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Phy_unb"),12,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2433), ValuesFormat, \
                                      QString::number(22)+".Угол тока небаланса относительно тока ф.А, град."),13,1,1,1);

    glyout->addWidget(WDFunc::NewLBL(parent, "Tamb"),12,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(4501), ValuesFormat, \
                                      QString::number(23)+".Температура окружающей среды, °С"),13,2,1,1);

    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "U_phi_next_f ф."+phase[i]),14,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(2400+i), ValuesFormat, \
                                          QString::number(24+i)+".Угол по напряжению ф."+phase[i]+", град."),15,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "I_phi_next_f ф."+phase[i]),16,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(2403+i), ValuesFormat, \
                                          QString::number(27+i)+".Угол по напряжению ф."+phase[i]+", град."),17,i,1,1);
    }

    /*QPushButton *pb = new QPushButton("Стереть журнал");
    //pb->setObjectName("pbmeasurements");
    #if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(SendErt()));
    #endif
     glyout->addWidget(pb,12,0,1,3);*/


    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *checktempmodbusdialog::Bd2W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "U0 ф.А"),0,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1020), ValuesFormat, \
                                      QString::number(29)+".Напряжение нулевой последовательности ф.А"),1,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1 ф.B"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1021), ValuesFormat, \
                                      QString::number(30)+".Напряжение прямой последовательности ф.B"),1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2 ф.C"),0,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1022), ValuesFormat, \
                                      QString::number(31)+".Напряжение обратной последовательности ф.C"),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim0"),2,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1023), ValuesFormat, \
                                      QString::number(32)+".Коэффициент несимметрии по нулевой последовательности по напряжению"),3,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim2"),2,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1024), ValuesFormat, \
                                      QString::number(33)+".Коэффициент несимметрии по обратной последовательности по напряжению"),3,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I0 ф.А"),4,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1120), ValuesFormat, \
                                      QString::number(34)+".Ток обратной последовательности ф.А"),5,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I1 ф.B"),4,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1121), ValuesFormat, \
                                      QString::number(35)+".Ток обратной последовательности ф.B"),5,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I2 ф.C"),4,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1122), ValuesFormat, \
                                      QString::number(36)+".Ток обратной последовательности ф.С"),5,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim0"),6,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1123), ValuesFormat, \
                                      QString::number(37)+".Коэффициент несимметрии по нулевой последовательности по току"),7,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim2"),6,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1124), ValuesFormat, \
                                      QString::number(38)+".Коэффициент несимметрии по обратной последовательности по току"),7,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "KrF ф.A"),8,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1008), ValuesFormat, \
                                      QString::number(39)+".Крест фактор"),9,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "KrF ф.B"),8,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1009), ValuesFormat, \
                                      QString::number(40)+".Крест фактор"),9,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "KrF ф.B"),8,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1010), ValuesFormat, \
                                      QString::number(41)+".Крест фактор"),9,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Ramb"),10,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(4502), ValuesFormat, \
                                      QString::number(42)+".Сопротивление датчика, Ом"),11,0,1,1);
    /*glyout->addWidget(WDFunc::NewLBL(parent, "Time"),6,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value29", ValuesFormat, \
                                      QString::number(29)+".Время записи, с"),7,0,1,1);*/



    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;

}

QWidget *checktempmodbusdialog::Bd3W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
    //{
        glyout->addWidget(WDFunc::NewLBL(parent, "Логический ноль - "),0,0,1,1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBL(parent, "", GRNCOLOR),0,1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Логическая единица - "),0,3,1,1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLBL(parent, "", REDCOLOR),0,4,1,1);


        glyout->addWidget(WDFunc::NewLBL(parent, "Сигналы:"),3,2,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Сигнал:"),4,1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "", GRNCOLOR, QString::number(3000)),4,2,1,1);
    //}


    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}


void checktempmodbusdialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetLBLText(parent, Name, Value);
}

void checktempmodbusdialog::onModbusStateChanged(QModbusDevice::State state)
{
    if(state == QModbusDevice::ConnectedState)
    {
     connectionStateRTU =  true;
     EMessageBox::information(this, "Успешно", "Связь по MODBUS установлена");
    }
    else
    {
     connectionStateRTU =  false;
     EMessageBox::information(this, "Провал", "Подключение отсутствует");
    }

}

void checktempmodbusdialog::RefreshAnalogValues(int bdnum)
{
  Q_UNUSED(bdnum);
}

void checktempmodbusdialog::PrepareHeadersForFile(int row)
{
  Q_UNUSED(row);
}

void checktempmodbusdialog::WriteToFile(int row, int bdnum)
{
  Q_UNUSED(row);
  Q_UNUSED(bdnum);
}

void checktempmodbusdialog::ChooseValuesToWrite(void)
{

}

void checktempmodbusdialog::SetDefaultValuesToWrite(void)
{

}

void checktempmodbusdialog::PrepareAnalogMeasurements(void)
{

}

QWidget *checktempmodbusdialog::CustomTab(void)
{
    QWidget *w = nullptr;

    return w;
}

void checktempmodbusdialog::UpdateModBusData(ModBusSignal *Signal, int * size)
{

    //ModBusSignal sig = *new ModBusSignal;
    int i = 0;
    for(i=0; i<*size; i++)
    {
      //sig = *(Signal+i);
      FillBd(this, QString::number((Signal+i)->SigAdr), WDFunc::StringValueWithCheck((Signal+i)->flVal));
    }
    ModBus::Reading = false;
}

void checktempmodbusdialog::ErrorRead()
{
  //EMessageBox::information(this, "INFO", "Ошибка чтения");
}
