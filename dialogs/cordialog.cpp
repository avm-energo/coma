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
    QWidget *cp2 = new QWidget;
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
     glyout->addWidget(WDFunc::NewSPB(this, "C_init1."+QString::number(i), 0, 10000, 1, paramcolor), row,2+i,1,1);

     /*QLineEdit *le = new QLineEdit("");
     le->setObjectName("C_init1."+QString::number(i));
     le->setStyleSheet(ValuesLEFormat);
     glyout->addWidget(le,row,2+i,1,1);*/
    }

    /*pb = new QPushButton("Задать начальные значения емкостей");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(WriteCorCapacity()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,5,1,1);*/

    row++;



    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Tg_init1."+QString::number(i), -10, 10, 2, paramcolor), row,2+i,1,1);
    }

    /*pb = new QPushButton("Задать начальные значения тангенсов");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(WriteCorTg()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,5,1,1);*/
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "corTg1."+QString::number(i), -10, 10, 2, paramcolor), row, 2+i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Iunb_init1", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Phy_unb_init1", 0, 10000, 1, paramcolor), row,2,1,3);


    /*pb = new QPushButton("Задать начальные значения небаланса");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(WriteCorNotBalance()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,5,1,1);*/

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
        connect(pb,SIGNAL(clicked()),this,SLOT(GetCorBd()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,5);

    row++;


    pb = new QPushButton("Задать коррекцию");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(SetCor()));
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
      connect(pb,SIGNAL(clicked()),this,SLOT(WriteCorNotBalance()));
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
    int i;
    QString tmps;

    WDFunc::SPBData(this, "Phy_unb_init1", CorBlock->Phy_unb_init);
    WDFunc::SPBData(this, "Iunb_init1", CorBlock->Iunb_init);

    for (i = 0; i < 3; i++)
    {
      WDFunc::SPBData(this, "C_init1."+QString::number(i), CorBlock->C_init[i]);

      //WDFunc::LEData(this, "C_init1."+QString::number(i), tmps);
      //CorBlock->C_init[i]=ToFloat(tmps);
      WDFunc::SPBData(this, "Tg_init1."+QString::number(i), CorBlock->Tg_init[i]);
      WDFunc::SPBData(this, "corTg1."+QString::number(i), CorBlock->corTg[i]);
    }
}


void CorDialog::FillCor()
{
    int i;

    WDFunc::SetSPBData(this, "Phy_unb_init1", CorBlock->Phy_unb_init);
    WDFunc::SetSPBData(this, "Iunb_init1", CorBlock->Iunb_init);

    for (i = 0; i < 3; i++)
    {

      //WDFunc::SetLEData(this, "C_init1."+QString::number(i), QString::number(CorBlock->C_init[i], 'f', 5));
      WDFunc::SetSPBData(this, "C_init1."+QString::number(i), CorBlock->C_init[i]);
      WDFunc::SetSPBData(this, "Tg_init1."+QString::number(i),CorBlock->Tg_init[i]);
      WDFunc::SetSPBData(this, "corTg1."+QString::number(i), CorBlock->corTg[i]);
    }
}

void CorDialog::GetCorBd()
{
    if(Commands::GetBd(7, CorBlock, sizeof(CorData)) == Error::ER_NOERROR)
    FillCor();
}

void CorDialog::WriteCorBd()
{

    FillBackCor();
    if(Commands::WriteBd(7, CorBlock, sizeof(CorData)) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");

}


void CorDialog::WriteCorCapacity()
{
    if(Commands::WriteCom(1) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");

}


void CorDialog::WriteCorTg()
{
    if(Commands::WriteCom(2) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");

}

void CorDialog::WriteCorNotBalance()
{
    if(Commands::WriteCom(1) == Error::ER_NOERROR)   // задание общей коррекции
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");

}

void CorDialog::SetCor()
{
    if(Commands::WriteCom(4) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");

}

void CorDialog::ResetCor()
{
    if(Commands::WriteCom(5) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");

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
