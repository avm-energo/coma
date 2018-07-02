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
#include "checkdialog21.h"
#include "../widgets/emessagebox.h"
#include "../gen/error.h"
#include "../gen/commands.h"
#include "../widgets/wd_func.h"

CheckDialog21::CheckDialog21(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ch21 = new Check21;
    Ch = new Check;
//    BdNum = 2; // количество блоков данных 1
    BdUINum = 2; // количество вкладок - 1
    int StartBd = (board == BoardTypes::BT_BASE) ? BT_STARTBD_BASE : BT_STARTBD_MEZ; // стартовый номер блока данных - 1 для базовой платы, 101 - для мезонинной
    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(StartBd + A21_BD, &Ch21->Bd_block, sizeof(Check21::Bd1));
    QStringList sl = QStringList() << "Общ" << "Все";
    SetupUI(sl);
#if PROGSIZE != PROGSIZE_EMUL
    timer->setInterval(ANMEASINT);
    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);
    connect(BdaTimer,SIGNAL(timeout()),this,SLOT(BdaTimerTimeout()));
#endif
}

QWidget *CheckDialog21::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        return Ch->Bd0W(this);
    case A21_BD: // Блок #1
        return Ch21->Bd1W(this);
    default:
        return new QWidget;
    }
}
#if PROGSIZE != PROGSIZE_EMUL
void CheckDialog21::SetDefaultValuesToWrite()
{

}

void CheckDialog21::PrepareAnalogMeasurements()
{

}

void CheckDialog21::StartBdaMeasurements()
{
    BdaTimer->start();
}

void CheckDialog21::StopBdaMeasurements()
{
    BdaTimer->stop();
}

void CheckDialog21::BdaTimerTimeout()
{
    if (Commands::GetBda(Board, &Ch21->Bda_block, sizeof(Check21::Bda)) == Error::ER_NOERROR)
        Ch21->FillBda(this);
}

void CheckDialog21::ChooseValuesToWrite()
{

}

void CheckDialog21::WriteToFile(int row, int bdnum)
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
        for (int i=0; i<AIN21_NUMCH; ++i)
        {
            xlsx->write(row,i+2,WDFunc::FloatValueWithCheck(Ch21->Bd_block.inI[i]), format);
            xlsx->write(row,i+2+AIN21_NUMCH,WDFunc::FloatValueWithCheck(Ch21->Bd_block.inU[i]), format);
        }
    }
    }
}

void CheckDialog21::PrepareHeadersForFile(int row)
{
    for (int i=0; i<AIN21_NUMCH; ++i)
    {
        xlsx->write(row,i+2,QVariant(("Ain")+QString::number(i+10, 36)+", мА"));
        xlsx->write(row,i+2+AIN21_NUMCH,QVariant(("Ain")+QString::number(i+10, 36)+", В"));
    }
    xlsx->write(row, 2+AIN21_NUMCH*2,QVariant("t, град"));
    xlsx->write(row, 3+AIN21_NUMCH*2, QVariant("Ubat, В"));
}

void CheckDialog21::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
    case 1: // Блок #1
        Ch21->FillBd1W(this);
    default:
        return;
    }
}
#endif
QWidget *CheckDialog21::CustomTab()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(Ch21->BdaW(this));
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
