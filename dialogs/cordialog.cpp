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
#include "../gen/timefunc.h"
#include "../dialogs/keypressdialog.h"
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
    first = 0;

    MessageTimer = new QTimer;
    MessageTimer->setInterval(5000);
    connect(MessageTimer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));

    for (i = 0; i < 3; i++)
    {
      CorBlock->C_init[i] = 0;
      CorBlock->Tg_init[i] = 0;
      CorBlock->corTg[i] = 0;
    }
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    MessageTimer->start();
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

    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения емкостей вводов, пФ:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, QString::number(4000+i), 0, 10000, 1, paramcolor), row,2+i,1,1);
    }


    row++;



    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов, %:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, QString::number(4003+i), -10, 10, 2, paramcolor), row,2+i,1,1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов,%:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, QString::number(4006+i), -10, 10, 2, paramcolor), row, 2+i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса, мА:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4009), 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса, град.:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4010), 0, 10000, 1, paramcolor), row,2,1,3);

    row++;

    //QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");

    pb = new QPushButton("Прочитать из модуля");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(GetCorBdButton()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,2);

    pb = new QPushButton("Записать в модуль");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteCorBd()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row,3,1,2);

    //row++;

    row++;

    pb = new QPushButton("Сбросить начальные значения");
    #if PROGSIZE != PROGSIZE_EMUL
        connect(pb,SIGNAL(clicked()),this,SLOT(ResetCor()));
    #endif
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,2);

    pb = new QPushButton("Задать начальные значения");
    #if PROGSIZE != PROGSIZE_EMUL
      connect(pb,SIGNAL(clicked()),this,SLOT(WriteCor()));
    #endif
    if (StdFunc::IsInEmulateMode())
       pb->setEnabled(false);

    glyout->addWidget(pb, row,3,1,2);

    //row++;




    row++;

    pb = new QPushButton("Прочитать значения из файла");
    #if PROGSIZE != PROGSIZE_EMUL
      connect(pb,SIGNAL(clicked()),this,SLOT(ReadFromFile()));
    #endif
    if (StdFunc::IsInEmulateMode())
       pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,2);

    pb = new QPushButton("Сохранить значения в файл");
    #if PROGSIZE != PROGSIZE_EMUL
      connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    #endif
    if (StdFunc::IsInEmulateMode())
       pb->setEnabled(false);

    glyout->addWidget(pb, row,3,1,2);

    //row++;



    //hlyout->addWidget(glyout,Qt::AlignTop);
    lyout->addLayout(glyout,Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);

}


void CorDialog::FillBackCor()
{
    int i;
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
    }
}


void CorDialog::FillCor()
{
   int i;

    WDFunc::SetSPBData(this, QString::number(4010), CorBlock->Phy_unb_init);
    WDFunc::SetSPBData(this, QString::number(4009), CorBlock->Iunb_init);

    for (i = 0; i < 3; i++)
    {

      //WDFunc::SetLEData(this, "C_init1."+QString::number(i), QString::number(CorBlock->C_init[i], 'f', 5));
      WDFunc::SetSPBData(this, QString::number(4000+i), CorBlock->C_init[i]);
      WDFunc::SetSPBData(this, QString::number(4003+i),CorBlock->Tg_init[i]);
      WDFunc::SetSPBData(this, QString::number(4006+i), CorBlock->corTg[i]);
    }
}

void CorDialog::GetCorBd(int index)
{
    if(index == corDIndex)
    {
       if(MainWindow::MainInterface.size() != 0)
       {
        if(MainWindow::MainInterface == "USB")
        {
            if(Commands::GetBd(7, CorBlock, sizeof(CorData)) == Error::ER_NOERROR)
            {
                FillCor();
                EMessageBox::information(this, "INFO", "Прочитано успешно");
            }
        }
        else if(MainWindow::MainInterface == "RS485")
        {
            information info;
            info.size = (sizeof(CorData)/4);
            info.adr = 4000;
            emit RS485ReadCorBd(&info);
        }
        else if(MainWindow::MainInterface == "Ethernet")
        {
            emit CorReadRequest();
        }
       }
    }
}
void CorDialog::GetCorBdButton()
{
    if(MainWindow::MainInterface.size() != 0)
    {
     if(MainWindow::MainInterface == "USB")
     {
       if(Commands::GetBd(7, CorBlock, sizeof(CorData)) == Error::ER_NOERROR)
       {
           FillCor();
           EMessageBox::information(this, "INFO", "Прочитано успешно");
       }
     }
     else if(MainWindow::MainInterface == "RS485")
     {
         information info;
         info.size = (sizeof(CorData)/4);
         info.adr = 4000;
         emit RS485ReadCorBd(&info);
     }
     else if(MainWindow::MainInterface == "Ethernet")
     {
         emit CorReadRequest();
     }
    }

}

