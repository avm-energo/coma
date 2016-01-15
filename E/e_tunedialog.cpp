#include <QtDebug>
#include <QtMath>
#include <QTime>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QThread>
#include <QCoreApplication>
#include "e_tunedialog.h"
#include "../publicclass.h"
#include "../canal.h"
#include "e_config.h"
#include "../widgets/waitwidget.h"

e_tunedialog::e_tunedialog(QWidget *parent) :
    QDialog(parent)
{
    Bac_defblock.Kinter = 0.0;
    Bac_defblock.K_freq = 1.0;
    for (int i=0; i<6; i++)
    {
        Bac_defblock.DPsi[i] = 0.0;
        Bac_defblock.KmI_1[i] = 1.0;
        Bac_defblock.KmI_5[i] = 1.0;
        Bac_defblock.KmU[i] = 1.0;
    }
    tmr = new QTimer;
    tmr->setObjectName("etunetimer");
    tmr->setInterval(ANMEASINT);
    XlsxWriting = false;
    xlsx = 0;
    connect(tmr,SIGNAL(timeout()),this,SLOT(ReadAnalogMeasurements()));
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void e_tunedialog::SetupUI()
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QWidget *cp5 = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);
    cp5->setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QGridLayout *glyout = new QGridLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Настройка");
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные МИП");
    TuneTW->addTab(cp4,"Данные 104");
    TuneTW->addTab(cp5,"Данные модуля");

    // CP1 - НАСТРОЙКА МОДУЛЯ

    lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать настройку с начала");
    pb->setObjectName("starttune1p");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Начать настройку с трех фаз");
    pb->setObjectName("starttune3p");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune3p()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    QStringList lbls;
    lbls << "7.2.3. Проверка связи РЕТОМ и МИП..." << "7.3.1. Получение настроечных параметров..." << \
            "7.3.1.1. Установка настроечных параметров по умолчанию..." << "7.3.2. Получение текущих аналоговых данных..." << \
            "7.3.3. Расчёт коррекции смещений сигналов по фазе..." << "7.3.4. Расчёт коррекции по частоте..." << \
            "7.3.5. Считывание напряжений при нулевых токах..." << "7.3.6.0. Пересборка схемы подключения..." << \
            "7.3.6.1. Получение текущих аналоговых данных..." << "7.3.6.2. Расчёт коррекции взаимного влияния каналов..." << \
            "7.3.7.1.1. Получение текущих аналоговых данных..." << \
            "7.3.7.1.2. Расчёт калибровочных коэффициентов по напряжениям..." << "7.3.7.2. Сохранение конфигурации и перезапуск..." << \
            "7.3.7.3. Получение текущих аналоговых данных..." << "7.3.7.5. Расчёт калибровочных коэффициентов по токам, напряжениям и углам..." << \
            "7.3.7.6. Сохранение конфигурации и перезапуск..." << "7.3.7.8. Получение текущих аналоговых данных..." << \
            "7.3.7.10. Расчёт калибровочных коэффициентов по токам, напряжениям и углам..." << "7.3.8.1. Запись настроечных коэффициентов и переход на новую конфигурацию..." << \
            "7.3.8.2. Проверка аналоговых данных..." << "7.3.9. Восстановление сохранённой конфигурации и проверка..." << "Настройка окончена!";
    for (int i = 0; i < lbls.size(); i++)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        lbl=new QLabel(lbls.at(i));
        lbl->setVisible(false);
        lbl->setObjectName("tunemsg"+QString::number(i));
        hlyout->addWidget(lbl);
        lbl=new QLabel("");
        lbl->setVisible(false);
        lbl->setObjectName("tunemsgres"+QString::number(i));
        hlyout->addWidget(lbl);
        hlyout->addStretch(1);
        lyout->addLayout(hlyout);
    }
    lyout->addStretch(1);
    cp1->setLayout(lyout);

    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("KmU["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,1,i,1,1);
        lbl = new QLabel("KmI_5["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+6));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,3,i,1,1);
        lbl = new QLabel("KmI_1["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+12));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,5,i,1,1);
        lbl = new QLabel("DPsi["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+18));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,7,i,1,1);
    }
    lbl=new QLabel("K_freq:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,0,1,1);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName("tune24");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,8,1,1,2);
    lbl=new QLabel("Kinter:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,3,1,1);
    le = new QLineEdit("");
    le->setObjectName("tune25");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,8,4,1,2);

    pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefCoefs()));
    glyout->addWidget(pb, 9, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    glyout->addWidget(pb, 10, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    glyout->addWidget(pb, 11, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    glyout->addWidget(pb, 12, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    glyout->addWidget(pb, 13, 0, 1, 6);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(1);
    cp2->setLayout(lyout);

    // CP3 - ПОКАЗАНИЯ МИП-02

    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hglyout = new QHBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    gb = new QGroupBox("Измеряемые параметры");
    QGroupBox *gb2 = new QGroupBox("1-3. Частота");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+1));
        lbl->setToolTip("Параметр "+QString::number(i+1));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("4-6. Фазное напряжение");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+4));
        lbl->setToolTip("Параметр "+QString::number(i+4));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("7-9. Фазный ток");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+7));
        lbl->setToolTip("Параметр "+QString::number(i+7));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("11-13. Угол нагрузки");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+11));
        lbl->setToolTip("Параметр "+QString::number(i+11));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("14-16. Фазовый угол напряжения");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+14));
        lbl->setToolTip("Параметр "+QString::number(i+14));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    gb2 = new QGroupBox("Прочие");
    lbl = new QLabel("10. Ток N");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip10");
    lbl->setToolTip("Параметр 10");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("17. Темп.");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip17");
    lbl->setToolTip("Параметр 17");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    gb2->setLayout(hlyout);
    vlyout->addWidget(gb2);
    gb->setLayout(vlyout);
    hglyout->addWidget(gb);
    gb = new QGroupBox("Вычисляемые параметры");
    gb2 = new QGroupBox("22-24. Активная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+22));
        lbl->setToolTip("Параметр "+QString::number(i+22));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    QVBoxLayout *gblyout = new QVBoxLayout;
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("26-28. Реактивная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+26));
        lbl->setToolTip("Параметр "+QString::number(i+26));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("30-32. Полная мощность");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+30));
        lbl->setToolTip("Параметр "+QString::number(i+30));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    gb2 = new QGroupBox("19-21. Линейное напряжение");
    hlyout = new QHBoxLayout;
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+19));
        lbl->setToolTip("Параметр "+QString::number(i+19));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb2->setLayout(hlyout);
    gblyout->addWidget(gb2);
    hlyout = new QHBoxLayout;
    vlyout=new QVBoxLayout;
    gb2 = new QGroupBox("Мощность");
    lbl = new QLabel("25. Акт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip25");
    lbl->setToolTip("Параметр 25");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("29. Реакт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip29");
    lbl->setToolTip("Параметр 29");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    vlyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl = new QLabel("33. Полная:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip33");
    lbl->setToolTip("Параметр 33");
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

    // CP4 - ДАННЫЕ 104

    lyout=new QVBoxLayout;

    // CP5 - ДАННЫЕ МОДУЛЯ

    lyout = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    lbl = new QLabel("0.Температура:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("value0");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lbl = new QLabel("1.Частота:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("value1");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lyout->addLayout(hlyout);
    glyout = new QGridLayout;
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("IUNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+2));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+2)+".Истинные действующие значения сигналов");
        glyout->addWidget(lbl,1,i,1,1);
        lbl = new QLabel("IUF["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+8));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+8)+".Действующие значения сигналов по 1-й гармонике");
        glyout->addWidget(lbl,3,i,1,1);
        lbl = new QLabel("PHF["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+14));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+14)+".Угол сдвига между сигналами по первой гармонике");
        glyout->addWidget(lbl,5,i,1,1);
    }
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("PNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+20));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+20)+".Истинная активная мощность");
        glyout->addWidget(lbl,7,i,1,1);
        lbl = new QLabel("SNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+23));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+23)+".Кажущаяся полная мощность");
        glyout->addWidget(lbl,7,i+3,1,1);
        lbl = new QLabel("QNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+26));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+26)+".Реактивная мощность");
        glyout->addWidget(lbl,9,i,1,1);
        lbl = new QLabel("Cos["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+29));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+29)+".Cos phi");
        glyout->addWidget(lbl,9,i+3,1,1);
        lbl = new QLabel("PF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+32));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+32)+".Активная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i,1,1);
        lbl = new QLabel("QF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+35));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+35)+".Реактивная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i+3,1,1);
        lbl = new QLabel("SF["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+38));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+38)+".Полная мощность по 1-й гармонике");
        glyout->addWidget(lbl,13,i,1,1);
        lbl = new QLabel("CosPhi["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+41));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+41)+".Cos phi по 1-й гармонике");
        glyout->addWidget(lbl,13,i+3,1,1);
    }
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("PHI["+QString::number(i)+"]");
        glyout->addWidget(lbl,14,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+44));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+44)+".Угол между током и напряжением");
        glyout->addWidget(lbl,15,i,1,1);
    }
    lyout->addLayout(glyout);

    hlyout = new QHBoxLayout;
    lbl = new QLabel("Период обновления данных измерения, сек:");
    hlyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("500");
    rb->setText("0,5");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("2000");
    rb->setText("2");
    rb->setChecked(true);
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("10000");
    rb->setText("10");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    lyout->addLayout(hlyout);

    pb = new QPushButton("Запустить чтение аналоговых сигналов");
    pb->setObjectName("pbmeasurements");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartAnalogMeasurements()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Запустить чтение аналоговых сигналов в файл");
    pb->setObjectName("pbfilemeasurements");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartAnalogMeasurementsToFile()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение аналоговых сигналов");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopAnalogMeasurements()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);

    lyout->addStretch(1);
    cp5->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

