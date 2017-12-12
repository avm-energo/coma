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
    BdMeasurementsActive = BdaMeasurementsActive = false;
    setAttribute(Qt::WA_DeleteOnClose);
//    Check21 *Ch21 = new Check21;
/*    for (int i = 0; i < 16; i++)
    {
        Bda_block.sin[i] = 0;
        Bd_block.inI[i] = 0;
        Bd_block.inU[i] = 0;
        Bd_block.ain[i] = 0;
        Bd_block.ainn[i] = 0;
        Bd_block.ainx[i] = 0;
    }
    Bd_block.at = 0; */
    SetupUI();
}

void CheckDialog21::SetupUI()
{
    /*int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QVBoxLayout *gb1lyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;
    CheckTW->addTab(cp1,"Автоматическая проверка");
    CheckTW->addTab(cp2,"Текущие измерения");
    CheckTW->addTab(cp3,"Прочее");

    QPushButton *pb = new QPushButton("Запустить измерения");
    pb->setObjectName("pbmeasurements");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMeasurements()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Запустить измерения с записью в файл");
    pb->setObjectName("pbfilemeasurements");
    if (pc.Emul)
        pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMeasurementsWithFile()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMeasurements()));
    gb1lyout->addWidget(pb);
    lyout->addLayout(gb1lyout);
    cp2->setLayout(lyout);

    gb2lyout = new QHBoxLayout;
    pb = new QPushButton("Запустить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartBdaMeasurements()));
    gb2lyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopBdaMeasurements()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройки");
    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Период обновления данных измерения, сек:");
    gb2lyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("500");
    rb->setText("0,5");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    gb2lyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("2000");
    rb->setText("2");
    rb->setChecked(true);
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    gb2lyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("10000");
    rb->setText("10");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    gb2lyout->addWidget(rb);
    gb->setLayout(gb2lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Проверки");
    gb1lyout = new QVBoxLayout;

    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Текущий IP-адрес:");
    gb2lyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("ipl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    pb = new QPushButton("Обновить");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetIP()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);

    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Контроль 1PPS:");
    gb2lyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("ppsl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    pb = new QPushButton("Обновить");
    connect(pb,SIGNAL(clicked()),this,SLOT(Check1PPS()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);

    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);
    cp3->setLayout(lyout);

    lyout = new QVBoxLayout;

    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка светодиодного индикатора...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("aledl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout = new QHBoxLayout;
    lbl=new QLabel("Проверка каналов измерения...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("ainl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка настроечных параметров...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("asetl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка контактов IP...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("aipl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка приёма сигнала 1PPS...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("appsl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    lyout->addStretch(99);
    cp1->setLayout(lyout);
    lyout = new QVBoxLayout;
    lyout->addWidget(CheckTW);
    setLayout(lyout); */
}

void CheckDialog21::SetDefaultValuesToWrite()
{

}

void CheckDialog21::PrepareAnalogMeasurements()
{

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

void CheckDialog21::StartBdaMeasurements()
{
    BdaMeasurementsActive = true;
}

void CheckDialog21::StopBdaMeasurements()
{
    BdaMeasurementsActive = false;
}


/*void CheckDialog21::TimerTimeout()
{
    if ((BdMeasurementsActive) && (OddTimeout)) // текущие измерения проводим на первом проходе таймера
    {
        cn->Send(CN_GBd, Canal::BT_NONE, &Bd_block, sizeof(Bd));
        if (cn->result == NOERROR)
            RefreshBd();
        if (XlsxWriting)
        {
            QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
            if (pb != 0)
            {
                int MSecs = ElapsedTimeCounter->elapsed();
                QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss.zzz");
                pb->setText("Идёт запись: "+TimeElapsed);
            }
            QXlsx::Format format;
            format.setNumberFormat("0.0000");
            xlsx->write(WRow,1,QVariant(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz")));
            for (int i=0; i<16; i++)
            {
                if (aconf->Config->Bci_block.in_type[i] == Config2x::AIT_MA)
                    xlsx->write(WRow,i+3,Bd_block.inI[i],format);
                else if (aconf->Config->Bci_block.in_type[i] == Config2x::AIT_V)
                    xlsx->write(WRow,i+3,Bd_block.inU[i],format);
                else
                    continue;
            }
            xlsx->write(WRow,2,Bd_block.at,format);
            WRow++;

        }
    }
    if ((BdaMeasurementsActive) && (!OddTimeout)) // все остальные - на втором
    {
        cn->Send(CN_GBda, Canal::BT_NONE, &Bda_block, sizeof(Bda));
        if (cn->result == NOERROR)
            RefreshBda();
    }
    OddTimeout = !OddTimeout;
} */
