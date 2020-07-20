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
#include "fwupdialog.h"
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/files.h"
#include "../widgets/etableview.h"
#include "../widgets/waitwidget.h"
#include "../gen/s2.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/timefunc.h"
#include "../usb/commands.h"

fwupdialog::fwupdialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
   // fwupdialog* fwup = new fwupdialog;
    SetupUI();
}

void fwupdialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("fwp");

    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Записать ПО в память "+tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFW()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, 1,1,1,1);

    pb = new QPushButton("Перейти на новое ПО");
        connect(pb,SIGNAL(clicked()),this,SLOT(RunSoft()));
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

        glyout->addWidget(pb, 2,1,1,1);

    //hlyout->addWidget(glyout,Qt::AlignTop);
    lyout->addLayout(glyout,Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);

}

int fwupdialog::LoadFW()
{
    QByteArray ba;
    File_struct PV_file;
    //quint32 crc=0xFFFFFFFF;
//    quint32 i;
//    quint32 tmpi = 0;
    //void *Rptr = static_cast<void *>(&PV_file.Type);
    //tmpi = sizeof(PV_file.Type)+sizeof(PV_file.File.FileDatHeader);
    QVector<S2::DataRec> S2DR;

    if (WriteCheckPassword() == NOERROR)
    {
        int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Program Version (*.hex)"), ba);
        if (res != Files::ER_NOERROR)
        {
            WARNMSG("Ошибка файла ПО");
            return GENERALERROR;
        }

        ParseHexToS2(ba);

        //PV_file.File_xxx_header.size=sizeof(PV_file.Type) + sizeof(PV_file.File.FileDatHeader) + sizeof(ba) + sizeof(PV_file.void_recHeader);
    }
    return NOERROR;
}

void fwupdialog::RunSoft()
{
    if (WriteCheckPassword() == NOERROR)
    {
        TimeFunc::Wait(100);
        int res = Commands::RunVPO();
        if (res != NOERROR)
        {
            WARNMSG("Ошибка перехода на новое ПО");
            EMessageBox::information(this, "Ошибка", "Ошибка");
        }
        else
        {
            EMessageBox::information(this, "Успешно", "Переход на новое ПО выполнен успешно");
        }
    }
}