void e_tunedialog::StartTune()
{
    SetTunePbEnabled(false);
    bool res=true;
    Cancelled = false;
    StopAnalogMeasurements(); // останавливаем текущие измерения, чтобы не мешали процессу
    StopMip(); // останавливаем измерения МИП
    MsgClear(); // очистка экрана с сообщениями

    if (!SaveWorkConfig())
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        return;
    }
    // показываем диалог с выбором метода контроля
    ShowControlChooseDialog();
    if (Cancelled)
    {
        SetTunePbEnabled(true);
        return;
    }
    // показываем диалог со схемой подключения
    Show1PhaseScheme();
    if (Cancelled)
        return;
    // показываем диалог с требованием установки необходимых сигналов на РЕТОМ
    Show1RetomDialog(60, 1);
    if (Cancelled)
    {
        SetTunePbEnabled(true);
        return;
    }
    WaitNSeconds(15);
    // 7.2.3. проверка связи РЕТОМ и МИП
    if (TuneControlType == TUNEMIP)
    {
        res = Start7_2_3();
        if (!res)
        {
            SetTunePbEnabled(true);
            ETUNEWARN;
            return;
        }
    }

    // 7.3.1. получение настроечных коэффициентов
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет настроечных коэффициентов в памяти модуля
        res = Start7_3_1(true); // запуск процедуры записи настроечных коэффициентов в режиме по умолчанию
    else
        res = Start7_3_1(false);
    if (!res)
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        return;
    }

    // переписываем считанные или установленные по умолчанию коэффициенты в блок новых коэффициентов
    SetNewTuneCoefs();

    // 7.3.2. получение аналоговых сигналов
    res = Start7_3_2(MSG_7_3_2);
    if (!res)
    {
        SetTunePbEnabled(true);
        return;
    }
    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i=0; i<6; i++)
        IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i];

    // 7.3.3. расчёт коррекции по фазе и по частоте
    res = Start7_3_3();
    if (!res)
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        return;
    }

    // 7.3.4. расчёт коррекции по частоте
    res = Start7_3_4();
    if (!res)
    {
        ETUNEWARN;
        SetTunePbEnabled(true);
        return;
    }

    // 7.3.5. Считывание Bda_in с нулевыми значениями токов для Э1Т1Н
    Start7_3_5();

    Show3PhaseScheme();
    if (Cancelled)
    {
        SetTunePbEnabled(true);
        return;
    }

    // 7.3.6.1. получение аналоговых данных
    WaitNSeconds(15);
    res = Start7_3_2(MSG_7_3_6_1);
    if (!res)
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        return;
    }

    // 7.3.6.2. расчёт коррекции влияния каналов
    Start7_3_6_2();

    Tune3p();
}

void e_tunedialog::StartTune3p()
{
    SetTunePbEnabled(false);
    Cancelled = false;
    StopAnalogMeasurements(); // останавливаем текущие измерения, чтобы не мешали процессу
    StopMip(); // останавливаем измерения МИП
    MsgClear(); // очистка экрана с сообщениями
    if (!SaveWorkConfig())
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        return;
    }
    ShowControlChooseDialog();
    if (Cancelled)
    {
        SetTunePbEnabled(true);
        return;
    }
    int res = true;
    // 7.3.1. получение настроечных коэффициентов
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет настроечных коэффициентов в памяти модуля
        res = Start7_3_1(true); // запуск процедуры записи настроечных коэффициентов в режиме по умолчанию
    else
        res = Start7_3_1(false);
    if (!res)
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        return;
    }

    // переписываем считанные или установленные по умолчанию коэффициенты в блок новых коэффициентов
    SetNewTuneCoefs();

    Show3PhaseScheme();
    if (Cancelled)
    {
        SetTunePbEnabled(true);
        return;
    }

    WaitNSeconds(15);

    // 7.3.6.1. получение аналоговых данных
    res = Start7_3_2(MSG_7_3_6_1);
    if (!res)
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        return;
    }

    Tune3p();
}

void e_tunedialog::Tune3p()
{
    int res = true;

    if (pc.ModuleBsi.MType1 == MTE_0T2N)
    {
        // 7.3.7.1. расчёт калибровочных коэффициентов по напряжениям
        res = Start7_3_7_1();
        if (!res)
        {
            ETUNEWARN;
            SetTunePbEnabled(true);
            LoadWorkConfig();
            return;
        }
    }
    else
    {
        // 7.3.7.2. расчёт калибровочных коэффициентов по токам
        res = Start7_3_7_2();
        if (!res)
        {
            SetTunePbEnabled(true);
            ETUNEWARN;
            LoadWorkConfig();
            return;
        }
    }
    res = Start7_3_8();
    if (!res)
    {
        SetTunePbEnabled(true);
        ETUNEWARN;
        LoadWorkConfig();
        return;
    }
    res = Start7_3_9();
    if (res)
    {
        SetTunePbEnabled(true);
        ETUNEINFO("Настройка проведена успешно!");
        OkMsgSetVisible(MSG_END);
    }
}

void e_tunedialog::SetTunePbEnabled(bool Enabled)
{
    QPushButton *pb = this->findChild<QPushButton *>("starttune1p");
    if (pb == 0)
    {
        ETUNEDBG;
        return;
    }
    pb->setEnabled(Enabled);
    pb = this->findChild<QPushButton *>("starttune1p");
    if (pb == 0)
    {
        ETUNEDBG;
        return;
    }
    pb->setEnabled(Enabled);
}

void e_tunedialog::SetTuneManual()
{
    TuneControlType=TUNEMAN;
}

void e_tunedialog::SetTuneMip()
{
    TuneControlType=TUNEMIP;
}

void e_tunedialog::SetTuneRetom()
{
    TuneControlType=TUNERET;
}

