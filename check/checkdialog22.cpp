#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QCoreApplication>
#include <QFileDialog>
#include <QDateTime>
#include "checkdialog22.h"
#include "../widgets/emessagebox.h"
//#include "../gen/publicclass.h"
#include "../gen/error.h"
#include "../gen/commands.h"
#include "../widgets/wd_func.h"

CheckDialog22::CheckDialog22(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ch22 = new Check22;
    Ch = new Check;
    BdUINum = 2; // количество вкладок - 1
    int StartBd = (board == BoardTypes::BT_BASE) ? BT_STARTBD_BASE : BT_STARTBD_MEZ; // стартовый номер блока данных - 1 для базовой платы, 101 - для мезонинной
    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(StartBd + A22_BD, &Ch22->Bd_block, sizeof(Check22::Bd1));
    QStringList sl = QStringList() << "Общ" << "Все";
    SetupUI(sl);
#if PROGSIZE != PROGSIZE_EMUL
    timer->setInterval(ANMEASINT);
    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);
    connect(BdaTimer,SIGNAL(timeout()),this,SLOT(BdaTimerTimeout()));
#endif
}

QWidget *CheckDialog22::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        return Ch->Bd0W(this);
    case A22_BD: // Блок #1
        return Ch22->Bd1W(this);
    default:
        return new QWidget;
    }
}

#if PROGSIZE != PROGSIZE_EMUL
void CheckDialog22::SetDefaultValuesToWrite()
{

}

void CheckDialog22::PrepareAnalogMeasurements()
{

}

void CheckDialog22::StartBdaMeasurements()
{
    BdaTimer->start();
}

void CheckDialog22::StopBdaMeasurements()
{
    BdaTimer->stop();
}

void CheckDialog22::BdaTimerTimeout()
{
    if (Commands::GetBda(Board, &Ch22->Bda_block, sizeof(Check22::Bda)) == Error::ER_NOERROR)
        Ch22->FillBda(this);
}

void CheckDialog22::ChooseValuesToWrite()
{

}

void CheckDialog22::WriteToFile(int row, int bdnum)
{
    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    QString Precision = "0.0000";
    format.setNumberFormat(Precision);
    switch (bdnum)
    {
    case 0:
        xlsx->write(row,2+AIN21_NUMCH*2,WDFunc::FloatValueWithCheck(Ch->Bd_block0.Tmk), format);
        xlsx->write(row,3+AIN21_NUMCH*2,WDFunc::FloatValueWithCheck(Ch->Bd_block0.Vbat), format);
    case 1:
    {
        for (int i=0; i<AIN22_NUMCH; ++i)
        {
            xlsx->write(row,i+2,WDFunc::FloatValueWithCheck(Ch22->Bd_block.Dmed[i]), format);
            xlsx->write(row,i+2+AIN22_NUMCH,WDFunc::FloatValueWithCheck(Ch22->Bd_block.Tmed[i]), format);
        }
    }
    }
}

void CheckDialog22::PrepareHeadersForFile(int row)
{
    for (int i=0; i<AIN22_NUMCH; ++i)
    {
        xlsx->write(row,i+2,QVariant(("Tin")+QString::number(i+10, 36)+", °C"));
        xlsx->write(row,i+2+AIN22_NUMCH,QVariant(("Tin")+QString::number(i+10, 36)+", Ом"));
    }
    xlsx->write(row, 2+AIN22_NUMCH*2,QVariant("t, град"));
    xlsx->write(row, 3+AIN22_NUMCH*2, QVariant("Ubat, В"));
}

void CheckDialog22::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
    case 1: // Блок #1
        Ch22->FillBd1W(this);
    default:
        return;
    }
}
#endif
QWidget *CheckDialog22::CustomTab()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(Ch22->BdaW(this));
    QPushButton *pb = new QPushButton("Начать измерения Bda");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StartBdaMeasurements()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения Bda");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StopBdaMeasurements()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}
