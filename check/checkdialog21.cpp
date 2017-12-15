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
#include "../gen/publicclass.h"
#include "../gen/commands.h"

CheckDialog21::CheckDialog21(QWidget *parent) : EAbstractCheckDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ch21 = new Check21;
    Ch = new Check;
    BdNum = 2; // количество блоков данных 1
    BdUINum = 2; // количество вкладок - 1
    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(A21_BD, &Ch21->Bd_block, sizeof(Check21::Bd1));
    QStringList sl = QStringList() << "Общ" << "Все";
    SetupUI(sl);
    timer->setInterval(ANMEASINT);
    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);
    connect(BdaTimer,SIGNAL(timeout()),this,SLOT(BdaTimerTimeout()));
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
    if (Commands::GetBda(BT_NONE, &Ch21->Bda_block, sizeof(Check21::Bda)) == NOERROR)
        Ch21->FillBda(this);
}

void CheckDialog21::ChooseValuesToWrite()
{

}

void CheckDialog21::WriteToFile(int row, int bdnum)
{

}

void CheckDialog21::PrepareHeadersForFile(int row)
{

}

void CheckDialog21::RefreshAnalogValues(int bdnum)
{

}

/*void CheckDialog21::StartMeasurementsWithFile()
{
    aconf = new config_21;
    // читаем текущую конфигурацию
    cn->Send(CN_GF,Canal::BT_NONE,NULL,0,1,aconf->Config->Config);
    if (cn->result != NOERROR)
    {
        ERMSG("Ошибка приёма данных конфигурации");
        return;
    }
    QString Filename = QFileDialog::getSaveFileName(this,"Сохранить данные","","Excel files (*.xlsx)");
    if (Filename == "")
    {
        ERMSG("Не задано имя файла");
        return; // !!! ошибка - не задано имя файла
    }
    XlsxWriting = true;
    xlsx = new QXlsx::Document(Filename);
    xlsx->write(1,1,QVariant("Модуль: "+pc.ModuleTypeString+" сер. ном. "+QString::number(pc.ModuleBsi.SerialNum,10)));
    xlsx->write(2,1,QVariant("Дата начала записи: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3,1,QVariant("Время начала записи: "+QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5,1,QVariant("Дата и время отсчёта"));
    for (int i=0; i<16; i++)
    {
        if (aconf->Config->Bci_block.in_type[i] == Config2x::AIT_MA)
            xlsx->write(5,i+3,"I"+QString::number(i)+", мА");
        else if (aconf->Config->Bci_block.in_type[i] == Config2x::AIT_V)
            xlsx->write(5,i+3,"U"+QString::number(i)+", В");
        else
            continue;
    }
    xlsx->write(5,2,QVariant("t, град"));
    WRow = 6;
    QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    pb = this->findChild<QPushButton *>("pbmeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    ElapsedTimeCounter = new QTime;
    ElapsedTimeCounter->start();
    StartMeasurements();
} */

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
