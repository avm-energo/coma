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
#include "checkdialog84.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../config/config.h"

CheckDialog84::CheckDialog84(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QStringList sl;
    Ch84 = new Check_84;
    Ch = new Check;
//    BdNum = 11;

    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(1, &Ch84->Bd_block1, sizeof(Check_84::Bd1));

    //if(ModuleBSI::GetMType(BoardTypes::BT_BASE) == MTB_81)
   // sl = QStringList() << "Общ" << "In1" << "Temperature";
   // else
    sl = QStringList() << "Общ" << "In1" << "In2";
    BdUINum =sl.size();
    SetupUI(sl);
}


QWidget *CheckDialog84::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case 0:
        return Ch->Bd0W(this);
    case 1: // Блок #1
        return Ch84->Bd1W(this);
    case 2: // Блок #1
        return Ch84->Bd2W(this);

    default:
        return new QWidget;
    }
}

void CheckDialog84::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
        break;
    case C84_BDA_IN: // Блок #1
        Ch84->FillBd1(this);
        break;

    default:
        return;
    }
}

void CheckDialog84::PrepareHeadersForFile(int row)
{
    for (int i=0; i<3; i++)
    {
        xlsx->write(row,i+2,QVariant(("Ueff ф")+QString::number(i)+", кВ"));
        xlsx->write(row,i+5,QVariant("Ieff ф"+QString::number(i)+", А"));
        xlsx->write(row,i+8,QVariant("Cbush ф"+QString::number(i)+", град"));
        xlsx->write(row,i+11,QVariant("Tg_d ф"+QString::number(i)+", Вт"));
        xlsx->write(row,i+14,QVariant("dCbush ф"+QString::number(i)+", ВА"));
        xlsx->write(row,i+17,QVariant("dTg_d ф"+QString::number(i)+", ВА"));
    }
    xlsx->write(row,20,QVariant("U0"));
    xlsx->write(row,21,QVariant("U1"));
    xlsx->write(row,22,QVariant("U2"));
    xlsx->write(row,23,QVariant("I0"));
    xlsx->write(row,24,QVariant("I1"));
    xlsx->write(row,25,QVariant("I2"));
    xlsx->write(row,26,QVariant("Iunb, мА"));
    xlsx->write(row,27,QVariant("Phy_unb, град"));
    xlsx->write(row,28,QVariant("Tmk, °С"));

    if(ModuleBSI::GetMType(BoardTypes::BT_BASE) == MTB_81)
    xlsx->write(row,28,QVariant("Tamb, °С"));
}

void CheckDialog84::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
/*    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    for (int i=0; i<3; i++)
    {
        QString Precision = (ModuleBSI::MType(ModuleBSI::BT_MEZONIN) != MTM_81) ? "0.000" : "0.0000";
        format.setNumberFormat(Precision);
        xlsx->write(WRow,i+2,Bda_block.IUeff_filtered[i],format);

        Precision = (ModuleBSI::MType(ModuleBSI::BT_MEZONIN) != MTM_83) ? "0.0000" : "0.000";
        format.setNumberFormat(Precision);
        xlsx->write(WRow,i+5,Bda_block.IUeff_filtered[i+3],format);

        format.setNumberFormat("0.000");
        float Phi = (static_cast<float>(180)*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        xlsx->write(WRow,i+8,Phi,format);
        xlsx->write(WRow,i+11,Bda_block.Pf[i],format);
        xlsx->write(WRow,i+14,Bda_block.Qf[i],format);
        xlsx->write(WRow,i+17,Bda_block.Sf[i],format);
    }
    format.setNumberFormat("0.0000");
    xlsx->write(WRow,20,Bda_block.Frequency,format);
    format.setNumberFormat("0.0");
    xlsx->write(WRow,21,Bda_block.Tmk,format); */
}

QWidget *CheckDialog84::CustomTab()
{
    return nullptr;
}
