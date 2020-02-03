// check35.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "check35.h"
#include "../gen/colors.h"
#include "../widgets/wd_func.h"
#include "../widgets/emessagebox.h"
#include "../gen/error.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

Check35::Check35() : QDialog()
{


}

Check35::~Check35()
{


}

QWidget *Check35::Bd1W(QWidget *parent)
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

    row++;
    Button = new QPushButton("Подать команду на реле №4");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    glyout->addWidget(WDFunc::NewLBL(parent, "Разомкнуто", QString(ACONFOCLR), "lbl4"), row,2,1,1,Qt::AlignCenter|Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Rele4()));

    row++;
    Button = new QPushButton("Сбросить все реле");
    glyout->addWidget(Button, row,1,1,2,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Switch_off()));

    vlyout->addLayout(glyout);
    w->setLayout(vlyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *Check35::BdUI(int bdnum)
{
    Q_UNUSED(bdnum);
    return nullptr;
}

QWidget *Check35::CustomTab()
{
    QWidget *w = new QWidget;

    return w;
}

void Check35::Rele1()
{
   if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_35)
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

   if((ModuleBSI::GetMType(BoardTypes::BT_MEZONIN)) == Config::MTM_35)
   {
       if(rele1 != 1)
       {
        if(Commands::WriteCom(105) == Error::ER_NOERROR)   // включение
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
        if(Commands::WriteCom(125) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl1", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele1 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");
       }
   }
}

void Check35::Rele2()
{
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_35)
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

    if((ModuleBSI::GetMType(BoardTypes::BT_MEZONIN)) == Config::MTM_35)
    {
        if(rele2 != 1)
        {
         if(Commands::WriteCom(106) == Error::ER_NOERROR)   // включение
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
         if(Commands::WriteCom(126) == Error::ER_NOERROR)
         {
          WDFunc::SetLBLText(Fwidjet, "lbl2", "Разомкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele2 = 0;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
    }

}

void Check35::Rele3()
{
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_35)
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

    if((ModuleBSI::GetMType(BoardTypes::BT_MEZONIN)) == Config::MTM_35)
    {
        if(rele3 != 1)
        {
         if(Commands::WriteCom(107) == Error::ER_NOERROR)   // включение
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
         if(Commands::WriteCom(127) == Error::ER_NOERROR)
         {
          WDFunc::SetLBLText(Fwidjet, "lbl3", "Разомкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele3 = 0;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
    }

}

void Check35::Rele4()
{
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_35)
    {
        if(rele4 != 1)
        {
         if(Commands::WriteCom(104) == Error::ER_NOERROR)   // включение
         {
          WDFunc::SetLBLText(Fwidjet, "lbl4", "Замкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele4 = 1;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
        else  // выключение
        {
         if(Commands::WriteCom(124) == Error::ER_NOERROR)
         {
          WDFunc::SetLBLText(Fwidjet, "lbl4", "Разомкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele4 = 0;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
    }

    if((ModuleBSI::GetMType(BoardTypes::BT_MEZONIN)) == Config::MTM_35)
    {
        if(rele4 != 1)
        {
         if(Commands::WriteCom(108) == Error::ER_NOERROR)   // включение
         {
          WDFunc::SetLBLText(Fwidjet, "lbl4", "Замкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele4 = 1;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
        else  // выключение
        {
         if(Commands::WriteCom(128) == Error::ER_NOERROR)
         {
          WDFunc::SetLBLText(Fwidjet, "lbl4", "Разомкнуто", true);
          //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
          rele4 = 0;
         }
         else
         EMessageBox::information(Fwidjet, "INFO", "Ошибка");
        }
    }

}

void Check35::Switch_off()
{
    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_35)
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

        if(Commands::WriteCom(124) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl4", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele4 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");

    }

    if((ModuleBSI::GetMType(BoardTypes::BT_MEZONIN)) == Config::MTM_35)
    {
        if(Commands::WriteCom(125) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl1", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele1 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");

        if(Commands::WriteCom(126) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl2", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele2 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");

        if(Commands::WriteCom(127) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl3", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele3 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");

        if(Commands::WriteCom(128) == Error::ER_NOERROR)
        {
         WDFunc::SetLBLText(Fwidjet, "lbl4", "Разомкнуто", true);
         //EMessageBox::information(Fwidjet, "INFO", "Команда отправлена успешно");
         rele4 = 0;
        }
        else
        EMessageBox::information(Fwidjet, "INFO", "Ошибка");
    }

}