int fwupdialog::WriteCheckPassword()
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
        return GENERALERROR;
    if (!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void fwupdialog::WritePasswordCheck(QString psw)
{
    if (psw == "admin")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}


int fwupdialog::ParseHexToS2(QByteArray ba)
{

    File_struct* PV_file = new File_struct;

    //quint32 crc=0xFFFFFFFF;
    //quint32 tmpi = 0;
    //void *Rptr = static_cast<void *>(&PV_file.Type);
    //tmpi = sizeof(PV_file.Type)+sizeof(PV_file.File.FileDatHeader);
    QVector<S2::DataRec> S2DR;
    uint usize;
    int i, j = 0, k, h, p, size, copysize, iVal;
    QString str,st;
    QStringList sl;
    bool ok = false;
    //QByteArray* ForProcess = new QByteArray;
    MainSize = 0;
    QString binnumber, process;
    char byte = '\0';
    //QVector<QByteArray> * BaForSend = new QVector<QByteArray>;
    QByteArray* BaForSend = new QByteArray;

    str = ba;
    str.split(":");

    sl.append(str.split("\r\n:"));

    //c = QString::number(sl.at(0), 2);
    //st.append(QString("%1").arg(c, 4, 2, QChar('0')));
    //BaForSend.data()[0] = st.at(0).toLatin1();

    //st.append(QString("%1").arg(c, 4, 2, QChar('0')));

    BaForSend->resize(MAXSIZE);
    BaForSend->clear();

    for(i = 0; i<sl.size(); i++)
    {
        str = sl.at(i);

        if(i == 1)
        {
            st = QString("%1%2").arg(str.at(0)).arg(str.at(1));
            //st =  QString("%1").arg(st.toULongLong(&ok, 16), 0, 16, QChar('0'));
            copysize = size = st.toInt(&ok, 16);

            if(size)
            {
               //if(size == 10)
               //size=16;

               st = QString("%1%2").arg(str.at(6)).arg(str.at(7));
               if(st == "00")
               {
                   MainSize+=size;
                   for(j=0; j<size; j=j+2)
                   {//while(size)

                       binnumber.clear();
                       process.clear();
                       iVal = QString("%1%2").arg(str.at(8+j)).arg(str.at(9+j)).toInt(&ok, 16); //size
                       binnumber = st.setNum(iVal, 2);//.arg(iVal, 8, QChar('0'));  //st.setNum(iVal, 2).

                       byte = '\0';

                       if((binnumber.size()-1)<7)
                       {
                           //memcpy(&process.data()[7-binnumber.size()-1], &binnumber, binnumber.size());
                           for(k=0; k<(8 - binnumber.size()); k++)
                           process.append("0");

                           process.append(binnumber);
                           binnumber = process;
                       }

                       for(h=7, p=0; h>=0; h--, p++)
                       {

                              if(binnumber.at(h).toLatin1() & 1)
                              byte |= 0b00000001<<p;
                              //else
                              //byte &= 0b11111110<<p;



                       }

                     BaForSend->append(byte);
                     //size = size-2;
                   }
\
                   st = QString("%1%2").arg(str.at(14)).arg(str.at(15));

                   for(j=0; j<copysize; j=j+2)
                   {
                       binnumber.clear();
                       process.clear();
                       iVal = QString("%1%2").arg(str.at(16+j)).arg(str.at(17+j)).toInt(&ok, 16);
                       binnumber = st.setNum(iVal, 2);//.arg(iVal, 8, QChar('0'));  //st.setNum(iVal, 2).

                       byte = '\0';

                       if((binnumber.size()-1)<7)
                       {
                           //memcpy(&process.data()[7-binnumber.size()-1], &binnumber, binnumber.size());
                           for(k=0; k<(8 - binnumber.size()); k++)
                           process.append("0");

                           process.append(binnumber);
                           binnumber = process;
                       }

                       for(h=7, p=0; h>=0; h--, p++)
                       {

                              if(binnumber.at(h).toLatin1() & 1)
                              byte |= 0b00000001<<p;
                              //else
                              //byte &= 0b11111110<<p;



                       }

                     BaForSend->append(byte);
                     //copysize = copysize-2;
                   }
               }
            }

          //ba.data()[0] = QString("%1").arg(str.toULongLong(&ok, 16), 16, 2, QChar('0'));
         // i++;
          //u64 = str.toULongLong(&ok, 16);
          //memcpy(&ba.data()[0], &str, 16);
          // QString("%1").arg(c, 8, 2, QChar('0')
          //str = str.arg(str.toULongLong(&ok, 2), 16, 2, QChar('0'));
        }
        else
        {
            // st = str.at(1).toUpper() + str.at(2).toLower();
             st = QString("%1%2").arg(str.at(0)).arg(str.at(1));
             size = st.toInt(&ok, 16);

             if(size)
             {
                //if(size == 10)
                //size=16;

                st = QString("%1%2").arg(str.at(6)).arg(str.at(7));
                if(st == "00")
                {
                    MainSize+=size;
                    for(j=0; j<2*size; j=j+2)
                    {
                      binnumber.clear();
                      process.clear();
                      st = QString("%1%2").arg(str.at(8+j)).arg(str.at(9+j));  //, 4, QChar('0')
                      iVal = QString("%1%2").arg(str.at(8+j)).arg(str.at(9+j)).toInt(&ok, 16);
                      binnumber = st.setNum(iVal, 2);//.arg(iVal, 8, QChar('0'));  //st.setNum(iVal, 2).

                      byte = '\0';

                      if((binnumber.size()-1)<7)
                      {
                          //memcpy(&process.data()[7-binnumber.size()-1], &binnumber, binnumber.size());
                          for(k=0; k<(8 - binnumber.size()); k++)
                          process.append("0");

                          process.append(binnumber);
                          binnumber = process;
                      }

                      for(h=7, p=0; h>=0; h--, p++)
                      {

                             if(binnumber.at(h).toLatin1() & 1)
                             byte |= 0b00000001<<p;
                             //else
                             //byte &= 0b11111110<<p;

                      }


                      BaForSend->append(byte);

                    }
                }
             }

        }

    }


    BaForSend->resize(MainSize);

    PV_file->File.Data.clear();
    PV_file->File.Data.resize(MainSize-8);

    PV_file->Type.TypeHeader.id = 8000;
    PV_file->Type.TypeHeader.NumByte = 8;
    memcpy(&PV_file->Type.TypeTheData, &BaForSend->data()[0], 4);
    memcpy(&PV_file->Type.VerPO, &BaForSend->data()[4], 4);

    st.clear();
    st.append("Ver");
    for(i=3; i>=0; i--)
    st.append("."+QString::number(PV_file->Type.VerPO[i]));

    PV_file->File.FileDatHeader.id = 8001;
    usize = (MainSize-8);
    PV_file->File.FileDatHeader.NumByte = usize;
    //PV_file->File.Data = &BaForSend->data()[8];
    memcpy(&PV_file->File.Data.data()[0], &BaForSend->data()[8], usize);

    PV_file->void_recHeader.id = 0xFFFFFFFF;
    PV_file->void_recHeader.NumByte = 0;

    S2DR.append({PV_file->Type.TypeHeader.id, PV_file->Type.TypeHeader.NumByte, &PV_file->Type.TypeTheData});
    S2DR.append({PV_file->File.FileDatHeader.id, PV_file->File.FileDatHeader.NumByte,  &PV_file->File.Data.data()[0]});  //BaForSend->data_ptr()
    S2DR.append({PV_file->void_recHeader.id, PV_file->void_recHeader.NumByte, nullptr});

    /*ForProcess->clear();
    ForProcess->resize(MAXSIZE);

    memcpy(&ForProcess->data()[0], &PV_file.Type.TypeHeader.id, 24);
    memcpy(&ForProcess->data()[24], &BaForSend->data()[8], (BaForSend->size()-8));
    ForProcess->resize((BaForSend->size()+16));
    //BaForSend->resize(ForProcess->size());
    memcpy(&BaForSend->data()[0], &ForProcess->data()[0], (BaForSend->size()+16));*/

    if (Commands::WriteFile(3, &S2DR) != NOERROR)
    {
        EMessageBox::information(this, "Ошибка", "Ошибка записи в модуль!");
        return GENERALERROR;
    }
    EMessageBox::information(this, "Успешно", "Загрузка ПО версии "+st+" прошла успешно!");
    return NOERROR;

   return NOERROR;
}
