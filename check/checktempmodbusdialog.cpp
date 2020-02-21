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
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QStringList sl;
    connectionStateRTU = false;
    sl = QStringList() << "Датчик температуры";
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

    default:
        return new QWidget;
    }
}

QWidget *checktempmodbusdialog::Bd1W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;


    glyout->addWidget(WDFunc::NewLBL(parent, "Ramb"), 0,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(4502), ValuesFormat, \
                                      QString::number(1)+".Сопротивление датчика, Ом"),1,0,1,1);
    /*glyout->addWidget(WDFunc::NewLBL(parent, "Time"),6,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "value29", ValuesFormat, \
                                      QString::number(29)+".Время записи, с"),7,0,1,1);*/



    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void checktempmodbusdialog::onModbusStateChanged(QModbusDevice::State* state)
{
    if(*state == QModbusDevice::ConnectedState)
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
/*void checktempmodbusdialog::UpdateModBusData(ModBus::Signal* Signal)
{
    Parse104::BS104Signals sig = *new Parse104::BS104Signals;
    sig = *Signal;
    ModuleBSI::Bsi* Bsi = new ModuleBSI::Bsi;
    int i;

    for(i=0; i<Signal->SigNumber; i++)
    {
        sig = *(Signal+i);

        Bsi->Hth = sig.BS.SigVal;
        if(sig.BS.SigAdr == 15)
        {
           emit BsiRefresh(Bsi);
        }
    }
}*/
