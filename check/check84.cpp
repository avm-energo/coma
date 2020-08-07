#include <QGridLayout>
#include <QtMath>
#include <QWidget>
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include <QObject>
#include "check84.h"
#include "../config/config.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"
#include "../widgets/emessagebox.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif


Check_84::Check_84(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    //setAttribute(Qt::WA_DeleteOnClose);
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
                "background-color: "+QString(ACONFOCLR)+"; font: bold 11px;}";
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";
}

Check_84::~Check_84()
{
}

QWidget *Check_84::Bd1W(QWidget *parent)
{
    int i;
    QString paramcolor = MAINWINCLR;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString phase[3] = {"A:","B:","C:"};
    hlyout->addWidget(WDFunc::NewLBL(parent, "Номер:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value0", ValuesFormat, "Номер"), 0);
    hlyout->addWidget(WDFunc::NewLBL(parent, "Температура микроконтроллера, °С:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value1", ValuesFormat, "Температура микроконтроллера, °С"), 0);
    hlyout->addWidget(WDFunc::NewLBL(parent, "Частота:"));
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value2", ValuesFormat, "Частота, Гц"));
    lyout->addLayout(hlyout);
    for (i = 0; i < 3; ++i)
    {
        //QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff ф."+phase[i]),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+3), ValuesFormat, \
                                          QString::number(i+3)+".Действующие значения напряжений по 1-й гармонике, кВ"),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ieff ф."+phase[i]),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+6), ValuesFormat, \
                                          QString::number(i+6)+".Действующие значения токов по 1-й гармонике, мА"),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Cbush ф."+phase[i]),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+9), ValuesFormat, \
                                          QString::number(i+9)+".Ёмкости вводов, пФ"),5,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Tg_d ф."+phase[i]),6,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+12), ValuesFormat, \
                                          QString::number(i+12)+".tg delta вводов, пФ"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dCbush ф."+phase[i]),8,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+15), ValuesFormat, \
                                          QString::number(i+15)+".Изменение емкостей вводов, пФ"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "dTg_d ф."+phase[i]),10,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+18), ValuesFormat, \
                                          QString ::number(i+18)+".Изменение тангенсов дельта вводов, %"),11,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "U_phi_next_f ф."+phase[i]),12,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "",  "value"+QString::number(i+31), ValuesFormat, \
                                          QString::number(24+i)+".Угол по напряжению ф."+phase[i]+", град."),13,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "I_phi_next_f ф."+phase[i]),14,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "",  "value"+QString::number(i+34), ValuesFormat, \
                                          QString::number(27+i)+".Угол по току ф."+phase[i]+", град."),15,i,1,1);
    }


    /*glyout->addWidget(WDFunc::NewLBL(this, "Заводские значения tg δ вводов:"), 12,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Tg_zav."+QString::number(i), -10, 10, 2, paramcolor), 12,2+i,1,1);
    }*/

    //QPushButton *pb = new QPushButton("Рассчитать начальные значения tg δ вводов");
    /*pb->setObjectName("pbcalc");
    #if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(EnterTg()));
    #endif
     glyout->addWidget(pb,13,0,1,3);*/
    
    QPushButton *pb = new QPushButton("Стереть журнал");
    //pb->setObjectName("pbmeasurements");
    #if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(SendErt()));
    #endif
     glyout->addWidget(pb,16,0,1,3);


    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_84::Bd2W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "U0"),0,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value21", ValuesFormat, \
                                      QString::number(21)+".Напряжение нулевой последовательности"),1,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value22", ValuesFormat, \
                                      QString::number(22)+".Напряжение прямой последовательности"),1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2"),0,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value23", ValuesFormat, \
                                      QString::number(23)+".Напряжение обратной последовательности"),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I0"),2,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value24", ValuesFormat, \
                                      QString::number(24)+".Ток нулевой последовательности"),3,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I1"),2,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value25", ValuesFormat, \
                                      QString::number(25)+".Ток прямой последовательности"),3,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "I2"),2,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value26", ValuesFormat, \
                                      QString::number(26)+".Ток обратной последовательности"),3,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Iunb"),4,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value27", ValuesFormat, \
                                      QString::number(27)+".Действующее значение 1-й гармоники тока небаланса, мА"),5,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Phy_unb"),4,1,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value28", ValuesFormat, \
                                      QString::number(28)+".Угол тока небаланса относительно тока ф.А, град."),5,1,1,1);
    //glyout->addWidget(WDFunc::NewLBL(parent, "Time"),4,2,1,1);
    //glyout->addWidget(WDFunc::NewLBLT(parent, "", "value29", ValuesFormat, \
    //                                  QString::number(29)+".Время записи, с"),5,2,1,1);



    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;

}

