#include "cordialogktf.h"
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
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/colors.h"
#include "../gen/timefunc.h"
#include "../dialogs/keypressdialog.h"
#include "../usb/commands.h"


CorDialogKTF::CorDialogKTF(QWidget *parent) :
    AbstractCorDialog(parent)
{
    //int i;

    Bd9Block = new Bd9;
    WBd7Block = new WBd7;


    first = 0;

    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();

}

CorDialogKTF::~CorDialogKTF()
{

}

void CorDialogKTF::SetupUI()
{
    //QWidget *cp2 = new QWidget;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("cor");
    int row = 0;
    QString paramcolor = MAINWINCLR;
    QPushButton *pb = new QPushButton();

    glyout->addWidget(WDFunc::NewLBL(this, "Текущее значение ресурса изоляции:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(907), 0, 1000000, 5, paramcolor), row,2,1,2);
    row++;


    pb = new QPushButton("Прочитать из модуля");
        connect(pb,SIGNAL(clicked()),this,SLOT(GetCorBdButton()));
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,2);

    pb = new QPushButton("Записать в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteCorBd()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row,3,1,2);

    row++;


    pb = new QPushButton("Прочитать значения из файла");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadFromFile()));
    if (StdFunc::IsInEmulateMode())
       pb->setEnabled(false);

    glyout->addWidget(pb, row,1,1,2);

    pb = new QPushButton("Сохранить значения в файл");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    if (StdFunc::IsInEmulateMode())
       pb->setEnabled(false);

    glyout->addWidget(pb, row,3,1,2);


    lyout->addLayout(glyout,Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);

}



void CorDialogKTF::FillBackCor()
{

    WDFunc::SPBData(this, QString::number(907), WBd7Block->InitAge);

}


void CorDialogKTF::FillCor()
{

    WDFunc::SetSPBData(this, QString::number(907), Bd9Block->Age);



}

void CorDialogKTF::GetCorBd(int index)
{
    if(index == corDIndex)
    {
        if (MainInterface == I_USB)
        {
            if(Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == NOERROR)
            {
                FillCor();
                EMessageBox::information(this, "INFO", "Прочитано успешно");
            }
        }

        else if (MainInterface == I_ETHERNET)
        {
            emit CorReadRequest();
        }
    }
}
void CorDialogKTF::GetCorBdButton()
{
    if (MainInterface == I_USB)
    {
        if(Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == NOERROR)
        {
            FillCor();
            EMessageBox::information(this, "INFO", "Прочитано успешно");
        }
    }
    else if (MainInterface == I_RS485)
    {
        ModBus::Information info;
        info.size = (sizeof(Bd9)/4);
        info.adr = 4000;
        emit RS485ReadCorBd(info);
    }
    else if (MainInterface == I_ETHERNET)
    {
        emit CorReadRequest();
    }
}

void CorDialogKTF::WriteCorBd()
{
    //int i;
    quint32 adr = 907;

    FillBackCor();

    if (WriteCheckPassword() == NOERROR)
    {
        if(MainInterface == I_ETHERNET)
        {

               float corblocki;
               memcpy(&corblocki, reinterpret_cast<float *>(WBd7Block), sizeof(float));
               emit SendCom50(adr, corblocki);
               TimeFunc::Wait(300);

        }
        else if(MainInterface == I_RS485)
        {
            ModBus::Information info;
            info.size = (sizeof(WBd7)/4);
            info.adr = adr;
            emit RS485WriteCorBd(info, (float*)WBd7Block);
        }
        else if(MainInterface == I_USB)
        {
            if(Commands::WriteBd(7, WBd7Block, sizeof(WBd7)) == NOERROR)
            EMessageBox::information(this, "INFO", "Записано успешно");
            else
            EMessageBox::information(this, "INFO", "Ошибка");

            if(Commands::GetBd(7, WBd7Block, sizeof(WBd7Block)) == NOERROR)
            FillCor();
        }
    }
}

void CorDialogKTF::WriteCor()
{

}



void CorDialogKTF::ResetCor()
{


}






void CorDialogKTF::SaveToFile()
{
    int res = NOERROR;
    QByteArray ba;
    FillBackCor();
    ba.resize(sizeof(*WBd7Block));
    memcpy(&(ba.data()[0]), WBd7Block, sizeof(*WBd7Block));
    res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.cor)", "cor"), ba, sizeof(*WBd7Block));
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

void CorDialogKTF::ReadFromFile()
{
    QByteArray ba;
    ba.resize(sizeof(*Bd9Block));

    int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.cor)"), ba);
    if (res != Files::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка при загрузке файла");
        ERMSG("Ошибка при загрузке файла");
        return;
    }

  //  FillCor();
//    memcpy(Bd9Block, &(ba.data()[0]), sizeof(*Bd9Block));
       memcpy(&Bd9Block->Age, &(ba.data()[0]),sizeof(float) );
    FillCor();
    EMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");

}