void CorDialog::WriteCorBd()
{
    int i;
    quint16 adr[11] = {910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920};

    FillBackCor();

    if (WriteCheckPassword() == Error::ER_NOERROR)
    {
        if(MainWindow::MainInterface.size() != 0)
        {
         if(MainWindow::MainInterface == "Ethernet")
         {
            for(i = 0; i<11; i++)
            {
              emit sendCom50((adr+i), (((float*)CorBlock)+i));
              TimeFunc::Wait(300);
            }
         }
         else if(MainWindow::MainInterface == "RS485")
         {
             information info;
             info.size = (sizeof(CorData)/4);
             info.adr = adr[0];
             emit RS485WriteCorBd(&info, (float*)CorBlock);
         }
         else if(MainWindow::MainInterface == "USB")
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
    if (WriteCheckPassword() == Error::ER_NOERROR)
    {
        quint32 Com = 900;
        if(MainWindow::MainInterface.size() != 0)
        {
         if(MainWindow::MainInterface == "Ethernet")
         {
            emit sendCom45(&Com);
            EMessageBox::information(this, "INFO", "Задано успешно");
            emit CorReadRequest();
         }
         else if(MainWindow::MainInterface == "RS485")
         {
             information info;
             info.size = 1;
             info.adr = (quint16)Com;
             emit RS485WriteCorBd(&info, nullptr);
             EMessageBox::information(this, "INFO", "Задано успешно");
             info.size = (sizeof(CorData)/4);
             info.adr = 4000;
             emit RS485ReadCorBd(&info);
         }
         else if(MainWindow::MainInterface == "USB")
         {
            if(Commands::WriteCom(1) == Error::ER_NOERROR)   // задание общей коррекции
            {
               if(Commands::GetBd(7, CorBlock, sizeof(CorData)) == Error::ER_NOERROR)
               {
                  FillCor();
                  EMessageBox::information(this, "INFO", "Задано и прочитано успешно");
               }
            }
            else
            EMessageBox::information(this, "INFO", "Ошибка");
         }
        }
    }
}

void CorDialog::SetCor()
{
    if(MainWindow::MainInterface.size() != 0)
    {
     if(MainWindow::MainInterface == "Ethernet")
     {
        quint32 Com = 903;
        emit sendCom45(&Com);
     }
     else if(MainWindow::MainInterface == "USB")
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
    quint32 Com = 905;

    if (WriteCheckPassword() == Error::ER_NOERROR)
    {
        if(MainWindow::MainInterface.size() != 0)
        {
         if(MainWindow::MainInterface == "Ethernet")
         {
            emit sendCom45(&Com);
         }
         else if(MainWindow::MainInterface == "RS485")
         {
             information info;
             info.size = 1;
             info.adr = (quint16)Com;
             emit RS485WriteCorBd(&info, nullptr);
         }
         else if(MainWindow::MainInterface == "USB")
         {
            if(Commands::WriteCom(5) == Error::ER_NOERROR)
            EMessageBox::information(this, "INFO", "Сброшено успешно");
            else
            EMessageBox::information(this, "INFO", "Ошибка");

            if(Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == Error::ER_NOERROR)
            FillCor();
         }
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

    if(((Signal)->fl.SigAdr >= 4000) && ((Signal)->fl.SigAdr <= 4010) && first)
    {
      EMessageBox::information(this, "INFO", "Прочитано успешно");
    }


    for(i=0; i<Signal->SigNumber; i++)
    {
        sig = *(Signal+i);
        //if((Signal+i)->fl.SigAdr >= 1000 || (Signal+i)->fl.SigAdr <= 1009)
        FillBd(this, QString::number((Signal+i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));
    }
}

void CorDialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    float fl;
    QLocale german(QLocale::German);
    fl = german.toDouble(Value);
    WDFunc::SetSPBData(parent, Name, fl);
}

void CorDialog::ModBusUpdateCorData(ModBusSignal *Signal, int * size)
{

    //ModBusSignal sig = *new ModBusSignal;
    int i = 0;

    if(Signal != nullptr)
    {
        if(Signal->SigAdr == 4000)
        {
            for(i=0; i<*size; i++)
            {
              //sig = *(Signal+i);
              FillBd(this, QString::number((Signal+i)->SigAdr), WDFunc::StringValueWithCheck((Signal+i)->flVal));
            }
            ModBus::Reading = false;
            EMessageBox::information(this, "INFO", "Прочитано успешно");
        }
        else if(*size == 1)
        {
          EMessageBox::information(this, "INFO", "Записано успешно");
          ModBus::Reading = false;
        }

    }


}

void CorDialog::SaveToFile()
{
    int res = Error::ER_NOERROR;
    QByteArray ba;
    FillBackCor();
    ba.resize(sizeof(*CorBlock));
    memcpy(&(ba.data()[0]), CorBlock, sizeof(*CorBlock));
    res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.cor)", "cor"), ba, sizeof(*CorBlock));
    switch (res)
    {
    case Files::ER_NOERROR:
        EMessageBox::information(this, "Внимание", "Файл коэффициентов коррекции записан успешно!");
        break;
    case Files::ER_FILEWRITE:
        EMessageBox::error(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Files::ER_FILENAMEEMP:
        EMessageBox::error(this, "Ошибка", "Пустое имя файла!");
        break;
    case Files::ER_FILEOPEN:
        EMessageBox::error(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void CorDialog::ReadFromFile()
{
    QByteArray ba;
    ba.resize(sizeof(*CorBlock));

    int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.cor)"), ba);
    if (res != Files::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка при загрузке файла");
        return;
    }

    memcpy(CorBlock, &(ba.data()[0]), sizeof(*CorBlock));

    FillCor();
    EMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");

}


int CorDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(WritePasswordCheck(QString)));
    connect(this,SIGNAL(WritePasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

void CorDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

void CorDialog::TimerTimeout()
{
   MessageTimer->stop();
   first = 1;
}

void CorDialog::WriteCorMessageOk()
{
   EMessageBox::information(this, "Успешно", "Записано успешно!");
}

void CorDialog::ErrorRead()
{
  EMessageBox::information(this, "Ошибка", "Ошибка чтения");
}