QWidget *Check_84::Bd3W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Tamb"),6,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value30", ValuesFormat, \
                                          QString::number(30)+".Температура окружающей среды, °С"),7,0,1,1);
        /*glyout->addWidget(WDFunc::NewLBL(parent, "Ramb"),8,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value31", ValuesFormat, \
                                          QString::number(31)+".Температура окружающей среды, °С"),9,0,1,1);*/
    }


    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *Check_84::Bd4W(QWidget *parent)
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QWidget *w = new QWidget(parent);
    int row = 0;
    rele1=rele2=rele3=rele4=0;
    Fwidjet = parent;


    QPushButton *Button = new QPushButton("Подать команду на реле №1");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    glyout->addWidget(WDFunc::NewLBL(parent, "Разомкнуто", QString(ACONFOCLR), "lbl1"), row,2,1,1,Qt::AlignCenter|Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Rele1()));

    row++;
    Button = new QPushButton("Подать команду на реле №2");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    glyout->addWidget(WDFunc::NewLBL(parent, "Разомкнуто", QString(ACONFOCLR), "lbl2"), row,2,1,1,Qt::AlignCenter|Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Rele2()));

    row++;
    Button = new QPushButton("Подать команду на реле №3");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    glyout->addWidget(WDFunc::NewLBL(parent, "Разомкнуто", QString(ACONFOCLR), "lbl3"), row,2,1,1,Qt::AlignCenter|Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Rele3()));

    /*row++;
    Button = new QPushButton("Подать команду на реле №4");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    glyout->addWidget(WDFunc::NewLBL(parent, "Разомкнуто", QString(ACONFOCLR), "lbl4"), row,2,1,1,Qt::AlignCenter|Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Rele4()));*/

    row++;
    Button = new QPushButton("Сбросить все реле");
    glyout->addWidget(Button, row,1,1,2,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Switch_off()));

    row++;
    glyout->addWidget(WDFunc::NewLBL(parent, "Din24"),row,1,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(parent, "Разомкнуто", QString(ACONFOCLR), "Din24"), row,2,1,1,Qt::AlignCenter);
    //connect(Button, SIGNAL(clicked()), this, SLOT(Rele1()));

    row++;
    glyout->addWidget(WDFunc::NewLBL(parent, "1PPS"),row,1,1,1,Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(parent, "Разомкнуто", QString(ACONFOCLR), "1PPS"), row,2,1,1,Qt::AlignCenter);
    //connect(Button, SIGNAL(clicked()), this, SLOT(Rele2()));

    vlyout->addLayout(glyout);
    w->setLayout(vlyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

void Check_84::Rele1()
{
   StartTest();
   if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
   {
       if(rele1 != 1)
       {
        if(Commands::WriteCom(101) == Error::ER_NOERROR)   // включение
        {
         WDFunc::SetLBLText(Fwidjet, "lbl1", "Замкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele1 = 1;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");
       }
       else  // выключение
       {
        if(Commands::WriteCom(121) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl1", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele1 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");
       }
   }
   StopTest();
}

void Check_84::Rele2()
{
    StartTest();
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        if(rele2 != 1)
        {
         if(Commands::WriteCom(102) == Error::ER_NOERROR)   // включение
         {
          WDFunc::SetLBLText(Fwidjet, "lbl2", "Замкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele2 = 1;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
        else  // выключение
        {
         if(Commands::WriteCom(122) == Error::ER_NOERROR)
         {
          WDFunc::SetLBLText(Fwidjet, "lbl2", "Разомкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele2 = 0;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
    }
    StopTest();

}

void Check_84::Rele3()
{
    StartTest();
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        if(rele3 != 1)
        {
         if(Commands::WriteCom(103) == Error::ER_NOERROR)   // включение
         {
          WDFunc::SetLBLText(Fwidjet, "lbl3", "Замкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele3 = 1;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
        else  // выключение
        {
         if(Commands::WriteCom(123) == Error::ER_NOERROR)
         {
          WDFunc::SetLBLText(Fwidjet, "lbl3", "Разомкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele3 = 0;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
    }
    StopTest();

}


void Check_84::Switch_off()
{
    StartTest();
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        if(Commands::WriteCom(121) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl1", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele1 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");

        if(Commands::WriteCom(122) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl2", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele2 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");

        if(Commands::WriteCom(123) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl3", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele3 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");
    }
    StopTest();
}

void Check_84::FillBd(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bd_block1.NUM, 3));
    WDFunc::SetLBLText(parent, "value1", WDFunc::StringValueWithCheck(Bd_block1.Tmk, 3));
    WDFunc::SetLBLText(parent, "value2", WDFunc::StringValueWithCheck(Bd_block1.Frequency, 3));
    for (int i = 0; i < 3; i++)
    {

        WDFunc::SetLBLText(parent, "value"+QString::number(i+3), WDFunc::StringValueWithCheck(Bd_block1.Ueff[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+6), WDFunc::StringValueWithCheck(Bd_block1.Ieff[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+9), WDFunc::StringValueWithCheck(Bd_block1.Cbush[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+12), WDFunc::StringValueWithCheck(Bd_block1.Tg_d[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+15), WDFunc::StringValueWithCheck(Bd_block1.dCbush[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+18), WDFunc::StringValueWithCheck(Bd_block1.dTg_d[i], 3));
    }

    WDFunc::SetLBLText(parent, "value21", WDFunc::StringValueWithCheck(Bd_block1.U0, 3));
    WDFunc::SetLBLText(parent, "value22", WDFunc::StringValueWithCheck(Bd_block1.U1, 3));
    WDFunc::SetLBLText(parent, "value23", WDFunc::StringValueWithCheck(Bd_block1.U2, 3));
    WDFunc::SetLBLText(parent, "value24", WDFunc::StringValueWithCheck(Bd_block1.I0, 3));
    WDFunc::SetLBLText(parent, "value25", WDFunc::StringValueWithCheck(Bd_block1.I1, 3));
    WDFunc::SetLBLText(parent, "value26", WDFunc::StringValueWithCheck(Bd_block1.I2, 3));
    WDFunc::SetLBLText(parent, "value27", WDFunc::StringValueWithCheck(Bd_block1.Iunb, 3));
    WDFunc::SetLBLText(parent, "value28", WDFunc::StringValueWithCheck(Bd_block1.Phy_unb, 3));
    //WDFunc::SetLBLText(parent, "value29", WDFunc::StringValueWithCheck(Bd_block1.Time, 3));


    if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
    {

        WDFunc::SetLBLText(parent, "value30", WDFunc::StringValueWithCheck(Bd_block1.Tamb, 3));
        WDFunc::SetLBLText(parent, "value30", WDFunc::StringValueWithCheck(Bd_block1.Tamb, 3));
    }

}

/*void Check_84::FillBd2(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "value21", WDFunc::StringValueWithCheck(Bd_block1.U0, 3));
    WDFunc::SetLBLText(parent, "value22", WDFunc::StringValueWithCheck(Bd_block1.U1, 3));
    WDFunc::SetLBLText(parent, "value23", WDFunc::StringValueWithCheck(Bd_block1.U2, 3));
    WDFunc::SetLBLText(parent, "value24", WDFunc::StringValueWithCheck(Bd_block1.I0, 3));
    WDFunc::SetLBLText(parent, "value25", WDFunc::StringValueWithCheck(Bd_block1.I1, 3));
    WDFunc::SetLBLText(parent, "value26", WDFunc::StringValueWithCheck(Bd_block1.I2, 3));
    WDFunc::SetLBLText(parent, "value27", WDFunc::StringValueWithCheck(Bd_block1.Iunb, 3));
    WDFunc::SetLBLText(parent, "value28", WDFunc::StringValueWithCheck(Bd_block1.Phy_unb, 3));
    WDFunc::SetLBLText(parent, "value29", WDFunc::StringValueWithCheck(Bd_block1.Time, 3));


    if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
    {
        WDFunc::SetLBLText(parent, "value30", WDFunc::StringValueWithCheck(Bd_block1.Tamb, 3));
    }
}*/

#if PROGSIZE != PROGSIZE_EMUL
void Check_84::SendErt(void)
{
   if(Commands::EraseTechBlock(5) == Error::ER_NOERROR)
   EMessageBox::information(this, "INFO", "Стёрто успешно");
   else
   EMessageBox::information(this, "INFO", "Ошибка стирания");

}
#endif

QWidget *Check_84::BdUI(int bdnum)
{
    Q_UNUSED(bdnum);
    return nullptr;
}
#if PROGSIZE != PROGSIZE_EMUL
void Check_84::SetDefaultValuesToWrite()
{

}

void Check_84::PrepareAnalogMeasurements()
{

}



void Check_84::ChooseValuesToWrite()
{

}

void Check_84::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);

}

void Check_84::PrepareHeadersForFile(int row)
{
     Q_UNUSED(row);

}

void Check_84::RefreshAnalogValues(int bdnum)
{
  Q_UNUSED(bdnum);
}
#endif
QWidget *Check_84::CustomTab()
{
    QWidget *w = new QWidget;

    return w;
}

void Check_84::EnterTg()
{
    int i;
    ask = new QDialog(this);
    //QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Задайте заводские значения tg δ вводов:");
    glyout->addWidget(lbl,0,0,1,6);

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Tg_d["+QString::number(i)+"]:");
        glyout->addWidget(lbl,1,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("Tg_zav."+QString::number(i));
        glyout->addWidget(ledit,2,i,1,1);
    }


    QPushButton *pb = new QPushButton("Рассчитать");
    connect(pb,SIGNAL(clicked()),this,SLOT(CalcTginit()));
    glyout->addWidget(pb,3,2,1,2);
   /* pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),this,SLOT(close()));
    connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
    glyout->addWidget(pb,9,3,1,3);*/

    ask->setLayout(glyout);
    ask->exec();

}

void Check_84::CalcTginit(void)
{
    float *tginit = new float[3];
    float tgzav[3];
    float tg_measurement[3];
    QString tmps;
    int i;

    for(i=0; i<3; i++)
    {
      WDFunc::LEData(ask, "Tg_zav."+QString::number(i), tmps);
      tgzav[i] = tmps.toFloat();
      ask->close();

      //WDFunc::SPBData(this, "Tg_zav."+QString::number(i), tgzav[i]);
      WDFunc::SPBData(this, "dTg_d"+QString::number(i), tg_measurement[i]);

      if(tgzav[i] != 0)
      {
         *(tginit+i) = tg_measurement[i] - tgzav[i];
      }
      else
      {
         EMessageBox::information(this, "Информация", "Задайте заводские значения");
         return;
      }
    }

    EMessageBox::information(this, "Информация", "Посчитано успешно");
    emit tgStart(tginit);

}