bool e_tunedialog::Start7_2_3()
{
    // высвечиваем надпись "Проверка связи РЕТОМ и МИП"
    MsgSetVisible(MSG_7_2_3);
    StartMip();
    QTime tmr;
    tmr.start();
    while (tmr.elapsed() < 5000)
        qApp->processEvents();
    StopMip();
    if (CheckMip())
        OkMsgSetVisible(MSG_7_2_3);
    else
    {
        ErMsgSetVisible(MSG_7_2_3);
        return false;
    }
    return true;
}

bool e_tunedialog::Start7_3_1(bool DefConfig)
{
    // высвечиваем надпись "Получение настроечных коэффициентов"
    MsgSetVisible(MSG_7_3_1);
    if (!DefConfig)
    {
        // получение настроечных коэффициентов от модуля
        cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
        while (cn->Busy)
            qApp->processEvents();
        if (cn->result != CN_OK)
        {
            ETUNEINFO("Ошибка при приёме данных");
            return false;
        }
        // обновление коэффициентов в соответствующих полях на экране
        WriteTuneCoefsToGUI();
        // проверка коэффициентов на правильность в соотв. с п. 7.3.1 "Д2"
        if (CheckTuneCoefs())
        {
            OkMsgSetVisible(MSG_7_3_1);
            return true;
        }
        else
            ErMsgSetVisible(MSG_7_3_1);
    }
    // высвечиваем надпись "Запись настроечных коэффициентов по умолчанию"
    MsgSetVisible(MSG_7_3_1_1);
    // запись настроечных коэффициентов в модуль
    cn->Send(CN_Wac, &Bac_defblock, sizeof(Bac));
    while (cn->Busy)
        qApp->processEvents();
    if (cn->result == CN_OK)
    {
        // перейти на новую конфигурацию
        cn->Send(CN_Cnc);
        while (cn->Busy)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (cn->result != CN_OK)
            return false;
        WaitNSeconds(5);
        // получение настроечных коэффициентов от модуля
        cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
        while (cn->Busy)
            qApp->processEvents();
        if (cn->result != CN_OK)
        {
            ETUNEINFO("Ошибка при приёме данных");
            return false;
        }
        // обновление коэффициентов в соответствующих полях на экране
        WriteTuneCoefsToGUI();
        OkMsgSetVisible(MSG_7_3_1_1);
    }
    else
    {
        ETUNEINFO("Ошибка при приёме данных");
        ErMsgSetVisible(MSG_7_3_1_1);
        return false;
    }
    return true;
}

bool e_tunedialog::Start7_3_2(int num)
{
    // высвечиваем надпись "Получение текущих аналоговых данных"
    MsgSetVisible(num);
    ReadAnalogMeasurements();
    // проверка данных на правильность
    int maxval;
    if (num == MSG_7_3_2) maxval=602; // 3~7.3.2, 6~7.3.6.1, 12~7.3.7.3
    else if (num == MSG_7_3_7_8)
    {
        if (!GetExternalData(MSG_7_3_7_8))
            return false;
        maxval = 607; // 15~7.3.7.8
    }
    else if (num == MSG_7_3_8_2)
    {
        if (!GetExternalData(MSG_7_3_8_2))
            return false;
        maxval = 617; // 18~7.3.8
    }
    else if (num == MSG_7_3_9)
    {
        if (!GetExternalData(MSG_7_3_9))
            return false;
        maxval = 572; // 19~7.3.9
    }
    else
    {
        OkMsgSetVisible(num);
        return true;
    }

    if (CheckAnalogValues(maxval-pc.ModuleBsi.MType1)) // MType: 0 = 2T0N, 1 = 1T1N, 2 = 0T2N; NTest: 600 = 0T2N, 601 = 1T1N, 602 = 2T0N
    {
        OkMsgSetVisible(num);
        return true;
    }
    else
    {
        ErMsgSetVisible(num);
        return false;
    }
    return true;
}

bool e_tunedialog::Start7_3_3()
{
    // высвечиваем надпись "7.3.3. Расчёт коррекции смещений сигналов по фазе"
    MsgSetVisible(MSG_7_3_3);
//    double fTmp = Bda_block.phi_next_f[0];
    Bac_newblock.DPsi[0] = 0;
    int k = (pc.ModuleBsi.MType1 == MTE_1T1N) ? 3 : 6;
    for (int i=1; i<k; i++)
    {
        Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - Bda_block.phi_next_f[i];
//        fTmp += Bda_block.phi_next_f[i];
    }
    OkMsgSetVisible(MSG_7_3_3);
    return true;
}

bool e_tunedialog::Start7_3_4()
{
    // высвечиваем надпись "7.3.4. Расчёт коррекции по частоте"
    MsgSetVisible(MSG_7_3_4);
    switch (TuneControlType)
    {
    case TUNEMIP:
    {
        StartMip();
        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 5000)
            qApp->processEvents();
        StopMip();
        if (CheckMip())
        {
            double fTmp = MipDat[1]; // частота ф. А
            Bac_newblock.K_freq = Bac_block.K_freq*fTmp/Bda_block.Frequency;
            OkMsgSetVisible(MSG_7_3_4);
        }
        else
        {
            ErMsgSetVisible(MSG_7_3_4);
            return false;
        }
        break;
    }
    case TUNERET:
    {
        Bac_newblock.K_freq = Bac_block.K_freq*50.0/Bda_block.Frequency; // считаем, что частота 50 Гц
        OkMsgSetVisible(MSG_7_3_4);
        break;
    }
    case TUNEMAN:
    {
        double fTmp = QInputDialog::getDouble(this,"Ввод числа","Введите показания приборов по частоте",50.0,40.0,60.0,5);
        Bac_newblock.K_freq = Bac_block.K_freq*fTmp/Bda_block.Frequency;
        OkMsgSetVisible(MSG_7_3_4);
        break;
    }
    }
    return true;
}

void e_tunedialog::Start7_3_5()
{
    if (pc.ModuleBsi.MType1 == MTE_1T1N)
    {
        // высвечиваем надпись "7.3.5. Считывание напряжений при нулевых токах"
        MsgSetVisible(MSG_7_3_5);
        QDialog *dlg = new QDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        QVBoxLayout *lyout = new QVBoxLayout;
        QLabel *lbl = new QLabel("Установите в РЕТОМ нулевые значения токов");
        lyout->addWidget(lbl);
        QPushButton *pb = new QPushButton("Готово");
        connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
        lyout->addWidget(pb);
        pb = new QPushButton("Отмена");
        connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
        connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
        lyout->addWidget(pb);
        dlg->setLayout(lyout);
        dlg->exec();
        // ждём 15 секунд
        WaitNSeconds(15);
        // измеряем текущие аналоговые значения
        ReadAnalogMeasurements();
        // сохраняем значения для выполнения п. 7.3.6
        for (int i=0; i<6; i++)
            IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i];
        OkMsgSetVisible(MSG_7_3_5);
    }
}

void e_tunedialog::Start7_3_6_2()
{
    // высвечиваем надпись "7.3.6.2. Расчёт коррекции взаимного влияния"
    MsgSetVisible(MSG_7_3_6_2);
    double fSum1 = 0.0;
    double fSum2 = 0.0;
    int k = (pc.ModuleBsi.MType1 == MTE_1T1N) ? 3 : 6;
    for (int i=0; i<k; i++)
    {
        fSum1 += (Bda_block.IUeff_filtered[i] - IUefNat_filt_old[i]);
        fSum2 += Bda_block.IUeff_filtered[i];
    }
    Bac_newblock.Kinter = Bac_block.Kinter+(fSum1/fSum2)/k;
    OkMsgSetVisible(MSG_7_3_6_2);
}

