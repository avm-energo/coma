#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "checkdialog31.h"
#include "../widgets/emessagebox.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

CheckDialog31::CheckDialog31(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ch31 = new Check31;
    Ch = new Check;
    BdUINum = 1; // количество вкладок - 1
    int StartBd = (board == BoardTypes::BT_BASE) ? BT_STARTBD_BASE : BT_STARTBD_MEZ; // стартовый номер блока данных - 1 для базовой платы, 101 - для мезонинной
    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
//    SetBd(StartBd + A31_BD, &Ch31->Bd_block, sizeof(Check31::Bd));
    QStringList sl = QStringList() << "Общ" << "Все";
    SetupUI(sl);
#if PROGSIZE != PROGSIZE_EMUL
    timer->setInterval(ANMEASINT);
    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);
    connect(BdaTimer,SIGNAL(timeout()),this,SLOT(BdaTimerTimeout()));
#endif
}

QWidget *CheckDialog31::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        return Ch->Bd0W(this);
    default:
        return new QWidget;
    }
}
#if PROGSIZE != PROGSIZE_EMUL
void CheckDialog31::SetDefaultValuesToWrite()
{

}

void CheckDialog31::PrepareAnalogMeasurements()
{

}

void CheckDialog31::ChooseValuesToWrite()
{

}

void CheckDialog31::WriteToFile(int row, int bdnum)
{
    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    QString Precision = "0.0000";
    format.setNumberFormat(Precision);
    switch (bdnum)
    {
    case 0:
        xlsx->write(row,2+A31_NUMCH,WDFunc::FloatValueWithCheck(Ch->Bd_block0.Tmk), format);
        xlsx->write(row,3+A31_NUMCH,WDFunc::FloatValueWithCheck(Ch->Bd_block0.Vbat), format);
        break;
    }
}

void CheckDialog31::PrepareHeadersForFile(int row)
{
    for (int i=0; i<A31_NUMCH; ++i)
        xlsx->write(row,i+2,QVariant(("D")+QString::number(i+10, 36)));
    xlsx->write(row, 2+A31_NUMCH,QVariant("t, град"));
    xlsx->write(row, 3+A31_NUMCH, QVariant("Ubat, В"));
}

void CheckDialog31::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
        break;
    default:
        return;
    }
}
#endif
QWidget *CheckDialog31::CustomTab()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(Ch31->BdaW(this));
    QPushButton *pb = new QPushButton("Начать измерения Bda");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StartBdaMeasurements()));
#endif
    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения Bda");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StopBdaMeasurements()));
#endif
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}

void CheckDialog31::StartBdaMeasurements()
{
    BdaTimer->start();
}

void CheckDialog31::StopBdaMeasurements()
{
    BdaTimer->stop();
}

void CheckDialog31::BdaTimerTimeout()
{
    if (Commands::GetBda(Board, &Ch31->Bda_block, sizeof(Check31::Bda)) == Error::ER_NOERROR)
        Ch31->FillBda(this);
}
