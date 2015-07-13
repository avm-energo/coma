#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QThread>
#include <QCoreApplication>
#include "e_tunedialog.h"
#include "../publicclass.h"
#include "../canal.h"

e_tunedialog::e_tunedialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void e_tunedialog::SetupUI()
{
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QWidget *cp5 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QGridLayout *glyout = new QGridLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp2,"Настройка");
    TuneTW->addTab(cp1,"Коэффициенты");
    TuneTW->addTab(cp4,"Данные модуля");
    TuneTW->addTab(cp5,"Данные АЦП");
    TuneTW->addTab(cp3,"Данные МИП");

    // CP1 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("KmU["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("kmu"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,1,i,1,1);
        lbl = new QLabel("KmI_5["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("kmi5"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,3,i,1,1);
        lbl = new QLabel("KmI_1["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("kmi1"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,5,i,1,1);
        lbl = new QLabel("DPsi["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("dpsi"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,7,i,1,1);
    }
    lbl=new QLabel("K_freq:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,0,1,1);
    lbl = new QLabel("");
    lbl->setObjectName("kfreq"+QString::number(i));
    lbl->setStyleSheet(ValuesFormat);
    glyout->addWidget(lbl,8,1,1,2);
    lbl=new QLabel("Kinter:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,3,1,1);
    lbl = new QLabel("");
    lbl->setObjectName("kinter"+QString::number(i));
    lbl->setStyleSheet(ValuesFormat);
    glyout->addWidget(lbl,8,4,1,2);

    QPushButton *pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    glyout->addWidget(pb, 9, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    glyout->addWidget(pb, 10, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    glyout->addWidget(pb, 11, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    glyout->addWidget(pb, 12, 0, 1, 6);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(1);
    cp1->setLayout(lyout);

    // CP3 - ПОКАЗАНИЯ МИП-02

    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hglyout = new QHBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    gb = new QGroupBox("Измеряемые параметры");
    QGroupBox *gb2 = new QGroupBox("Частота");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+1));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Фазное напряжение");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+4));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Фазный ток");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+7));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Угол нагрузки");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+11));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Фазовый угол напряжения");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+14));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Прочие");
    lbl = new QLabel("Ток N");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip10");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("Темп.");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip17");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    gb->setLayout(vlyout);
    hglyout->addWidget(gb);
    gb = new QGroupBox("Вычисляемые параметры");
    gb2 = new QGroupBox("Активная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+22));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    QVBoxLayout *gblyout = new QVBoxLayout;
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("Реактивная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+26));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("Полная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+30));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("Линейное напряжение");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+19));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    vlyout=new QVBoxLayout;
    gb2 = new QGroupBox("Мощность");
    lbl = new QLabel("Акт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip25");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("Реакт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip29");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    vlyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl = new QLabel("Полная:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip33");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    vlyout->addLayout(hlyout);
    vlyout->addStretch(1);
    gb2->setLayout(vlyout);
    gblyout->addWidget(gb2,1);
    gb->setLayout(gblyout);
    hglyout->addWidget(gb);
    vlyout = new QVBoxLayout;
    vlyout->addLayout(hglyout);
    pb = new QPushButton("Запустить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMip()));
    vlyout->addWidget(pb);
    pb = new QPushButton("Остановить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMip()));
    vlyout->addWidget(pb);
    cp3->setLayout(vlyout);

    // CP4 - ДАННЫЕ МОДУЛЯ

    lyout=new QVBoxLayout;

    // CP2 - НАСТРОЙКА МОДУЛЯ

    lyout = new QVBoxLayout;
    pb = new QPushButton("Начать настройку");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
    lyout->addWidget(pb);
    hlyout = new QHBoxLayout;
    lbl=new QLabel("7.3.1. Получение настроечных параметров...");
    lbl->setVisible(false);
    lbl->setObjectName("tune1");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setVisible(false);
    lbl->setObjectName("tune1res");
    hlyout->addWidget(lbl);
    hlyout->addStretch(1);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl=new QLabel("7.3.2. Получение текущих аналоговых данных...");
    lbl->setVisible(false);
    lbl->setObjectName("tune2");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setVisible(false);
    lbl->setObjectName("tune2res");
    hlyout->addWidget(lbl);
    hlyout->addStretch(1);
    lyout->addLayout(hlyout);

    lyout->addStretch(1);
    cp2->setLayout(lyout);

    // CP5 - ДАННЫЕ АЦП МОДУЛЯ

    lyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Температура:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("tmk");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lbl = new QLabel("Частота:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("frequency");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lyout->addLayout(hlyout);
    glyout = new QGridLayout;
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("IUNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("iunf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Истинные действующие значения сигналов");
        glyout->addWidget(lbl,1,i,1,1);
        lbl = new QLabel("IUF["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("iuf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Действующие значения сигналов по 1-й гармонике");
        glyout->addWidget(lbl,3,i,1,1);
        lbl = new QLabel("PHF["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("phf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Угол сдвига между сигналами по первой гармонике");
        glyout->addWidget(lbl,5,i,1,1);
    }
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("PNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("pnf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Истинная активная мощность");
        glyout->addWidget(lbl,7,i,1,1);
        lbl = new QLabel("SNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("snf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Кажущаяся полная мощность");
        glyout->addWidget(lbl,7,i+3,1,1);
        lbl = new QLabel("QNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("qnf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Реактивная мощность");
        glyout->addWidget(lbl,9,i,1,1);
        lbl = new QLabel("Cos["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("cos"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Cos phi");
        glyout->addWidget(lbl,9,i+3,1,1);
        lbl = new QLabel("PF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("pf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Активная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i,1,1);
        lbl = new QLabel("QF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("qf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Реактивная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i+3,1,1);
        lbl = new QLabel("SF["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("sf"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Полная мощность по 1-й гармонике");
        glyout->addWidget(lbl,13,i,1,1);
        lbl = new QLabel("CosPhi["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("cosphi"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip("Cos phi по 1-й гармонике");
        glyout->addWidget(lbl,13,i+3,1,1);
    }
    lyout->addLayout(glyout);
    lyout->addStretch(1);
    cp5->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

void e_tunedialog::StartTune()
{
    // высвечиваем надпись "Получение настроечных коэффициентов"
    QLabel *lbl = this->findChild<QLabel *>("tune1");
    if (lbl == 0)
        return;
    lbl->setVisible(true);
    // получение настроечных коэффициентов от модуля
    cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        qApp->processEvents();
    if (cn->result != CN_OK)
        return;
    // обновление коэффициентов в соответствующих полях на экране
    RefreshTuneCoefs();
    lbl=this->findChild<QLabel *>("tune1res");
    if (lbl == 0)
        return;
    // проверка коэффициентов на правильность в соотв. с п. 7.3.2 "Д2"
    if (CheckTuneCoefs())
    {
        lbl->setVisible(true);
        lbl->setText("готово!");
    }
    else
    {
        lbl->setVisible(true);
        lbl->setStyleSheet("QLabel {color: red};");
        lbl->setText("ошибка!");
        return;
    }
    // высвечиваем надпись "Получение настроечных коэффициентов"
    lbl = this->findChild<QLabel *>("tune2");
    if (lbl == 0)
        return;
    lbl->setVisible(true);
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_Gda, &Bda_block, sizeof(Bda_block));
    while (cn->Busy)
        qApp->processEvents();
    if (cn->result != CN_OK)
        return;
    // обновление коэффициентов в соответствующих полях на экране
    RefreshAnalogValues();
    lbl=this->findChild<QLabel *>("tune1res");
    if (lbl == 0)
        return;
    // проверка коэффициентов на правильность в соотв. с п. 7.3.2 "Д2"
}

/*void e_tunedialog::tune(int tunenum)
{
    switch (tunenum)
    {
    case 0:
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на ток,\nустановите напряжение 0 В " \
                                     "на всех\nвходах модуля и нажмите OK")\
                == QMessageBox::Ok)
        {
            cn->Send(CN_Gda, &Bda0, sizeof(Bda));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckAndShowTune0();
        }
        break;
    }
    case 5:
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на напряжение,\nустановите напряжение" \
                                     "5 В на всех\nвходах модуля и нажмите OK")\
                == QMessageBox::Ok)
        {
            cn->Send(CN_Gda, &Bda5, sizeof(Bda));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckAndShowTune5();
        }
        break;
    }
    case 20:
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на ток,\nустановите ток 20 мА на всех" \
                                     "\nвходах модуля и нажмите OK") == QMessageBox::Ok)
        {
            cn->Send(CN_Gda, &Bda20, sizeof(Bda));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckAndShowTune20();
        }
        break;
    }
    default:
        break;
    }
}

void e_tunedialog::tune0()
{
    tune(0);
}*/

bool e_tunedialog::CheckTuneCoefs()
{
    QLabel *lbl;
    bool ok = true;
    for (int i = 0; i < 6; i++)
    {
        if (!IsWithinLimits(Bac_block.KmU[i],1.0,0.02))
        {
            ok=false;
            lbl = this->findChild<QLabel *>("kmu"+QString::number(i));
            if (lbl == 0)
                return false;
            lbl->setStyleSheet("QLabel {color: red};");
        }
        if (!IsWithinLimits(Bac_block.KmI_1[i],1.0,0.02))
        {
            ok=false;
            lbl = this->findChild<QLabel *>("kmi1"+QString::number(i));
            if (lbl == 0)
                return false;
            lbl->setStyleSheet("QLabel {color: red};");
        }
        if (!IsWithinLimits(Bac_block.KmI_5[i],1.0,0.02))
        {
            ok=false;
            lbl = this->findChild<QLabel *>("kmi5"+QString::number(i));
            if (lbl == 0)
                return false;
            lbl->setStyleSheet("QLabel {color: red};");
        }
        if (!IsWithinLimits(Bac_block.DPsi[i],0.0,1.0))
        {
            ok=false;
            lbl = this->findChild<QLabel *>("dpsi"+QString::number(i));
            if (lbl == 0)
                return false;
            lbl->setStyleSheet("QLabel {color: red};");
        }
    }
    if (!IsWithinLimits(Bac_block.K_freq,1.0,0.02))
    {
        ok=false;
        lbl = this->findChild<QLabel *>("kfreq");
        if (lbl == 0)
            return false;
        lbl->setStyleSheet("QLabel {color: red};");
    }
    if (!IsWithinLimits(Bac_block.Kinter,0.0,0.005))
    {
        ok=false;
        lbl = this->findChild<QLabel *>("kinter");
        if (lbl == 0)
            return false;
        lbl->setStyleSheet("QLabel {color: red};");
    }
    return ok;
}

bool e_tunedialog::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

/*
void e_tunedialog::tune5()
{
    tune(5);
}

void e_tunedialog::CheckAndShowTune5()
{
    for (int i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tune5ch"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda5.sin[i]/Z,16));
    }
    CalcNewTuneCoefs();
    RefreshTuneCoefs();
}

void e_tunedialog::tune20()
{
    tune(20);
}

void e_tunedialog::CheckAndShowTune20()
{
    for (int i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tune20ch"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda20.sin[i]/Z,16));
    }
    CalcNewTuneCoefs();
    RefreshTuneCoefs();
}
*/
void e_tunedialog::ReadTuneCoefs()
{
    cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        RefreshTuneCoefs();
}

void e_tunedialog::WriteTuneCoefs()
{
    cn->Send(CN_Wac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        QMessageBox::information(this,"Успешно!","Записано успешно!");
}

void e_tunedialog::CalcNewTuneCoefs()
{
/*    for (int i = 0; i < 16; i++)
    {
        Bac_block[i].fbin = 1.25 - (static_cast<float>(Bda0.sin[i])/(Z*1638.0));
        if ((Bda0.sin[i] == Bda5.sin[i]) || (Bda0.sin[i] == Bda20.sin[i]))
        {
            QMessageBox::warning(this,"Предупреждение","Ошибки в настроечных коэффициентах\nДеление на ноль");
            return;
        }
        Bac_block[i].fkuin = Z*1638.0/static_cast<float>(Bda0.sin[i]-Bda5.sin[i]);
        Bac_block[i].fkiin = Z*1638.0/static_cast<float>(Bda0.sin[i]-Bda20.sin[i]);
    }*/
}

void e_tunedialog::RefreshTuneCoefs()
{
    QLabel *lbl;
    for (int i = 0; i < 6; i++)
    {
        lbl = this->findChild<QLabel *>("kmu"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block.KmU[i], 'f', 5));
        lbl = this->findChild<QLabel *>("kmi5"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString("%1").arg(Bac_block.KmI_5[i], 0, 'f', 5));
        lbl = this->findChild<QLabel *>("kmi1"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block.KmI_1[i], 'f', 5));
        lbl = this->findChild<QLabel *>("dpsi"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block.DPsi[i], 'f', 5));
    }
    lbl = this->findChild<QLabel *>("kfreq");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bac_block.K_freq, 'f', 5));
    lbl = this->findChild<QLabel *>("kinter");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bac_block.Kinter, 'f', 5));

}

void e_tunedialog::RefreshAnalogValues()
{

}

void e_tunedialog::LoadFromFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть файл", ".", "Configuration (*.conf)");
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error(ER_FILEOPENERROR);
        return;
    }
    QByteArray *ba = new QByteArray(file.readAll());
    publicclass::Bsi Bsi_block;
    if (ba->size() >= (sizeof(publicclass::Bsi)+sizeof(Bac_block)))
    {
        memcpy(&Bsi_block,ba,sizeof(publicclass::Bsi));
        if ((Bsi_block.CpuIdHigh != pc.CpuIdHigh) || (Bsi_block.SerNum != pc.SerNum))
        {
            if (QMessageBox::question(this,"Не тот файл","В файле содержатся данные для модуля с другим CPUID и/или SN.\nПродолжить загрузку?",\
                                      QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok);
            else
                return;
        }
        // продолжение загрузки файла
    }
}

void e_tunedialog::SaveToFile()
{

}

void e_tunedialog::StartMip()
{
    mipcanal = new iec104;
    connect(mipcanal,SIGNAL(error(int)),this,SIGNAL(error(int)));
    connect(mipcanal,SIGNAL(signalsready(Parse104::Signals104&)),this,SLOT(MipData(Parse104::Signals104&)));
    connect(mipcanal,SIGNAL(ethconnected()),this,SLOT(EthConnected()));
    connect(mipcanal,SIGNAL(ethdisconnected()),this,SLOT(EthDisconnected()));
    connect(this,SIGNAL(stopall()),mipcanal,SLOT(Stop()));
    connect(mipcanal,SIGNAL(finished()),this,SLOT(DeleteMip()));
}

void e_tunedialog::EthConnected()
{
    QByteArray ba;
    ba.clear();
    ba.insert(0,0x3A);
    emit dataready(ba);
}

void e_tunedialog::EthDisconnected()
{
    QByteArray ba;
    ba.clear();
    ba.insert(0,0x3B);
    emit dataready(ba);
}

void e_tunedialog::MipData(Parse104::Signals104 &Signal)
{
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.SigNum;
    if (index != -1)
    {
        QLabel *lbl = this->findChild<QLabel *>("mip"+QString::number(index));
        if (lbl != 0)
            lbl->setText(Signal.SigVal);
    }
}

void e_tunedialog::StopMip()
{
    emit stopall();
}

void e_tunedialog::DeleteMip()
{
    delete mipcanal;
}

void e_tunedialog::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}