bool e_tunedialog::Start7_3_7_1()
{
    // высвечиваем надпись "7.3.7.1.1"
    MsgSetVisible(MSG_7_3_7_1_1);

    if (!GetExternalData(MSG_7_3_7_1_1))
        return false;

    // высвечиваем надпись "7.3.7.1.2"
    MsgSetVisible(MSG_7_3_7_1_2);
    for (int i=0; i<3; i++)
    {
        Bac_newblock.KmU[i] = Bac_block.KmU[i] * mipd[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmU[i+3] = Bac_block.KmU[i+3] * mipd[i] / Bda_block.IUefNat_filt[i+3];
    }
    OkMsgSetVisible(MSG_7_3_7_1_2);
    return true;
}

bool e_tunedialog::Start7_3_7_2()
{
    // 1. установить в конфигурации токи i2nom в 1 А и перезагрузить модуль
    MsgSetVisible(MSG_7_3_7_2);
//    qDebug() << "2";
    for (int i=0; i<6; i++)
        econf->Bci_block.inom2[i] = 1.0;
    // послать новые коэффициенты по току в конфигурацию
    cn->Send(CN_WF, &econf->Bci_block, sizeof(e_config::Bci), 2, econf->Config);
//    qDebug() << "3";
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
        return false;
//    qDebug() << "4";
    // дать команду перехода на новую конфигурацию
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
        return false;
    OkMsgSetVisible(MSG_7_3_7_2);
    // 2. выдать сообщение об установке 60 В 1 А
    MsgSetVisible(MSG_7_3_7_3);
    Show1RetomDialog(60, 1);
    WaitNSeconds(15);
    // 3. получить аналоговые данные
    if (!Start7_3_2(MSG_7_3_7_3))
    {
        ErMsgSetVisible(MSG_7_3_7_3);
        return false;
    }
    // 4. ввести показания (с МИП или вручную)
    if (!GetExternalData(MSG_7_3_7_3))
        return false;
    // 5. рассчитать новые коэффициенты
    OkMsgSetVisible(MSG_7_3_7_3);
    MsgSetVisible(MSG_7_3_7_5);
    for (int i=0; i<3; i++)
    {
        if (pc.ModuleBsi.MType1 == MTE_1T1N)
        {
            Bac_newblock.KmU[i] = Bac_block.KmU[i] * mipd[i] / Bda_block.IUefNat_filt[i];
            Bac_newblock.DPsi[i+3] = Bac_block.DPsi[i+3] + Bac_newblock.DPsi[i] - Bac_block.DPsi[i] \
                    + mipd[i+6] - (180*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        }
        else
            Bac_newblock.KmI_1[i] = Bac_block.KmI_1[i] * mipd[i+3] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_1[i+3] = Bac_block.KmI_1[i+3] * mipd[i+3] / Bda_block.IUefNat_filt[i+3];
    }
    OkMsgSetVisible(MSG_7_3_7_5);
    // 6. установить в конфигурации токи i2nom в 5 А и перезагрузить модуль
    MsgSetVisible(MSG_7_3_7_6);
    for (int i=0; i<6; i++)
        econf->Bci_block.inom2[i] = 5.0;
    cn->Send(CN_WF, &econf->Bci_block, sizeof(e_config::Bci), 2, econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
    {
        ErMsgSetVisible(MSG_7_3_7_6);
        return false;
    }
    // перейти на новую конфигурацию
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
    {
        ErMsgSetVisible(MSG_7_3_7_6);
        return false;
    }
    OkMsgSetVisible(MSG_7_3_7_6);
    // 2. выдать сообщение об установке 60 В 5 А
    MsgSetVisible(MSG_7_3_7_8);
    Show1RetomDialog(60, 5);
    WaitNSeconds(15);
    // 3. получить аналоговые данные
    if (!Start7_3_2(MSG_7_3_7_8))
    {
        ErMsgSetVisible(MSG_7_3_7_8);
        return false;
    }
    // 4. ввести показания (с МИП или вручную)
//    if (!GetExternalData(MSG_7_3_7_8))
//        return false;
    // 5. рассчитать новые коэффициенты
    OkMsgSetVisible(MSG_7_3_7_8);
    MsgSetVisible(MSG_7_3_7_10);
    for (int i=0; i<3; i++)
    {
        if (pc.ModuleBsi.MType1 == MTE_2T0N)
            Bac_newblock.KmI_5[i] = Bac_block.KmI_5[i] * mipd[i+3] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_5[i+3] = Bac_block.KmI_5[i+3] * mipd[i+3] / Bda_block.IUefNat_filt[i+3];
    }
    OkMsgSetVisible(MSG_7_3_7_10);
    return true;
}

bool e_tunedialog::Start7_3_8()
{
    MsgSetVisible(MSG_7_3_8_1);
    // 1. Отправляем настроечные параметры в модуль
    cn->Send(CN_Wac, &Bac_newblock, sizeof(Bac));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
    {
        ErMsgSetVisible(MSG_7_3_8_1);
        return false;
    }
    // переходим на новую конфигурацию
    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        OkMsgSetVisible(MSG_7_3_8_1);
    else
    {
        ErMsgSetVisible(MSG_7_3_8_1);
        return false;
    }
    // 2. Проверяем измеренные напряжения
    MsgSetVisible(MSG_7_3_8_2);
    WaitNSeconds(15);
    if (!Start7_3_2(MSG_7_3_8_2))
    {
        ErMsgSetVisible(MSG_7_3_8_2);
        return false;
    }
    OkMsgSetVisible(MSG_7_3_8_2);
    return true;
}

bool e_tunedialog::Start7_3_9()
{
    MsgSetVisible(MSG_7_3_9);
    if (QMessageBox::question(this,"Закончить?","Закончить настройку?",QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == \
            QMessageBox::Yes)
    {
        if (!LoadWorkConfig())
        {
            ErMsgSetVisible(MSG_7_3_9);
            return false;
        }
        // переходим на прежнюю конфигурацию
        cn->Send(CN_Cnc);
        while (cn->Busy)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (cn->result != CN_OK)
            return false;
        // измеряем и проверяем
        Show1RetomDialog(60.0f, 1.0);
        WaitNSeconds(15);
        if (!Start7_3_2(MSG_7_3_9))
        {
            ErMsgSetVisible(MSG_7_3_9);
            return false;
        }
        OkMsgSetVisible(MSG_7_3_9);
    }
    else
        return false;
    if (QMessageBox::question(this,"Вопрос","Очистить память осциллограмм?",QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == \
            QMessageBox::Yes)
    {
        cn->Send(CN_OscEr);
        while (cn->Busy)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (cn->result == CN_OK)
            ETUNEINFO("Стёрто успешно");
        else
            ETUNEINFO("Ошибка при стирании");
    }
    return true;
}

bool e_tunedialog::SaveWorkConfig()
{
    econf = new e_config;

    cn->Send(CN_GF,NULL,0,1,econf->Config);
//    qDebug() << "1";
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        memcpy(&Bci_block_work,&econf->Bci_block,sizeof(e_config::Bci));
    else
        return false;
    return true;
}

bool e_tunedialog::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&econf->Bci_block,&Bci_block_work,sizeof(e_config::Bci));
    cn->Send(CN_WF, &econf->Bci_block, sizeof(econf->Bci_block), 2, econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
        return false;
    return true;
}

bool e_tunedialog::GetExternalData(int numexc)
{
    switch (TuneControlType)
    {
    case TUNEMIP:
    {
        StartMip();
/*        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 5000)
            qApp->processEvents(); */
        WaitNSeconds(5);
        OkMsgSetVisible(numexc);
        StopMip();
        if (CheckMip())
        {
            for (int i=0; i<3; i++)
            {
                mipd[i] = MipDat[i+4];
                mipd[i+3] = MipDat[i+7];
                mipd[i+6] = MipDat[i+11];
            }
            return true;
        }
        return false;
        break;
    }
    case TUNEMAN:
    {
        QDialog *dlg = new QDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setObjectName("dlg7371");
        QGridLayout *glyout = new QGridLayout;
        QLabel *lbl = new QLabel("Введите значения сигналов по приборам");
        glyout->addWidget(lbl,0,0,1,6);
        for (int i=0; i<3; i++) // for A to C
        {
            lbl = new QLabel("Uф" + QString::number(i+10,36).toUpper()+", В");
            glyout->addWidget(lbl,1,i*2,1,1);
            QDoubleSpinBox *spb = new QDoubleSpinBox;
            spb->setDecimals(5);
            spb->setValue(60.0);
            spb->setObjectName("spb7371"+QString::number(i));
            glyout->addWidget(spb,1,i*2+1,1,1);
        }
        if (numexc != MSG_7_3_7_1_1) // ввод всех параметров, не только напряжений
        {
            for (int i=0; i<3; i++) // for A to C
            {
                lbl = new QLabel("Iф" + QString::number(i+10,36).toUpper()+", A");
                glyout->addWidget(lbl,2,i*2,1,1);
                QDoubleSpinBox *spb = new QDoubleSpinBox;
                spb->setDecimals(5);
                spb->setValue(econf->Bci_block.inom2[i]);
                spb->setObjectName("spb7371"+QString::number(i+3));
                glyout->addWidget(spb,2,i*2+1,1,1);
            }
            for (int i=0; i<3; i++) // for A to C
            {
                lbl = new QLabel("Уг.нагр. ф" + QString::number(i+10,36).toUpper()+", град");
                glyout->addWidget(lbl,3,i*2,1,1);
                QDoubleSpinBox *spb = new QDoubleSpinBox;
                spb->setDecimals(3);
                spb->setMinimum(-360.0);
                spb->setMaximum(360.0);
                spb->setValue(0.0);
                spb->setObjectName("spb7371"+QString::number(i+6));
                glyout->addWidget(spb,3,i*2+1,1,1);
            }
        }
        QPushButton *pb = new QPushButton("Готово");
        connect(pb,SIGNAL(clicked()),this,SLOT(SetExtData()));
        glyout->addWidget(pb,4,0,1,3);
        pb = new QPushButton("Отмена");
        connect(pb,SIGNAL(clicked()),this,SLOT(CancelExtData()));
        glyout->addWidget(pb,4,3,1,3);
        dlg->setLayout(glyout);
        dlg->exec();
        if (Cancelled)
        {
            ErMsgSetVisible(numexc);
            return false;
        }
        OkMsgSetVisible(numexc);
        return true;
        break;
    }
    case TUNERET:
    {
        for (int i=0; i<3; i++)
        {
            mipd[i] = 60.0;
            mipd[i+3] = econf->Bci_block.inom2[i];
            mipd[i+6] = 0.0;
        }
        OkMsgSetVisible(numexc);
        return true;
        break;
    }
    }
    return false;
}

void e_tunedialog::SetExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == 0)
        return;
    for (int i=0; i<9; i++)
    {
        QDoubleSpinBox *spb = this->findChild<QDoubleSpinBox *>("spb7371"+QString::number(i));
        if (spb != 0)
            mipd[i] = spb->value();
    }
    Cancelled = false;
    dlg->close();
}

void e_tunedialog::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == 0)
        return;
    Cancelled = true;
    dlg->close();
}

