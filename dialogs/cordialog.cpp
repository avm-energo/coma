#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include <QFileDialog>
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/files.h"
#include "../widgets/etableview.h"
#include "../gen/s2.h"
#include "../dialogs/cordialog.h"
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/colors.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif


CorDialog::CorDialog(QWidget *parent) :
    QDialog(parent)
{
    int i;

    CorBlock = new CorData;
    CorBlock->Phy_unb_init = 0;
    CorBlock->Iunb_init = 0;

    for (i = 0; i < 3; i++)
    {
      CorBlock->C_init[i] = 0;
      CorBlock->Tg_init[i] = 0;
      CorBlock->corTg[i] = 0;
    }
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

CorDialog::~CorDialog()
{

}

void CorDialog::SetupUI()
{
    //QWidget *cp2 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("cor");
    int row = 0;
    QString paramcolor = MAINWINCLR;
    QPushButton *pb = new QPushButton();

    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения емкостей вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, QString::number(4000+i), 0, 10000, 1, paramcolor), row,2+i,1,1);
    }


    row++;



    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, QString::number(4003+i), -10, 10, 2, paramcolor), row,2+i,1,1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, QString::number(4006+i), -10, 10, 2, paramcolor), row, 2+i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4009), 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4010), 0, 10000, 1, paramcolor), row,2,1,3);

    row++;

    //QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");

    pb = new QPushButton("Записать в модуль");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteCorBd()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,5);

    row++;

    pb = new QPushButton("Прочитать из модуля");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(GetCorBdButton()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,5);

    row++;

    pb = new QPushButton("Сбросить коррекцию");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(ResetCor()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,5);

    row++;

    pb = new QPushButton("Задать начальные значения");
    #if PROGSIZE != PROGSIZE_EMUL
      connect(pb,SIGNAL(clicked()),this,SLOT(WriteCor()));
    #endif
    if (StdFunc::IsInEmulateMode())
       pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,5);



    //hlyout->addWidget(glyout,Qt::AlignTop);
    lyout->addLayout(glyout,Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);

}


void CorDialog::FillBackCor()
{
    /*int i;
    QString tmps;

    WDFunc::SPBData(this, QString::number(4010), CorBlock->Phy_unb_init);
    WDFunc::SPBData(this, QString::number(4009), CorBlock->Iunb_init);

    for (i = 0; i < 3; i++)
    {
      WDFunc::SPBData(this, QString::number(4000+i), CorBlock->C_init[i]);

      //WDFunc::LEData(this, "C_init1."+QString::number(i), tmps);
      //CorBlock->C_init[i]=ToFloat(tmps);
      WDFunc::SPBData(this, QString::number(4003+i), CorBlock->Tg_init[i]);
      WDFunc::SPBData(this, QString::number(4006+i), CorBlock->corTg[i]);
    }*/
}


void CorDialog::FillCor()
{
   /* int i;

    WDFunc::SetSPBData(this, QString::number(4010), CorBlock->Phy_unb_init);
    WDFunc::SetSPBData(this, QString::number(4009), CorBlock->Iunb_init);

    for (i = 0; i < 3; i++)
    {

      //WDFunc::SetLEData(this, "C_init1."+QString::number(i), QString::number(CorBlock->C_init[i], 'f', 5));
      WDFunc::SetSPBData(this, QString::number(4000+i), CorBlock->C_init[i]);
      WDFunc::SetSPBData(this, QString::number(4003+i),CorBlock->Tg_init[i]);
      WDFunc::SetSPBData(this, QString::number(4006+i), CorBlock->corTg[i]);
    }*/
}

void CorDialog::GetCorBd(int index)
{
    if(index == corDIndex)
    {
       if(MainWindow::interface.size() != 0)
       {
        if(MainWindow::interface == "USB")
        {
            if(Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == Error::ER_NOERROR)
            FillCor();
        }
       }
    }
}
void CorDialog::GetCorBdButton()
{
    if(MainWindow::interface.size() != 0)
    {
     if(MainWindow::interface == "USB")
     {
       if(Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == Error::ER_NOERROR)
       FillCor();
     }
    }
}