// CHECKING

bool e_tunedialog::CheckTuneCoefs()
{
    double ValuesToCheck[26] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,\
                               0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0};
    double ThresholdsToCheck[26] = {0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,\
                                   0.5,0.5,0.5,0.5,0.5,0.5,1.0,1.0,1.0,1.0,1.0,1.0,0.5,0.005};
    bool res = true;
    for (int i = 0; i < 26; i++)
    {
        QLineEdit *lbl = this->findChild<QLineEdit *>("tune"+QString::number(i));
        if (lbl == 0)
            return false;
        bool ok;
        double vl = lbl->text().toDouble(&ok);
        if (!ok)
            return false;
        if (!IsWithinLimits(vl, ValuesToCheck[i], ThresholdsToCheck[i]))
        {
            ETUNEINFO("Настроечные по параметру "+QString::number(i)+". Измерено: "+QString::number(vl,'f',4)+\
                      ", должно быть: "+QString::number(ValuesToCheck[i],'f',4)+\
                      " +/- "+QString::number(ThresholdsToCheck[i],'f',4));
            res=false;
            lbl->setStyleSheet("QLabel {color: red};");
        }
    }
    return res;
    return true;
}

bool e_tunedialog::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

bool e_tunedialog::CheckMip()
{
    double ValuesToCheck[10] = {0,50.0,50.0,50.0,60.0,60.0,60.0,1.0,1.0,1.0};
    double ThresholdsToCheck[10] = {0,0.1,0.1,0.1,1.0,1.0,1.0,0.05,0.05,0.05};
    switch(pc.ModuleBsi.MType1) //
    {
    case MTE_2T0N:
    {
        ValuesToCheck[4] = ValuesToCheck[5] = ValuesToCheck[6] = 0;
        ThresholdsToCheck[4] = ThresholdsToCheck[5] = ThresholdsToCheck[6] = FLT_MAX;
        break;
    }
    case MTE_0T2N:
    {
        ValuesToCheck[7] = ValuesToCheck[8] = ValuesToCheck[9] = 0;
        ThresholdsToCheck[7] = ThresholdsToCheck[8] = ThresholdsToCheck[9] = FLT_MAX;
        break;
    }
    default:
        break;
    }
    for (int i=7; i<10; i++)
        ValuesToCheck[i] = econf->Bci_block.inom2[i-7]; // значения токов - по текущим номинальным значениям
    for (int i = 1; i < 10; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("mip"+QString::number(i));
        if (lbl == 0)
            return false;
        bool ok;
        double vl = lbl->text().toDouble(&ok);
        if (!ok)
            return false;
        if (!IsWithinLimits(vl,ValuesToCheck[i],ThresholdsToCheck[i]))
        {
            ETUNEINFO("Несовпадение МИП по параметру "+QString::number(i)+". Измерено: "+QString::number(vl,'f',4)+\
                      ", должно быть: "+QString::number(ValuesToCheck[i],'f',4)+\
                      " +/- "+QString::number(ThresholdsToCheck[i],'f',4));
            return false;
        }
    }
    return true;
}

bool e_tunedialog::CheckAnalogValues(int ntest)
{
    double ValuesToCheck[44] = {25.0,50.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,\
                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,\
                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    double ThresholdsToCheck[44] = {25.0,0.05,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,\
                                    1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,\
                                    1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
    switch (ntest)
    {
    case 600: // test 60, 0T2N
    case 605: // test 60, 0T2N, 5A
        break;
    case 601: // test 60, 1T1N
    {
        for (int i = 5; i<8; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+6] = 1.0; // i=1A
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.05; // +/- 0.05A
            ValuesToCheck[i+15] = ValuesToCheck[i+27] = ValuesToCheck[i+18] = \
                    ValuesToCheck[i+33] = 60.0; // P=S=60Вт
            ThresholdsToCheck[i+15] = ThresholdsToCheck[i+27] = ThresholdsToCheck[i+18] = \
                    ThresholdsToCheck[i+33] = 10; // 16%
            ValuesToCheck[i+21] = ValuesToCheck[i+30] = 0.0; // Q=0ВАр
            ThresholdsToCheck[i+21] = ThresholdsToCheck[i+30] = 10; // 16%
            ValuesToCheck[i+24] = ValuesToCheck[i+36] = 1.0; // CosPhi=1.0
            ThresholdsToCheck[i+24] = ThresholdsToCheck[i+36] = 0.01;
        }
        break;
    }
    case 602: // test 60, 2T0N
    {
        for (int i = 2; i<14; i++)
        {
            ValuesToCheck[i] = 1.0; // i=1A
            ThresholdsToCheck[i] = 0.05; // +/- 0.05A
        }
        break;
    }
    case 606: // test 60, 1I1U, 5A
    {
        for (int i = 2; i<5; i++)
        {
            ValuesToCheck[i+3] = ValuesToCheck[i+9] = 5.0; // i=1A
            ThresholdsToCheck[i+3] = ThresholdsToCheck[i+9] = 0.05; // +/- 0.05A
            ValuesToCheck[i+18] = ValuesToCheck[i+30] = ValuesToCheck[i+21] = \
                    ValuesToCheck[i+36] = 300.0; // P=S=60Вт
            ThresholdsToCheck[i+18] = ThresholdsToCheck[i+30] = ThresholdsToCheck[i+21] = \
                    ThresholdsToCheck[i+36] = 10; // 16%
            ValuesToCheck[i+24] = ValuesToCheck[i+33] = 0.0; // Q=0ВАр
            ThresholdsToCheck[i+24] = ThresholdsToCheck[i+33] = 10; // 16%
            ValuesToCheck[i+27] = ValuesToCheck[i+39] = 1.0; // CosPhi=1.0
            ThresholdsToCheck[i+27] = ThresholdsToCheck[i+39] = 0.01;
        }
        ValuesToCheck[14] = ValuesToCheck[17] = 0.0;
        ValuesToCheck[15] = ValuesToCheck[18] = -120.0;
        ValuesToCheck[16] = ValuesToCheck[19] = 120.0;
        break;
    }
    case 607: // test 60, 2T0N, 5A
    {
        for (int i = 2; i<14; i++)
        {
            ValuesToCheck[i] = 5.0; // i=5A
            ThresholdsToCheck[i] = 0.05; // +/- 0.05A
        }
        break;
    }
    case 615: // test 60, 0T2N, 5A, 0,1%
    {
        for (int i = 2; i<8; i++)
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.15; // +/- 0.15В
        ValuesToCheck[14] = ValuesToCheck[17] = 0.0;
        ValuesToCheck[15] = ValuesToCheck[18] = -120.0;
        ValuesToCheck[16] = ValuesToCheck[19] = 120.0;
        break;
    }
    case 616: // test 60, 1I1U, 5A, 0,1%
    {
        for (int i = 2; i<5; i++)
        {
            ValuesToCheck[i+18] = ValuesToCheck[i+30] = ValuesToCheck[i+21] = \
                    ValuesToCheck[i+36] = 300.0; // P=S=60Вт
            ThresholdsToCheck[i+18] = ThresholdsToCheck[i+30] = ThresholdsToCheck[i+21] = \
                    ThresholdsToCheck[i+36] = 30; // 10%
            ValuesToCheck[i+24] = ValuesToCheck[i+33] = 0.0; // Q=0ВАр
            ThresholdsToCheck[i+24] = ThresholdsToCheck[i+33] = 10; // 16%
            ValuesToCheck[i+27] = ValuesToCheck[i+39] = 1.0; // CosPhi=1.0
            ThresholdsToCheck[i+27] = ThresholdsToCheck[i+39] = 0.01;
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.15; // +/- 0.15В
        }
        for (int i = 5; i<8; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+6] = 5.0; // i=5A
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.01; // +/- 0.01A
        }
        ValuesToCheck[14] = ValuesToCheck[17] = 0.0;
        ValuesToCheck[15] = ValuesToCheck[18] = -120.0;
        ValuesToCheck[16] = ValuesToCheck[19] = 120.0;
        break;
    }
    case 617: // test 60, 2T0N, 5A, 0,1%
    {
        for (int i = 2; i<14; i++)
        {
            ValuesToCheck[i] = 5.0; // i=5A
            ThresholdsToCheck[i] = 0.01; // +/- 0.05A
        }
        ValuesToCheck[14] = ValuesToCheck[17] = 0.0;
        ValuesToCheck[15] = ValuesToCheck[18] = -120.0;
        ValuesToCheck[16] = ValuesToCheck[19] = 120.0;
        break;
    }
    case 570: // test 57.74, 0T2N, 1(5) A, 0,1 %
    {
        for (int i = 2; i<14; i++)
        {
            ValuesToCheck[i] = mipd[i-2]; // u=60,0В
            ThresholdsToCheck[i] = 0.1; // +/- 0.1В
        }
        ValuesToCheck[14] = ValuesToCheck[17] = 0.0;
        ValuesToCheck[15] = ValuesToCheck[18] = -120.0;
        ValuesToCheck[16] = ValuesToCheck[19] = 120.0;
        break;
    }
    case 571: // test 57.74, 1T1N
    {
        for (int i = 2; i<5; i++)
        {
            ValuesToCheck[i] = ValuesToCheck[i+6] = mipd[i-2]; // u=60,0В
            ThresholdsToCheck[i] = ThresholdsToCheck[i+6] = 0.1; // +/- 0.1В
            ValuesToCheck[i+3] = ValuesToCheck[i+9] = mipd[i+1]; // i=1(5)A
            ThresholdsToCheck[i+3] = ThresholdsToCheck[i+9] = 0.05; // +/- 0.05A
            ValuesToCheck[i+18] = ValuesToCheck[i+30] = ValuesToCheck[i+21] = \
                    ValuesToCheck[i+36] = mipd[i-2]*mipd[i+1]; // P=S=60.0*I, W
            ThresholdsToCheck[i+18] = ThresholdsToCheck[i+30] = ThresholdsToCheck[i+21] = \
                    ThresholdsToCheck[i+36] = mipd[i+1]*1.25; // 2%
            ValuesToCheck[i+24] = ValuesToCheck[i+33] = 0.0; // Q=0ВАр
            ThresholdsToCheck[i+24] = ThresholdsToCheck[i+33] = mipd[i+1]*1.25; // 2%
            ValuesToCheck[i+27] = ValuesToCheck[i+39] = 1.0; // CosPhi=1.0
            ThresholdsToCheck[i+27] = ThresholdsToCheck[i+39] = 0.01;
        }
        ValuesToCheck[14] = ValuesToCheck[17] = 0.0;
        ValuesToCheck[15] = ValuesToCheck[18] = -120.0;
        ValuesToCheck[16] = ValuesToCheck[19] = 120.0;
        break;
    }
    case 572: // 2T0N
    {
        for (int i = 2; i<14; i++)
        {
            ValuesToCheck[i] = mipd[i+1]; // i=1(5)A
            ThresholdsToCheck[i] = 0.05; // +/- 0.05A
        }
        ValuesToCheck[14] = ValuesToCheck[17] = 0.0;
        ValuesToCheck[15] = ValuesToCheck[18] = -120.0;
        ValuesToCheck[16] = ValuesToCheck[19] = 120.0;
        break;
    }
    }

    for (int i = 0; i < 44; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("value"+QString::number(i));
        if (lbl == 0)
            return false;
        bool ok;
        double vl = lbl->text().toDouble(&ok);
        if (!ok)
            return false;
        if (!IsWithinLimits(vl,ValuesToCheck[i],ThresholdsToCheck[i]))
        {
            ETUNEINFO("Несовпадение по параметру "+QString::number(i)+". Измерено: "+QString::number(vl,'f',4)+\
                      ", должно быть: "+QString::number(ValuesToCheck[i],'f',4)+\
                      " +/- "+QString::number(ThresholdsToCheck[i],'f',4));
            return false;
        }
    }
    return true;
}

void e_tunedialog::ReadTuneCoefs()
{
    cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        WriteTuneCoefsToGUI();
}