void CorDialog::WriteCorBd()
{
    int i;
    quint16 adr[11] = {910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920};

    FillBackCor();

    if(MainWindow::interface.size() != 0)
    {
     if(MainWindow::interface == "Ethernet и RS485")
     {
        for(i = 0; i<11; i++)
        {
          emit sendCom50((adr+i), (float*)(CorBlock+i));
        }
     }
     else
     {
         if(Commands::WriteBd(7, CorBlock, sizeof(CorData)) == Error::ER_NOERROR)
         EMessageBox::information(this, "INFO", "Записано успешно");
         else
         EMessageBox::information(this, "INFO", "Ошибка");

         if(Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == Error::ER_NOERROR)
         FillCor();
     }
    }

}


void CorDialog::WriteCorCapacity()
{
    quint32 Com = 900;
    emit sendCom45(&Com);
}


void CorDialog::WriteCorTg()
{
    quint32 Com = 901;
    emit sendCom45(&Com);
    /*if(Commands::WriteCom(2) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");*/
}

void CorDialog::WriteCor()
{
    if(MainWindow::interface.size() != 0)
    {
     if(MainWindow::interface == "Ethernet и RS485")
     {
        quint32 Com = 900;
        emit sendCom45(&Com);
     }
     else if(MainWindow::interface == "USB")
     {
        if(Commands::WriteCom(1) == Error::ER_NOERROR)   // задание общей коррекции
        EMessageBox::information(this, "INFO", "Записано успешно");
        else
        EMessageBox::information(this, "INFO", "Ошибка");

     }
    }
}

void CorDialog::SetCor()
{
    if(MainWindow::interface.size() != 0)
    {
     if(MainWindow::interface == "Ethernet и RS485")
     {
        quint32 Com = 903;
        emit sendCom45(&Com);
     }
     else if(MainWindow::interface == "USB")
     {
        if(Commands::WriteCom(4) == Error::ER_NOERROR)
        EMessageBox::information(this, "INFO", "Записано успешно");
        else
        EMessageBox::information(this, "INFO", "Ошибка");
     }
    }

}

void CorDialog::ResetCor()
{
    if(MainWindow::interface.size() != 0)
    {
     if(MainWindow::interface == "Ethernet и RS485")
     {
        quint32 Com = 905;
        emit sendCom45(&Com);
     }
     else if(MainWindow::interface == "USB")
     {
        if(Commands::WriteCom(5) == Error::ER_NOERROR)
        EMessageBox::information(this, "INFO", "Записано успешно");
        else
        EMessageBox::information(this, "INFO", "Ошибка");

        if(Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == Error::ER_NOERROR)
        FillCor();
     }
    }

}

float CorDialog::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        ERMSG("Значение "+text+" не может быть переведено во float");
        return 0;
    }
    return tmpf;
}

void CorDialog::MessageOk()
{
  EMessageBox::information(this, "INFO", "Записано успешно");
}

void CorDialog::UpdateFlCorData(Parse104::FlSignals104 *Signal)
{
    Parse104::FlSignals104 sig = *new Parse104::FlSignals104;
    int i;
    for(i=0; i<Signal->SigNumber; i++)
    {
        sig = *(Signal+i);
        //WDFunc::SetLBLText(Ch, QString::number((Signal+i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));
        //if((Signal+i)->fl.SigAdr == 101 || (Signal+i)->fl.SigAdr == 102)
        //Ch->FillBd0(this, QString::number((Signal+i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));

        //if((Signal+i)->fl.SigAdr >= 1000 || (Signal+i)->fl.SigAdr <= 1009)
        FillBd(this, QString::number((Signal+i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));
    }
}

void CorDialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetSPBData(parent, Name, Value.toFloat());
}