void e_tunedialog::WriteTuneCoefs()
{
    ReadTuneCoefsFromGUI();
    cn->Send(CN_Wac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        ETUNEINFO("Записано успешно!");
}

void e_tunedialog::SetNewTuneCoefs()
{
    Bac_newblock.Kinter = Bac_block.Kinter;
    Bac_newblock.K_freq = Bac_block.K_freq;
    for (int i=0; i<6; i++)
    {
        Bac_newblock.DPsi[i] = Bac_block.DPsi[i];
        Bac_newblock.KmI_1[i] = Bac_block.KmI_1[i];
        Bac_newblock.KmI_5[i] = Bac_block.KmI_5[i];
        Bac_newblock.KmU[i] = Bac_block.KmU[i];
    }
}

void e_tunedialog::SetDefCoefs()
{
    memcpy(&Bac_block, &Bac_defblock, sizeof(Bac));
    WriteTuneCoefsToGUI();
}

void e_tunedialog::WriteTuneCoefsToGUI()
{
    QLineEdit *le;
    for (int i = 0; i < 6; i++)
    {
        le = this->findChild<QLineEdit *>("tune"+QString::number(i));
        if (le == 0)
            return;
        le->setText(QString::number(Bac_block.KmU[i], 'f', 5));
        le = this->findChild<QLineEdit *>("tune"+QString::number(i+6));
        if (le == 0)
            return;
        le->setText(QString("%1").arg(Bac_block.KmI_5[i], 0, 'f', 5));
        le = this->findChild<QLineEdit *>("tune"+QString::number(i+12));
        if (le == 0)
            return;
        le->setText(QString::number(Bac_block.KmI_1[i], 'f', 5));
        le = this->findChild<QLineEdit *>("tune"+QString::number(i+18));
        if (le == 0)
            return;
        le->setText(QString::number(Bac_block.DPsi[i], 'f', 5));
    }
    le = this->findChild<QLineEdit *>("tune24");
    if (le == 0)
        return;
    le->setText(QString::number(Bac_block.K_freq, 'f', 5));
    le = this->findChild<QLineEdit *>("tune25");
    if (le == 0)
        return;
    le->setText(QString::number(Bac_block.Kinter, 'f', 5));
}

void e_tunedialog::ReadTuneCoefsFromGUI()
{
    QLineEdit *le;
    for (int i = 0; i < 6; i++)
    {
        le = this->findChild<QLineEdit *>("tune"+QString::number(i));
        if (le == 0)
            return;
        bool ok;
        Bac_block.KmU[i]=le->text().toFloat(&ok);
        if (!ok)
        {
            ETUNEER("Значение "+le->text()+" не может быть переведено во float");
            return;
        }
        le = this->findChild<QLineEdit *>("tune"+QString::number(i+6));
        if (le == 0)
            return;
        Bac_block.KmI_5[i]=le->text().toFloat(&ok);
        if (!ok)
        {
            ETUNEER("Значение "+le->text()+" не может быть переведено во float");
            return;
        }
        le = this->findChild<QLineEdit *>("tune"+QString::number(i+12));
        if (le == 0)
            return;
        Bac_block.KmI_1[i]=le->text().toFloat(&ok);
        if (!ok)
        {
            ETUNEER("Значение "+le->text()+" не может быть переведено во float");
            return;
        }
        le = this->findChild<QLineEdit *>("tune"+QString::number(i+18));
        if (le == 0)
            return;
        Bac_block.DPsi[i]=le->text().toFloat(&ok);
        if (!ok)
        {
            ETUNEER("Значение "+le->text()+" не может быть переведено во float");
            return;
        }
    }
    le = this->findChild<QLineEdit *>("tune24");
    if (le == 0)
        return;
    bool ok;
    Bac_block.K_freq=le->text().toFloat(&ok);
    if (!ok)
    {
        ETUNEER("Значение "+le->text()+" не может быть переведено во float");
        return;
    }
    le = this->findChild<QLineEdit *>("tune25");
    if (le == 0)
        return;
    Bac_block.Kinter=le->text().toFloat(&ok);
    if (!ok)
    {
        ETUNEER("Значение "+le->text()+" не может быть переведено во float");
        return;
    }
}

void e_tunedialog::RefreshAnalogValues()
{
    QLabel *lbl;
    lbl = this->findChild<QLabel *>("value0");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bda_block.Tmk, 'f', 1));
    lbl = this->findChild<QLabel *>("value1");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bda_block.Frequency, 'f', 4));
    for (int i = 0; i < 6; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+2));
        if (lbl == 0)
            return;
        int Precision = (pc.ModuleBsi.MType1 == MTE_0T2N) ? 3 : 4;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+8));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 == MTE_2T0N) ? 4 : 3;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+14));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.phi_next_f[i], 'f', 3));
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+20));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.PNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+23));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.SNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+26));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.QNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+29));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhiNat[i], 'f', 4));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+32));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Pf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+35));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Qf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+38));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Sf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+41));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhi[i], 'f', 4));
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+44));
        if (lbl == 0)
            return;
        float PHI = (180*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        lbl->setText(QString::number(PHI, 'f', 3));
    }
}

void e_tunedialog::StartAnalogMeasurementsToFile()
{
    QString Filename = QFileDialog::getSaveFileName(this,"Сохранить данные","","Excel files (*.xlsx)");
    if (Filename == "")
    {
        ETUNEER("Не задано имя файла");
        return; // !!! ошибка - не задано имя файла
    }
    XlsxWriting = true;
    xlsx = new QXlsx::Document(Filename);
    xlsx->write(1,1,QVariant("Модуль: "+pc.ModuleTypeString+" сер. ном. "+QString::number(pc.ModuleBsi.SerNum,10)));
    xlsx->write(2,1,QVariant("Дата начала записи: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3,1,QVariant("Время начала записи: "+QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5,1,QVariant("Дата и время отсчёта"));
    for (int i=0; i<3; i++)
    {
        if (pc.ModuleBsi.MType1 != MTE_2T0N)
            xlsx->write(5,i+2,QVariant(("U1 ф")+QString::number(i+10, 36)+", В"));
        else
            xlsx->write(5,i+2,QVariant("I1 ф"+QString::number(i+10, 36)+", А"));

        if (pc.ModuleBsi.MType1 != MTE_0T2N)
            xlsx->write(5,i+5,QVariant("I2 ф"+QString::number(i+10, 36)+", А"));
        else
            xlsx->write(5,i+5,QVariant("U2 ф"+QString::number(i+10, 36)+", В"));

        xlsx->write(5,i+8,QVariant("Phi ф"+QString::number(i+10, 36)+", град"));
        xlsx->write(5,i+11,QVariant("Pf ф"+QString::number(i+10, 36)+", Вт"));
        xlsx->write(5,i+14,QVariant("Qf ф"+QString::number(i+10, 36)+", ВА"));
        xlsx->write(5,i+17,QVariant("Sf ф"+QString::number(i+10, 36)+", ВА"));
    }
    xlsx->write(5,20,QVariant("f, Гц"));
    xlsx->write(5,21,QVariant("t, град"));
    WRow = 6;
    QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    pb = this->findChild<QPushButton *>("pbmeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    ElapsedTimeCounter = new QTime;
    ElapsedTimeCounter->start();
    StartAnalogMeasurements();
}

void e_tunedialog::StartAnalogMeasurements()
{
    tmr->start();
}

void e_tunedialog::StopAnalogMeasurements()
{
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            ETUNEINFO("Файл создан успешно");
            delete xlsx;
        }
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != 0)
        {
            pb->setEnabled(true);
            pb->setText("Запустить чтение аналоговых сигналов в файл");
        }
        pb = this->findChild<QPushButton *>("pbmeasurements");
        if (pb != 0)
            pb->setEnabled(true);
        XlsxWriting = false;
    }
    tmr->stop();
}

void e_tunedialog::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBdi, &Bda_block, sizeof(Bda_block));
    while (cn->Busy)
        qApp->processEvents();
    if (cn->result != CN_OK)
    {
        ETUNEINFO("Ошибка при приёме данных");
        return;
    }
    // обновление коэффициентов в соответствующих полях на экране
    RefreshAnalogValues();
    if (XlsxWriting)
    {
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != 0)
        {
            int MSecs = ElapsedTimeCounter->elapsed();
            QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss");
            pb->setText("Идёт запись: "+TimeElapsed);
        }
        QXlsx::Format format;
        xlsx->write(WRow,1,QVariant(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss")));
        for (int i=0; i<3; i++)
        {
            QString Precision = (pc.ModuleBsi.MType1 != MTE_2T0N) ? "0.000" : "0.0000";
            format.setNumberFormat(Precision);
            xlsx->write(WRow,i+2,Bda_block.IUeff_filtered[i],format);

            Precision = (pc.ModuleBsi.MType1 != MTE_0T2N) ? "0.0000" : "0.000";
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
        xlsx->write(WRow,21,Bda_block.Tmk,format);
        WRow++;
    }
}

void e_tunedialog::StartMip()
{
    mipcanal = new iec104;
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
    static int Thresholds[34] = {0,4,4,4,3,3,3,4,4,4,4,3,3,3,3,3,3,1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.SigNum;
    if (index != -1)
    {
        if ((index >= 11) && (index <= 13))
            MipDat[index] = -MipDat[index]; // у МИП-а знак угла отрицательный
        QLabel *lbl = this->findChild<QLabel *>("mip"+QString::number(index));
        if (lbl != 0)
            lbl->setText(QString::number(Signal.SigVal, 'f', Thresholds[index]));
        MipDat[index] = Signal.SigVal;
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

void e_tunedialog::ShowControlChooseDialog()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Выберите метод подтверждения измеряемых данных:");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Автоматически по показаниям МИП-02");
    TuneControlType = TUNERET; // по-умолчанию тип контроля - по РЕТОМу
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneMip()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Вручную");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneManual()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Автоматически по прибору РЕТОМ");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneRetom()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void e_tunedialog::Show1PhaseScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPixmap pmp;
    switch(pc.ModuleBsi.MType1) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case MTE_2T0N:
    {
        pmp.load(":/mte_2t0n.png");
        break;
    }
    case MTE_1T1N:
    {
        pmp.load(":/mte_1t1n.png");
        break;
    }
    case MTE_0T2N:
    {
        pmp.load(":/mte_0t2n.png");
        break;
    }
    default:
        return;
        break;
    }
    QLabel *lbl = new QLabel("Соберите схему подключения по нижеследующей картинке:");
    QLabel *lblpmp = new QLabel;
    lblpmp->setPixmap(pmp);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(lbl);
    lyout->addWidget(lblpmp);
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void e_tunedialog::Show1RetomDialog(float U, float A)
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    int count = 1;
    QLabel *lbl = new QLabel(QString::number(count)+".Вызовите программу управления РЕТОМ и войдите в режим \"Ручное управление выходами\";");
    lyout->addWidget(lbl);
    count++;
    lbl = new QLabel(QString::number(count)+".На выходах РЕТОМ задайте частоту 50,0 Гц, трёхфазные напряжения на уровне " \
                             + QString::number(U) + " В с фазой 0 градусов;");
    lyout->addWidget(lbl);
    count++;
    if (pc.ModuleBsi.MType1!=MTE_0T2N)
    {
        lbl=new QLabel(QString::number(count)+".Задайте трёхфазные токи на уровне "+QString::number(A)+",0 А с фазой 0 градусов;");
        lyout->addWidget(lbl);
        count++;
    }
    if (TuneControlType == TUNEMIP)
    {
        lbl=new QLabel(QString::number(count)+".Включите питание прибора МИП-02;");
        lyout->addWidget(lbl);
        count++;
    }
    lbl=new QLabel(QString::number(count)+".Включите выходы РЕТОМ");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void e_tunedialog::Show3PhaseScheme()
{
    // высвечиваем надпись "Проверка связи РЕТОМ и МИП"
    MsgSetVisible(MSG_7_3_6_0);
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPixmap pmp;
    switch(pc.ModuleBsi.MType1) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case MTE_2T0N:
    {
        pmp.load(":/mte3-e2t0n.png");
        break;
    }
    case MTE_1T1N:
    {
        pmp.load(":/mte3-e1t1n.png");
        break;
    }
    case MTE_0T2N:
    {
        pmp.load(":/mte3-e2t0n.png");
        break;
    }
    default:
        return;
        break;
    }
    QLabel *lblpmp = new QLabel;
    lblpmp->setPixmap(pmp);
    lyout->addWidget(lblpmp);
    QLabel *lbl = new QLabel("1. Отключите выходы РЕТОМ;");
    lyout->addWidget(lbl);
    lbl = new QLabel("2. Соберите схему подключения по вышеприведённой картинке;");
    lyout->addWidget(lbl);
    lbl=new QLabel("3. Задайте на РЕТОМ трёхфазный режим токов и напряжений с углами "\
                   " сдвига в фазах А: 0 град., в фазах В: 240 град., в фазах С: 120 град.;");
    lyout->addWidget(lbl);
    lbl = new QLabel("4. Включите выходы РЕТОМ (не меняя значений напряжения и тока!)");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    OkMsgSetVisible(MSG_7_3_6_0);
}

void e_tunedialog::MsgClear()
{
    for (int i=0; i<MSG_COUNT; i++)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    }
}

void e_tunedialog::MsgSetVisible(int msg, bool Visible)
{
    QLabel *lbl = this->findChild<QLabel *>("tunemsg"+QString::number(msg));
    if (lbl == 0)
    {
        ETUNEDBG;
        return;
    }
    lbl->setVisible(Visible);
}

void e_tunedialog::OkMsgSetVisible(int msg, bool Visible)
{
    QLabel *lbl=this->findChild<QLabel *>("tunemsgres"+QString::number(msg));
    if (lbl == 0)
    {
        ETUNEDBG;
        return;
    }
    lbl->setStyleSheet("QLabel {color: black};");
    lbl->setText("готово!");
    lbl->setVisible(Visible);
}

void e_tunedialog::ErMsgSetVisible(int msg, bool Visible)
{
    QLabel *lbl=this->findChild<QLabel *>("tunemsgres"+QString::number(msg));
    if (lbl == 0)
    {
        ETUNEDBG;
        return;
    }
    lbl->setStyleSheet("QLabel {color: red};");
    lbl->setText("ошибка!");
    lbl->setVisible(Visible);
}

void e_tunedialog::CancelTune()
{
    Cancelled = true;
}

void e_tunedialog::LoadFromFile()
{
    QByteArray ba = pc.LoadFile("Tune files (*.etn)");
    memcpy(&Bac_block,&(ba.data()[0]),sizeof(Bac_block));
    WriteTuneCoefsToGUI();
    ETUNEINFO("Загрузка прошла успешно!");
}

void e_tunedialog::SaveToFile()
{
    ReadTuneCoefsFromGUI();
    int res = pc.SaveFile("Tune files (*.etn)", &Bac_block, sizeof(Bac_block));
    switch (res)
    {
    case 0:
        ETUNEINFO("Записано успешно!");
        break;
    case 1:
        ETUNEER("Ошибка при записи файла!");
        break;
    case 2:
        ETUNEER("Пустое имя файла!");
        break;
    case 3:
        ETUNEER("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void e_tunedialog::SetTimerPeriod()
{
    bool TimerIsActive = false;
    if (tmr->isActive())
        TimerIsActive = true;
    bool ok;
    int per = sender()->objectName().toInt(&ok);
    if (!ok)
        return;
    tmr->stop();
    tmr->setInterval(per);
    if (TimerIsActive)
        tmr->start();
}

void e_tunedialog::WaitNSeconds(int Seconds)
{
    QTime tme;
    Seconds *= 1000;
    WaitWidget *w = new WaitWidget;
    QTimer *tmr = new QTimer;
    float RandInterval = (float)qrand()/RAND_MAX*1000.0;
    tmr->setInterval(RandInterval);
    connect(this,SIGNAL(SecondsRemaining(QString)),w,SLOT(SetMessage(QString)));
    connect(tmr,SIGNAL(timeout()),this,SLOT(UpdateNSecondsWidget()));
    tmr->start();
    w->Start();
    tme.start();
    while (tme.elapsed() < Seconds)
    {
        SecondsToEnd15SecondsInterval = Seconds - tme.elapsed();
        qApp->processEvents();
    }
    w->Stop();
    tmr->stop();
}

void e_tunedialog::UpdateNSecondsWidget()
{
    QString tmps = "Подождите " + QString::number(SecondsToEnd15SecondsInterval--) + " мс";
    emit SecondsRemaining(tmps);
}
