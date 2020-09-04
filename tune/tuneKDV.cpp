#include <QTime>
#include <QtMath>
#include <QInputDialog>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QCoreApplication>
#include <QThread>
#include "tuneKDV.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/maindef.h"
#include "../check/checkKDV.h"
#include "../gen/maindef.h"
#include "../gen/modulebsi.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include <QScrollArea>
#include <QScrollBar>
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

TuneDialogKDV::TuneDialogKDV(QVector<S2::DataRec> *S2Config, QWidget *parent) : EAbstractTuneDialog(parent)
{
    this->S2ConfigForTune = S2Config;
    int i;

    CKDV = new ConfigKDV(*S2ConfigForTune);
    ReportModel = new QStandardItemModel;
    ViewModel = new QStandardItemModel;
    ledit = new QLineEdit;
    ask = new QDialog;
    TempCor = 0;
    inc = 0;
//    Ch80 = new Check80;
    SetBac(&Bac_block1, 1, sizeof(Bac_block1));
    SetBac(&Bac_block2, 2, sizeof(Bac_block2));
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();

    for (i=0; i<3; i++)
    {
        Bac_newblock2.KmU1[i] = 0;
        Bac_newblock2.KmU2[i] = 0;
        Bac_newblock2.KmU4[i] = 0;
        Bac_newblock2.KmI1[i] = 0;
        Bac_newblock2.KmI2[i] = 0;
        Bac_newblock2.KmI4[i] = 0;
        Bac_newblock2.KmI8[i] = 0;
        Bac_newblock2.KmI16[i] = 0;
        Bac_newblock2.KmI32[i] = 0;
        Bac_newblock2.DPsi[i] = 0;
        Bac_newblock2.DPsi[i+3] = 0;
    }

    Bac_newblock1.Art = 0;
    Bac_newblock1.K_freq = 0;
    Bac_newblock1.Brt = 0;
}

void TuneDialogKDV::SetupUI()
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *cp1 = new QWidget;// = TuneUI();

    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *Hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Настроить канал Pt100");
    #if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(TunePt100Channel()));
    #endif

    glyout->addWidget(pb, 1,1,1,1);

    glyout->addWidget(TuneUI(), 3,1,1,1);

    pb = new QPushButton("Настройка температурной коррекции");
    #if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(TuneTemp()));
    #endif

    glyout->addWidget(pb, 2,1,1,1);

    pb = new QPushButton("Начать поверку");
    #if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(GenerateReport()));
    #endif
    glyout->addWidget(pb, 15,1,1,1);
    lyout->addLayout(glyout);

    cp1->setLayout(lyout);

    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    #if PROGSIZE != PROGSIZE_EMUL
    QWidget *cp4 = Bd1W(this);
    #endif
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);

    QLabel *lbl;
    glyout = new QGridLayout;
    QTabWidget *TuneTW = new QTabWidget;
    QScrollArea *area = new QScrollArea;
    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::WinPanel);
    area->setWidgetResizable(true);

    TuneTW->addTab(cp1,"Настройка");
    #if PROGSIZE != PROGSIZE_EMUL
    TuneTW->addTab(cp4,"Измеренные параметры");
    #endif
    TuneTW->addTab(area,"Коэффициенты");
    //TuneTW->addTab(cp3,"Данные МИП");


    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ
    lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 3; i++)
    {
        //lbl = new QLabel("N1_TT["+QString::number(i)+"]");
        //glyout->addWidget(lbl,0,i,1,1);
        //QLineEdit *le = new QLineEdit("");
        //le->setObjectName("tune"+QString::number(i));
        //le->setStyleSheet(ValuesLEFormat);
        //glyout->addWidget(le,1,i,1,1);
        lbl = new QLabel("KmU1["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,1,i,1,1);
        lbl = new QLabel("KmU2["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+3));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,1,i+3,1,1);
        lbl = new QLabel("KmU4["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+6));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,3,i,1,1);
        lbl = new QLabel("KmI1["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+9));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,3,i+3,1,1);
        lbl = new QLabel("KmI2["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+12));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,5,i,1,1);
        lbl = new QLabel("KmI4["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+15));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,5,i+3,1,1);
        lbl = new QLabel("KmI8["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+18));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,7,i,1,1);
        lbl = new QLabel("KmI16["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+21));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,7,i+3,1,1);
        lbl = new QLabel("KmI32["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+24));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,9,i,1,1);
        /*lbl = new QLabel("TKPsi_a["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+27));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,9,i,1,1);
        lbl = new QLabel("TKPsi_b["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+27));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,9,i+3,1,1);*/
    }

    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("DPsi["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i,1,1);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+27));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,11,i,1,1);
        /*lbl = new QLabel("TKUa["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+36));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,13,i,1,1);
        lbl = new QLabel("TKUb["+QString::number(i)+"]");
        glyout->addWidget(lbl,14,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+42));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,15,i,1,1);*/
    }

    lbl=new QLabel("K_freq");
    //lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,12,0,1,1);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName("tune33");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,13,0,1,1);
    lbl=new QLabel("Art");
    //lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,12,1,1,1);
    le = new QLineEdit("");
    le->setObjectName("tune34");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,13,1,1,1);
    lbl=new QLabel("Brt");
    //lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,12,2,1,1);
    le = new QLineEdit("");
    le->setObjectName("tune35");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,13,2,1,1);

    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addWidget(BottomUI(BoardTypes::BT_MEZONIN));
    lyout->addStretch(1);
    cp2->setLayout(lyout);
    area->setWidget(cp2);
    //lyout->addWidget(area);
    //cp2->setLayout(lyout);
    //lyout->addWidget(area);
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());

    // CP3 - ПОКАЗАНИЯ МИП-02

    /*QVBoxLayout *vlyout = new QVBoxLayout;
    QVBoxLayout *gblyout = new QVBoxLayout;
    gb = new QGroupBox("Измеряемые параметры");
    QHBoxLayout *hlyout = MipPars(1, "Частота");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(4, "Фазное напряжение");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(7, "Фазный ток");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(11, "Угол нагрузки");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(43, "Фазовый угол напряжения");
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
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
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    vlyout->addWidget(gb);

    gb = new QGroupBox("Вычисляемые параметры");
    hlyout = MipPars(22, "Активная мощность");
    gblyout = new QVBoxLayout;
    gblyout->addLayout(hlyout);
    hlyout = MipPars(26, "Реактивная мощность");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(30, "Полная мощность");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(19, "Линейное напряжение");
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
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
    gblyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl = new QLabel("33. Полная:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip33");
    lbl->setToolTip("Параметр 33");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    vlyout->addWidget(gb);
    #if PROGSIZE != PROGSIZE_EMUL
    hlyout = new QHBoxLayout;
    pb = new QPushButton("Запустить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMip()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMip()));
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
    #endif
    cp3->setLayout(vlyout);*/

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);

}

QHBoxLayout *TuneDialogKDV::MipPars(int parnum, const QString &groupname)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QLabel *lbl = new QLabel(QString::number(parnum) + "." + groupname + ": ");
    hlyout->addWidget(lbl);
    for (int i = 0; i < 3; i++)
    {
        QLabel *lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+parnum));
        lbl->setToolTip("Параметр "+QString::number(i+parnum));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    return hlyout;
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialogKDV::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Сохранение текущей конфигурации...");
    lbls.append("3. Отображение диалога выбора режима контроля показаний...");
    lbls.append("4. Отображение схемы подключения...");
    lbls.append("5. Информация...");
    lbls.append("6. Установка коэффициентов...");
    lbls.append("7. 7_3_2. Переход на новую конфигурацию 74В и 7.8А...");
    lbls.append("8. 7.3.4. Информация...");
    lbls.append("9. 7.3.4.2. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току и напряжению для Кацп=4...");
    lbls.append("10. 7.3.4.6 Переход на новую конфигурацию 290В и 31А...");
    lbls.append("11. 7.3.4.7. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по частоте и фазным углам,\n" \
                "а также по току и напряжению для Кацп=1...");
    lbls.append("12. 7.3.4.11. Переход на новую конфигурацию 145В и 15.5А...");
    lbls.append("13. 7.3.4.12. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току и напряжению для Кацп=2...");
    lbls.append("14. 7.3.4.13. Переход на новую конфигурацию 300В и 3.9А...");
    lbls.append("15. 7.3.4.14. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=8...");
    lbls.append("16. 7.3.4.15. Переход на новую конфигурацию 300В и 1.9А...");
    lbls.append("17. 7.3.4.16. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=16...");
    lbls.append("18. 7.3.4.17. Переход на новую конфигурацию 300В и 0.9А...");
    lbls.append("19. 7.3.4.18. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=32...");
    lbls.append("20. 7.3.5. Восстановление сохранённой конфигурации и проверка...");
}

void TuneDialogKDV::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::SaveWorkConfig); // Сохранение текущей конфигурации
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::ShowControlChooseDialog); // Отображение диалога выбора режима контроля показаний
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Show3PhaseScheme); // Отображение схемы подключения
    pf[lbls.at(count++)] = func;
    //func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_2_3); // Проверка связи РЕТОМ и МИП
    //pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_1); // Информация
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::SetNewTuneCoefs); // Установка коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4); // Информация
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_2); // Расчёт коррекции по фазе
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_6); // Расчёт коррекции по частоте
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_7); // Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=2
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_11); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_12); // Расчёт коррекции взаимного влияния каналов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_13); // Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_14); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_15); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_16); // Ввод измеренных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_17); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_4_18); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKDV::Start7_3_5); // Восстановление сохранённой конфигурации и проверка
    pf[lbls.at(count++)] = func;
}


#endif

void TuneDialogKDV::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
    for (int i = 0; i < 3; i++)
    {
        //WDFunc::SetLEData(this, QString::number(Bac_block.N1_TT[i]), "tune"+QString::number(i));  //'g', 5
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_block2.KmU1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(Bac_block2.KmU2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_block2.KmU4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(Bac_block2.KmI1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+12), QString::number(Bac_block2.KmI2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+15), QString::number(Bac_block2.KmI4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+18), QString::number(Bac_block2.KmI8[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+21), QString::number(Bac_block2.KmI16[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+24), QString::number(Bac_block2.KmI32[i], 'f', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+27), QString::number(Bac_block2.TKPsi_a[i], 'e', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+30), QString::number(Bac_block2.TKPsi_b[i], 'e', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i+27), QString::number(Bac_block2.DPsi[i], 'f', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+36), QString::number(Bac_block.TKUa[i], 'e', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+42), QString::number(Bac_block.TKUb[i], 'e', 5));
    }

    WDFunc::SetLEData(this, "tune33", QString::number(Bac_block1.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune34", QString::number(Bac_block1.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune35", QString::number(Bac_block1.Brt, 'f', 5));
    //WDFunc::SetLEData(this, "tune51", QString::number(Bac_block.Tmk0, 'f', 5));

}

void TuneDialogKDV::FillNewBac()
{
    for (int i = 0; i < 3; i++)
    {
        //WDFunc::SetLEData(this, QString::number(Bac_block.N1_TT[i]), "tune"+QString::number(i));  //'g', 5
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_newblock2.KmU1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(Bac_newblock2.KmU2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_newblock2.KmU4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(Bac_newblock2.KmI1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+12), QString::number(Bac_newblock2.KmI2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+15), QString::number(Bac_newblock2.KmI4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+18), QString::number(Bac_newblock2.KmI8[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+21), QString::number(Bac_newblock2.KmI16[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+24), QString::number(Bac_newblock2.KmI32[i], 'f', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+27), QString::number(Bac_block2.TKPsi_a[i], 'e', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+30), QString::number(Bac_block2.TKPsi_b[i], 'e', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i+27), QString::number(Bac_newblock2.DPsi[i], 'f', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+36), QString::number(Bac_block.TKUa[i], 'e', 5));
        //WDFunc::SetLEData(this, "tune"+QString::number(i+42), QString::number(Bac_block.TKUb[i], 'e', 5));
    }

    WDFunc::SetLEData(this, "tune33", QString::number(Bac_newblock1.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune34", QString::number(Bac_newblock1.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune35", QString::number(Bac_block1.Brt, 'f', 5));
    //WDFunc::SetLEData(this, "tune51", QString::number(Bac_block.Tmk0, 'f', 5));
}

void TuneDialogKDV::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum);
    QString tmps;
    for (int i = 0; i < 3; i++)
    {
        //WDFunc::LE_read_data(this, "tune"+QString::number(i), tmps);
        //Bac_block.N1_TT[i]=tmps.toUInt();
        WDFunc::LE_read_data(this, "tune"+QString::number(i), tmps);
        Bac_block2.KmU1[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+3), tmps);
        Bac_block2.KmU2[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+6), tmps);
        Bac_block2.KmU4[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+9), tmps);
        Bac_block2.KmI1[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+12), tmps);
        Bac_block2.KmI2[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+15), tmps);
        Bac_block2.KmI4[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+18), tmps);
        Bac_block2.KmI8[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+21), tmps);
        Bac_block2.KmI16[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+24), tmps);
        Bac_block2.KmI32[i]=ToFloat(tmps);
        /*WDFunc::LE_read_data(this, "tune"+QString::number(i+27), tmps);
        Bac_block.TKPsi_a[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+27), tmps);
        Bac_block.TKPsi_b[i]=ToFloat(tmps);*/
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::LE_read_data(this, "tune"+QString::number(i+27), tmps);
        Bac_block2.DPsi[i]=ToFloat(tmps);
        /*WDFunc::LE_read_data(this, "tune"+QString::number(i+36), tmps);
        Bac_block.TKUa[i]=ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune"+QString::number(i+42), tmps);
        Bac_block.TKUb[i]=ToFloat(tmps);*/
    }

    WDFunc::LE_read_data(this, "tune33", tmps);
    Bac_block1.K_freq=ToFloat(tmps);
    WDFunc::LE_read_data(this, "tune34", tmps);
    Bac_block1.Art=ToFloat(tmps);
    WDFunc::LE_read_data(this, "tune35", tmps);
    Bac_block1.Brt=ToFloat(tmps);
    //WDFunc::LE_read_data(this, "tune51", tmps);
    //Bac_block.Tmk0=ToFloat(tmps);

}

void TuneDialogKDV::SetDefCoefs()
{
    Bac_block1.Art = 44.65;
    Bac_block1.Brt = 3345.75;
    //Bac_block1.Tmk0 = 24;
    Bac_block1.K_freq = 1;

    for (int i=0; i<6; i++)
    {
       Bac_block2.DPsi[i] = 0;
    }

    for (int i=0; i<3; i++)
    {
        Bac_block2.KmU1[i] = 1;
        Bac_block2.KmU2[i] = 1;
        Bac_block2.KmU4[i] = 1;
        Bac_block2.KmI1[i] = 1;
        Bac_block2.KmI16[i] = 1;
        Bac_block2.KmI2[i] = 1;
        Bac_block2.KmI32[i] = 1;
        Bac_block2.KmI8[i] = 1;
    }
    Bac_block2.KmI4[0] =  0.997060814;
    Bac_block2.KmI4[1] =  0.993458061;
    Bac_block2.KmI4[2] =  0.992731500;

    FillBac(BoardTypes::BT_BASE);
}

int TuneDialogKDV::SetNewTuneCoefs()
{
    Bac_newblock1.Art = Bac_block1.Art;
    Bac_newblock1.Brt = Bac_block1.Brt;
    Bac_newblock1.K_freq = Bac_block1.K_freq;

    for (int i=0; i<6; i++)
    {
        Bac_newblock2.DPsi[i] = Bac_block2.DPsi[i];
    }

    for (int i=0; i<3; i++)
    {
        Bac_newblock2.KmU1[i] = Bac_block2.KmU1[i];
        Bac_newblock2.KmU2[i] = Bac_block2.KmU2[i];
        Bac_newblock2.KmU4[i] = Bac_block2.KmU4[i];
        Bac_newblock2.KmI1[i] = Bac_block2.KmI1[i];
        Bac_newblock2.KmI16[i] = Bac_block2.KmI16[i];
        Bac_newblock2.KmI2[i] = Bac_block2.KmI2[i];
        Bac_newblock2.KmI32[i] = Bac_block2.KmI32[i];
        Bac_newblock2.KmI4[i] = Bac_block2.KmI4[i];
        Bac_newblock2.KmI8[i] = Bac_block2.KmI8[i];
    }
    return NOERROR;
}

float TuneDialogKDV::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        ERMSG("Значение "+text+" не может быть переведено во float");
        return 0;
    }
    return tmpf;
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialogKDV::StartMip()
{
    mipcanal = new IEC104;
    connect(mipcanal,SIGNAL(signalsready(Parse104::Signals104&)),this,SLOT(ParseMipData(Parse104::Signals104&)));
    connect(this,SIGNAL(stopall()),mipcanal,SLOT(Stop()));
}

void TuneDialogKDV::ParseMipData(Parse104::FlSignals104 &Signal)
{
    // precision
    static int Precisions[46] = {0,4,4,4,3,3,3,4,4,4,4,3,3,3,3,3,3,1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.fl.SigAdr;
    if (index != quint32(-1))
    {
        MipDat[index] = Signal.fl.SigVal;
        /*if ((index >= 11) && (index <= 13))
            MipDat[index] = -MipDat[index]; // у МИП-а знак угла отрицательный*/
        WDFunc::SetLBLText(this, "mip"+QString::number(index), QString::number(Signal.fl.SigVal, 'f', Precisions[index]));

    }
}

int TuneDialogKDV::CheckMip()
{
    double ValuesToCheck[10] = {S0,HZ50,HZ50,HZ50,MVTC.u,MVTC.u,MVTC.u,MVTC.i[0],MVTC.i[1],MVTC.i[2]};
    double ThresholdsToCheck[10] = {S0,TH005,TH005,TH005,MTTC.u,MTTC.u,MTTC.u,MTTC.i,MTTC.i,MTTC.i};
    double *VTC, *TTC;
    VTC = ValuesToCheck;
    TTC = ThresholdsToCheck;
    for (int i = 0; i < 10; i++)
    {
        QString tmps;
        WDFunc::LBLText(this, "mip"+QString::number(i), tmps);
        bool ok;
        double tmpd = tmps.toDouble(&ok);
        if (!ok)
            return GENERALERROR;
        if (!IsWithinLimits(tmpd, *VTC, *TTC))
        {
            EMessageBox::information(this, "Внимание", "Несовпадение МИП по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+\
                      " +/- "+QString::number(*TTC,'f',4));
            return GENERALERROR;
        }
        ++VTC;
        ++TTC;
    }
    return NOERROR;
}

bool TuneDialogKDV::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

void TuneDialogKDV::SetTuneMode()
{
    TuneControlType = sender()->objectName().toInt();
}

void TuneDialogKDV::StopMip()
{
    emit stopall();
}



int TuneDialogKDV::ShowControlChooseDialog()
{
    TuneControlType = TUNERET; // по-умолчанию тип контроля - по РЕТОМу
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Метод подтверждения измеряемых данных:");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Вручную по прибору Энергомонитор");
    pb->setObjectName(QString::number(TUNERET));
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneMode()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    if (Cancelled)
        return GENERALERROR;
    else
        return NOERROR;
}

int TuneDialogKDV::Show3PhaseScheme()
{
    //QDialog *dlg = new QDialog;
    //QVBoxLayout *lyout = new QVBoxLayout;
    //QPixmap pmp;

    /*pmp.load("../tune84.png");

    QLabel *lblpmp = new QLabel;
    lblpmp->setPixmap(pmp);
    lyout->addWidget(lblpmp);
    QLabel *lbl = new QLabel("1. Отключите выходы РЕТОМ;");
    lyout->addWidget(lbl);
    lbl = new QLabel("2. Соберите схему подключения по вышеприведённой картинке;");
    lyout->addWidget(lbl);
    lbl=new QLabel("3. Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "\
                   "сдвига по всем фазам 0 град.;");
    lyout->addWidget(lbl);
    lbl=new QLabel("4. Задайте на РЕТОМ значения напряжений по фазам 60 В;");
    lyout->addWidget(lbl);
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != Config::MTM_83)
    {
        lbl=new QLabel("   Задайте на РЕТОМ значения токов по фазам 1 А;");
        lyout->addWidget(lbl);
    }
    lbl = new QLabel("5. Включите выходы РЕТОМ");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();*/
    return NOERROR;
}

int TuneDialogKDV::Start7_2_3()
{
    if (TuneControlType == TUNEMIP)
    {
        GED_Type = TD_GED_U; // любой параметр для проверки связи сгодится
        return GetExternalData();
    }
    return NOERROR;
}

int TuneDialogKDV::Start7_3_1()
{

    /*QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl=new QLabel("Задайте входные напряжения равными 70,0 В (21,7В в РЕТОМ,\n" \
                           "входные токи равными 7,5А (0,75 А в РЕТОМ), частоту 51 Гц,\n" \
                           "угол между токами и напряжениями 0 градусов");"
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();*/

    return NOERROR;
}

int TuneDialogKDV::Start7_3_2()
{
   //int i;
   if (Commands::GetBac(1, &Bac_block1, sizeof(Bac1)) != NOERROR)
   {
       WARNMSG("Ошибка при приёме данных");
       return GENERALERROR;
   }
   Bac_newblock1 = Bac_block1;

   TimeFunc::Wait(100);

   if (Commands::GetBac(2, &Bac_block2, sizeof(Bac2)) != NOERROR)
   {
       WARNMSG("Ошибка при приёме данных");
       return GENERALERROR;
   }
   Bac_newblock2 = Bac_block2;
   // обновление коэффициентов в соответствующих полях на экране
   FillBac(0);

    //for(i = 0; i<3; i++)
    //CKDV->Bci_block.C_pasp[i] = 15036;
    CKDV->Bci_block.Imax = 7.8;
    CKDV->Bci_block.Umax = 74;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
    return GENERALERROR;

    return NOERROR;

}

int TuneDialogKDV::Start7_3_4()
{

    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl=new QLabel("Значение тока и напряжения при этом контролируются по показаниям прибора Энергомонитор.\n" \
                           "При использовании в качестве источника сигналов РЕТОМ-51 задается угол между током и\n"\
                           "напряжением в фазе А, при использовании имитатора АВМ-КИВ задается значение tg δ.");
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

    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_2()
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Количество усреднений");
    ledit = new QLineEdit;
    ledit->setObjectName("N");
    QPushButton* pb = new QPushButton;
    ask = new QDialog();
    ask->setAttribute(Qt::WA_DeleteOnClose);

    glyout->addWidget(lbl,0,1,1,1);
    glyout->addWidget(ledit,1,1,1,1);
    pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadN()));
    glyout->addWidget(pb,2,1,1,1);
    vlyout->addLayout(glyout);
    ask->setLayout(vlyout);
    ask->exec();

    ShowRetomDialog(20.7, 0.75, 0);
    Kadc = 4;
    EnterDataTune();
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_6()
{
    //int i;
    //for(i = 0; i<3; i++)
    //CKDV->Bci_block.C_pasp[i] = 10024;
    CKDV->Bci_block.Imax = 31;
    CKDV->Bci_block.Umax = 290;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
    return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_7()
{
    ShowRetomDialog(82.8, 3.0, 0);
    Kadc = 1;
    EnterDataTune();
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_11()
{
    //int i;
    //for(i = 0; i<3; i++)
    //CKDV->Bci_block.C_pasp[i] = 5012;
    CKDV->Bci_block.Imax = 15.5;
    CKDV->Bci_block.Umax = 145;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR) //&CKDV->Bci_block,
    return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_12()
{
    ShowRetomDialog(41.4, 1.5, 0);
    Kadc = 2;
    EnterDataTune();
    return NOERROR;;
}

int TuneDialogKDV::Start7_3_4_13()
{
    //int i;
    //for(i = 0; i<3; i++)
    //CKDV->Bci_block.C_pasp[i] = 2506;
    CKDV->Bci_block.Imax = 3.9;
    CKDV->Bci_block.Umax = 300;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR) //&CKDV->Bci_block,
    return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_14()
{
    ShowRetomDialog(41.4, 0.375, 0);
    Kadc = 8;
    EnterDataTune();
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_15()
{
    //int i;
    //for(i = 0; i<3; i++)
    CKDV->Bci_block.Imax = 1.9;
    CKDV->Bci_block.Umax = 300;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
    return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_16()
{
    ShowRetomDialog(41.4, 0.18, 0);
    Kadc = 16;
    EnterDataTune();
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_17()
{
    //int i;
    //for(i = 0; i<3; i++)
    //CKDV->Bci_block.C_pasp[i] = 626;
    CKDV->Bci_block.Imax = 0.9;
    CKDV->Bci_block.Umax = 300;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
    return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneDialogKDV::Start7_3_4_18()
{
    ShowRetomDialog(41.4, 0.09, 0);
    Kadc = 32;
    EnterDataTune();
    FillNewBac();
    return NOERROR;
}


int TuneDialogKDV::Start7_3_5()
{
    if (LoadWorkConfig())
       return GENERALERROR;
    WaitNSeconds(5);
    if (EMessageBox::question(this,"Закончить?","Закончить настройку и записать коэффициенты в модуль?"))
    {

        // Пишем в модуль посчитанные регулировочные коэффициенты

        if (Commands::WriteBac(1, &Bac_newblock1, sizeof(Bac_newblock1)) != NOERROR)  // Григорий Матвеевич попросил писать коэффициенты сразу в модуль
        return GENERALERROR;

        TimeFunc::Wait(100);

        if (Commands::WriteBac(2, &Bac_newblock2, sizeof(Bac_newblock2)) != NOERROR)  // Григорий Матвеевич попросил писать коэффициенты сразу в модуль
        return GENERALERROR;

        if (EMessageBox::question(this,"Протокол поверки","Начать поверку?"))
        {
           GenerateReport();
        }

        return NOERROR;
    }
    else
        return false;
}

void TuneDialogKDV::ReadN()
{
    QString tmps;
    WDFunc::LE_read_data(ask, "N", tmps);
    N=tmps.toInt();
    ask->close();
}

int TuneDialogKDV::GetExternalData()
{
#if PROGSIZE != PROGSIZE_EMUL
    switch (TuneControlType)
    {
    case TUNEMIP:
    {
        StartMip();
        WaitNSeconds(5);
        StopMip();
        if (CheckMip())
        {
            for (int i=1; i<4; ++i)
            {
                RealData.f[i-1] = MipDat[i];
                RealData.u[i-1] = MipDat[i+3];
                RealData.i[i-1] = MipDat[i+6];
                RealData.d[i-1] = MipDat[i+10];
            }
            RealData.dpsiU[0] = MipDat[43];
            RealData.dpsiU[1] = MipDat[44];
            return NOERROR;
        }
        return GENERALERROR;
    }
    case TUNEMAN:
    {
        QDialog *dlg = new QDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setObjectName("dlg7371");
        QGridLayout *glyout = new QGridLayout;
        QLabel *lbl = new QLabel("Введите значения сигналов по приборам");
        glyout->addWidget(lbl,0,0,1,6);
        int row = 1;
        int column = 0;
        if (GED_Type & TD_GED_U)
        {
            for (int i=0; i<3; i++) // for A to C
            {
                lbl = new QLabel("Uф" + QString::number(i+10,36).toUpper()+", В");
                glyout->addWidget(lbl,row,column++,1,1);
                QDoubleSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i), 0, 200, 2);
                spb->setValue(60.0);
                glyout->addWidget(spb,row,column++,1,1);
            }
            ++row;
            column = 0;
        }
        if (GED_Type & TD_GED_I)
        {
            for (int i=0; i<3; i++) // for A to C
            {
                lbl = new QLabel("Iф" + QString::number(i+10,36).toUpper()+", A");
                glyout->addWidget(lbl,row, column++,1,1);
                QDoubleSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i+3), 0, 6, 3);
                spb->setValue(CKDV->Bci_block.Imax);//(1.5*CKDV->Bci_block.C_pasp[i]*CKDV->Bci_block.Unom*qPow(10,-6)*CONST2PIF);
                glyout->addWidget(spb,row, column++,1,1);
            }
            ++row;
            column = 0;
        }
        if (GED_Type & TD_GED_D)
        {
            for (int i=0; i<3; i++) // for A to C
            {
                lbl = new QLabel("Уг.нагр. ф" + QString::number(i+10,36).toUpper()+", град");
                glyout->addWidget(lbl,3,i*2,1,1);
                QDoubleSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i+6), -360, 360, 2);
                spb->setValue(0.0);
                glyout->addWidget(spb,row, column++,1,1);
            }
            ++row;
            column = 0;
        }
        if (GED_Type & TD_GED_F)
        {
            lbl = new QLabel("Частота ");
            glyout->addWidget(lbl,row,column++,1,1);
            QDoubleSpinBox *spb = WDFunc::NewSPB(this, "spb73719", 40.0, 60.0, 3);
            spb->setValue(50.0);
            glyout->addWidget(spb,row, column++,1,1);
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
            return GENERALERROR;
        return NOERROR;
    }
    case TUNERET:
    {
        for (int i=0; i<3; i++)
        {
            RealData.u[i] = 60.0;
            RealData.i[i] =(CKDV->Bci_block.Imax);//(1.5*CKDV->Bci_block.C_pasp[i]*CKDV->Bci_block.Unom*qPow(10,-6)*CONST2PIF);
            RealData.d[i] = 0.0;
            RealData.f[i] = 50.0;
        }
        return NOERROR;
    }
    }
    return GENERALERROR;
#else
    return NOERROR;
#endif
}

void TuneDialogKDV::EnterDataTune()
{
    int i;
    ask = new QDialog(this);
    //QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl,0,0,1,6);

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Uэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,1,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU"+QString::number(i));
        glyout->addWidget(ledit,2,i,1,1);
    }

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Iэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,3,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI"+QString::number(i));
        glyout->addWidget(ledit,4,i,1,1);
    }

    for(i=0; i<6; i++)
    {
        lbl = new QLabel("φэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,5,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi"+QString::number(i));
        glyout->addWidget(ledit,6,i,1,1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl,7,0,1,1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit,8,0,1,1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb,SIGNAL(clicked()),this,SLOT(CalcTuneCoefs()));
    glyout->addWidget(pb,9,2,1,2);
   /* pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),this,SLOT(close()));
    connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
    glyout->addWidget(pb,9,3,1,3);*/

    ask->setLayout(glyout);
    ask->exec();

    /*lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();*/

}

void TuneDialogKDV::Enter20Data()
{
    int i;
    ask = new QDialog(this);
    //QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl,0,0,1,6);

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Uэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,1,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU"+QString::number(i));
        glyout->addWidget(ledit,2,i,1,1);
    }

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Iэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,3,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI"+QString::number(i));
        glyout->addWidget(ledit,4,i,1,1);
    }

    for(i=0; i<6; i++)
    {
        lbl = new QLabel("φэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,5,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi"+QString::number(i));
        glyout->addWidget(ledit,6,i,1,1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl,7,0,1,1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit,8,0,1,1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveValuesTemp20()));
    glyout->addWidget(pb,9,2,1,2);
   /* pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),this,SLOT(close()));
    connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
    glyout->addWidget(pb,9,3,1,3);*/

    ask->setLayout(glyout);
    ask->exec();

}

void TuneDialogKDV::Enterminus20Data()
{
    int i;
    ask = new QDialog(this);
    //QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl,0,0,1,6);

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Uэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,1,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU"+QString::number(i));
        glyout->addWidget(ledit,2,i,1,1);
    }

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Iэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,3,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI"+QString::number(i));
        glyout->addWidget(ledit,4,i,1,1);
    }

    for(i=0; i<6; i++)
    {
        lbl = new QLabel("φэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,5,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi"+QString::number(i));
        glyout->addWidget(ledit,6,i,1,1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl,7,0,1,1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit,8,0,1,1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveValuesTempMinus20()));
    glyout->addWidget(pb,9,2,1,2);
   /* pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),this,SLOT(close()));
    connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
    glyout->addWidget(pb,9,3,1,3);*/

    ask->setLayout(glyout);
    ask->exec();

}

void TuneDialogKDV::Enter60Data()
{
    int i;
    ask = new QDialog(this);
    //QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl,0,0,1,6);

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Uэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,1,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU"+QString::number(i));
        glyout->addWidget(ledit,2,i,1,1);
    }

    for(i=0; i<3; i++)
    {
        lbl = new QLabel("Iэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,3,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI"+QString::number(i));
        glyout->addWidget(ledit,4,i,1,1);
    }

    for(i=0; i<6; i++)
    {
        lbl = new QLabel("φэт["+QString::number(i)+"]:");
        glyout->addWidget(lbl,5,i,1,1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi"+QString::number(i));
        glyout->addWidget(ledit,6,i,1,1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl,7,0,1,1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit,8,0,1,1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveValuesTemp60()));
    glyout->addWidget(pb,9,2,1,2);
   /* pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),this,SLOT(close()));
    connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
    glyout->addWidget(pb,9,3,1,3);*/

    ask->setLayout(glyout);
    ask->exec();

}

void TuneDialogKDV::CloseAsk()
{
    ask->close();
}

void TuneDialogKDV::SaveValuesTemp20()
{
    int i;
    QString tmps;
    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneU"+QString::number(i), tmps);
      Uet[i]=tmps.toFloat();
    }

    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneI"+QString::number(i), tmps);
      Iet[i]=tmps.toFloat();
    }

    for (i=0; i<6; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetunePhi"+QString::number(i), tmps);
      PHIet[i]=tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    FREQet=tmps.toFloat();

    ask->close();
}

void TuneDialogKDV::SaveValuesTempMinus20()
{
    int i;
    QString tmps;
    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneU"+QString::number(i), tmps);
      UetMinus20[i]=tmps.toFloat();
    }

    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneI"+QString::number(i), tmps);
      IetMinus20[i]=tmps.toFloat();
    }

    for (i=0; i<6; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetunePhi"+QString::number(i), tmps);
      PHIetMinus20[i]=tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    FREQetMinus20=tmps.toFloat();

    ask->close();
}

void TuneDialogKDV::SaveValuesTemp60()
{
    int i;
    QString tmps;
    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneU"+QString::number(i), tmps);
      Uet60[i]=tmps.toFloat();
    }

    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneI"+QString::number(i), tmps);
      Iet60[i]=tmps.toFloat();
    }

    for (i=0; i<6; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetunePhi"+QString::number(i), tmps);
      PHIet60[i]=tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    FREQet60=tmps.toFloat();

    ask->close();
}

int TuneDialogKDV::CalcTuneCoefs()
{
    int i;
    QString tmps;
    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneU"+QString::number(i), tmps);
      Uet[i]=tmps.toFloat();
    }

    for (i=0; i<3; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetuneI"+QString::number(i), tmps);
      Iet[i]=tmps.toFloat();
    }

    for (i=0; i<6; i++)
    {
      WDFunc::LE_read_data(ask, "ValuetunePhi"+QString::number(i), tmps);
      PHIet[i]=tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    FREQet=tmps.toFloat();

    ask->close();

    if(ReadAnalogMeasurements() != NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
        return GENERALERROR;
    }

        if(Kadc == 1)
        {
            for (i=0; i<3; i++)
            {
                Bac_newblock2.KmU1[i] = Bac_block2.KmU1[i] * Uet[i]/ Bda_in.IUefNat_filt[i];
                Bac_newblock2.KmI1[i] = Bac_block2.KmI1[i] * Iet[i] / Bda_in.IUefNat_filt[i+3] ;
            }

            Bac_newblock1.K_freq = Bac_block1.K_freq * FREQet / Bda_in.Frequency ;

            for (i=1; i < 3; i++) Bac_newblock2.DPsi[i] =  Bac_block2.DPsi[i] - Bda_in.phi_next_f[i];
            for (i=3; i < 6; i++) Bac_newblock2.DPsi[i] =  Bac_block2.DPsi[i] + PHIet[i] - Bda_in.phi_next_f[i];
        }
        else if(Kadc == 2)
        {
            for (i=0; i<3; i++)
            {
                Bac_newblock2.KmU2[i] = Bac_block2.KmU2[i] * Uet[i]/ Bda_in.IUefNat_filt[i];
                Bac_newblock2.KmI2[i] = Bac_block2.KmI2[i] * Iet[i] / Bda_in.IUefNat_filt[i+3];
            }
        }
        else if(Kadc == 4)
        {
            for (i=0; i<3; i++)
            {
                Bac_newblock2.KmU4[i] = Bac_block2.KmU4[i] * Uet[i]/ Bda_in.IUefNat_filt[i];
                Bac_newblock2.KmI4[i] = Bac_block2.KmI4[i] * Iet[i] / Bda_in.IUefNat_filt[i+3];
            }
        }
        else if(Kadc == 8)
        {
            for (i=0; i<3; i++)
            {
                Bac_newblock2.KmI8[i] = Bac_block2.KmI8[i] * Iet[i] / Bda_in.IUefNat_filt[i+3];
            }
        }
        else if(Kadc == 16)
        {
            for (i=0; i<3; i++)
            {
                Bac_newblock2.KmI16[i] = Bac_block2.KmI16[i] * Iet[i] / Bda_in.IUefNat_filt[i+3];
            }
        }
        else if(Kadc == 32)
        {
            for (i=0; i<3; i++)
            {
                Bac_newblock2.KmI32[i] = Bac_block2.KmI32[i] * Iet[i] / Bda_in.IUefNat_filt[i+3];
            }
        }


    return NOERROR;

}



/*int TuneDialogKDV::CalcTuneCoefsKadc4()
{

    return NOERROR;
}

int TuneDialogKDV::CalcTuneCoefsKadc8()
{

    return NOERROR;
}

int TuneDialogKDV::CalcTuneCoefsKadc16()
{

    return NOERROR;
}

int TuneDialogKDV::CalcTuneCoefsKadc32()
{

    return NOERROR;
}*/

void TuneDialogKDV::GetBdAndFillMTT()
{

}

int TuneDialogKDV::SaveWorkConfig()
{
    if (Commands::GetFileWithRestore(CM_CONFIGFILE,S2ConfigForTune) == NOERROR)
        memcpy(&Bci_block_work,&CKDV->Bci_block,sizeof(ConfigKDV::Bci));
    else
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogKDV::ReadAnalogMeasurements()
{
    int i,j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;


    for(i = 0; i<3; i++)
    {
        sumU[i] = 0;
        sumI[i] = 0;
        sumPHI[i] = 0;
        sumPHI[i+3] = 0;
    }

    for(i = 0; i<N; i++)
    {
        // получение текущих аналоговых сигналов от модуля
         if (Commands::GetBd(BT_BASE, &Bda_in, sizeof(Bda_in)) != NOERROR)
         {
             EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
             return GENERALERROR;
         }
         else
         {
             for(j = 0; j<3; j++)
             {
               sumU[j] += Bda_in.IUefNat_filt[j];
               sumI[j] += Bda_in.IUefNat_filt[j+3];
             }

             for(j = 0; j<6; j++)
             {
               sumPHI[j] += Bda_in.phi_next_f[j];
               /*if(j==4)
               {
                   samples[inc] = Bda_in.phi_next_f[j];
                   inc++;
               }*/
             }

             sumFreq += Bda_in.Frequency;

             QThread::msleep(500);
         }
    }

    for(i = 0; i<3; i++)
    {
      Bda_in.IUefNat_filt[i] = sumU[i]/N;
      Bda_in.IUefNat_filt[i+3] = sumI[i]/N;
    }

    for(i = 0; i<6; i++)
    {
      Bda_in.phi_next_f[i] = sumPHI[i]/N;
    }

    Bda_in.Frequency = sumFreq/N;

    return NOERROR;
}

int TuneDialogKDV::ReadAnalogTemp20()
{
    int i,j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;
    for(i = 0; i<N; i++)
    {
        // получение текущих аналоговых сигналов от модуля
         if (Commands::GetBd(BT_BASE, &Bda_block20, sizeof(Bda_block20)) != NOERROR)
         {
             EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
             return GENERALERROR;
         }
         else
         {
             for(j = 0; j<3; j++)
             {
               sumU[j] += Bda_block20.IUefNat_filt[j];
               sumI[j] += Bda_block20.IUefNat_filt[j+3];
             }

             for(j = 0; j<6; j++)
             {
               sumPHI[j] += Bda_block20.phi_next_f[j];
             }

             sumFreq += Bda_block20.Frequency;

             QThread::msleep(500);
         }
    }

    for(i = 0; i<3; i++)
    {
      Bda_block20.IUefNat_filt[i] = sumU[i]/N;
      Bda_block20.IUefNat_filt[i+3] = sumI[i]/N;
    }

    for(i = 0; i<6; i++)
    {
      Bda_block20.phi_next_f[i] = sumPHI[i]/N;
    }

    Bda_block20.Frequency = sumFreq/N;

    return NOERROR;
}

int TuneDialogKDV::ReadAnalogTemp60()
{
    int i,j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;
    for(i = 0; i<N; i++)
    {
        // получение текущих аналоговых сигналов от модуля
         if (Commands::GetBd(BT_BASE, &Bda_block60, sizeof(Bda_block60)) != NOERROR)
         {
             EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
             return GENERALERROR;
         }
         else
         {
             for(j = 0; j<3; j++)
             {
               sumU[j] += Bda_block60.IUefNat_filt[j];
               sumI[j] += Bda_block60.IUefNat_filt[j+3];
             }

             for(j = 0; j<6; j++)
             {
               sumPHI[j] += Bda_block60.phi_next_f[j];
             }

             sumFreq += Bda_block60.Frequency;

             QThread::msleep(500);
         }
    }

    for(i = 0; i<3; i++)
    {
      Bda_block60.IUefNat_filt[i] = sumU[i]/N;
      Bda_block60.IUefNat_filt[i+3] = sumI[i]/N;
    }

    for(i = 0; i<6; i++)
    {
      Bda_block60.phi_next_f[i] = sumPHI[i]/N;
    }

    Bda_block60.Frequency = sumFreq/N;

    return NOERROR;
}

int TuneDialogKDV::ReadAnalogTempMinus20()
{
    int i,j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;
    for(i = 0; i<N; i++)
    {
        // получение текущих аналоговых сигналов от модуля
         if (Commands::GetBd(BT_BASE, &Bda_blockMinus20, sizeof(Bda_blockMinus20)) != NOERROR)
         {
             EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
             return GENERALERROR;
         }
         else
         {
             for(j = 0; j<3; j++)
             {
               sumU[j] += Bda_blockMinus20.IUefNat_filt[j];
               sumI[j] += Bda_blockMinus20.IUefNat_filt[j+3];
             }

             for(j = 0; j<6; j++)
             {
               sumPHI[j] += Bda_blockMinus20.phi_next_f[j];
             }

             sumFreq += Bda_blockMinus20.Frequency;

             QThread::msleep(500);
         }
    }

    for(i = 0; i<3; i++)
    {
      Bda_blockMinus20.IUefNat_filt[i] = sumU[i]/N;
      Bda_blockMinus20.IUefNat_filt[i+3] = sumI[i]/N;
    }

    for(i = 0; i<6; i++)
    {
      Bda_blockMinus20.phi_next_f[i] = sumPHI[i]/N;
    }

    Bda_blockMinus20.Frequency = sumFreq/N;

    return NOERROR;
}

int TuneDialogKDV::SaveUeff()
{
/*    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i=0; i<6; i++)
        IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i]; */
    return NOERROR;
}

int TuneDialogKDV::ShowRetomDialog(double U, double I, double Y)
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc)"\
                   "Угол между токами и напряжениями: "+QString::number(Y, 'f', 2)+" град.,\n"\
                   "Значения напряжений(для РЕТОМ): "+QString::number(U, 'f', 2)+" В, токов(для РЕТОМ): "+QString::number(I, 'f', 2)+" А\n"\
                   "Значение частоты: 51Гц");
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
    return NOERROR;
}

/*int TuneDialogKDV::ShowRetomDialog2(double U)
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим напряжений 2-ой тройки (Uabc) с углами "\
                   "сдвига по фазам: А - 0 град., В - 240 град., С - 120 град.,\n"\
                   "Значения напряжений: "+QString::number(U, 'g', 2)+" В");
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
    return NOERROR;
}*/

int TuneDialogKDV::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&CKDV->Bci_block,&Bci_block_work,sizeof(ConfigKDV::Bci));
    if (Commands::WriteFile(CM_CONFIGFILE, S2ConfigForTune) != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

void TuneDialogKDV::SetExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == nullptr)
        return;
    for (int i=0; i<3; ++i)
    {
        QDoubleSpinBox *spb = this->findChild<QDoubleSpinBox *>("spb7371"+QString::number(i));
        if (spb != nullptr)
            RealData.u[i] = spb->value();
        spb = this->findChild<QDoubleSpinBox *>("spb7371"+QString::number(i+3));
        if (spb != nullptr)
            RealData.i[i] = spb->value();
        spb = this->findChild<QDoubleSpinBox *>("spb7371"+QString::number(i+6));
        if (spb != nullptr)
            RealData.d[i] = spb->value();
    }
    QDoubleSpinBox *spb = this->findChild<QDoubleSpinBox *>("spb73719");
    if (spb != nullptr)
        RealData.f[0] = spb->value();
    StdFunc::ClearCancel();
    dlg->close();
}

void TuneDialogKDV::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == nullptr)
        return;
    StdFunc::Cancel();
    dlg->close();
}

QWidget *TuneDialogKDV::Bd1W(QWidget *parent)
{
    int i;
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";

    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;

    lyout->addLayout(hlyout);

    for (i = 0; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff_ADC"+IndexStr),0,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i), ValuesFormat, \
                                          QString::number(i)+"Ueff_ADC"+IndexStr+".Измеренные сигналы в кодах АЦП"),1,i,1,1);
    }

        glyout->addWidget(WDFunc::NewLBL(parent, "Frequency"),2,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value6", ValuesFormat, \
                                          "Частота"),3,0,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Pt100"),4,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value7", ValuesFormat, \
                                          "Температура"),5,0,1,1);

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void TuneDialogKDV::FillBd1(QWidget *parent)
{

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i), WDFunc::StringValueWithCheck(Bda_block.Ueff_ADC[i], 4));
    }

     WDFunc::SetLBLText(parent, "value6", WDFunc::StringValueWithCheck(Bda_block.Frequency, 4));
     WDFunc::SetLBLText(parent, "value7", WDFunc::StringValueWithCheck(Bda_block.Pt100, 4));

}

void TuneDialogKDV::CancelTune()
{
    StdFunc::Cancel();
}

void TuneDialogKDV::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {

    case CKDV_BDA_IN: // Блок #1
        FillBd1(this);
        break;

    default:
        return;
    }
}


int TuneDialogKDV::TunePt100Channel()
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Количество усреднений");
    ledit = new QLineEdit;
    ledit->setObjectName("N");
    QPushButton* pb = new QPushButton;
    ask = new QDialog();
    ask->setAttribute(Qt::WA_DeleteOnClose);

    if (EAbstractTuneDialog::CheckPassword() == NOERROR)
    {
        //StdFunc::ClearCancel();
        //QEventLoop EnterLoop;
        glyout->addWidget(lbl,0,1,1,1);
        glyout->addWidget(ledit,1,1,1,1);
        pb = new QPushButton("Ok");
        connect(pb,SIGNAL(clicked()),this,SLOT(TuneChannel()));
        //connect(pb,SIGNAL(clicked()),&EnterLoop,SLOT(quit()));
        glyout->addWidget(pb,2,1,1,1);
        vlyout->addLayout(glyout);
        ask->setLayout(vlyout);
        ask->show();
        //ask->setModal(false);


       return NOERROR;

    }
    return GENERALERROR;
}


int TuneDialogKDV::TuneChannel()
{

    int i;
    QString tmps;
    WDFunc::LE_read_data(ask, "N", tmps);
    N=tmps.toInt();
    ask->close();
    float sum = 0.0;
    SaveWorkConfig();

    if (Commands::GetBac(1, &Bac_block1, sizeof(Bac1)) != NOERROR)
    {
        WARNMSG("Ошибка при приёме данных");
        return GENERALERROR;
    }
    Bac_newblock1 = Bac_block1;

    TimeFunc::Wait(100);

    if (Commands::GetBac(2, &Bac_block2, sizeof(Bac2)) != NOERROR)
    {
        WARNMSG("Ошибка при приёме данных");
        return GENERALERROR;
    }

    Bac_newblock2 = Bac_block2;
    // обновление коэффициентов в соответствующих полях на экране
    FillBac(0);


       if(Show80() == GENERALERROR)
       return GENERALERROR;
       else
       {
           sum = 0;
           for(i = 0; i<N; i++)
           {
             if(Commands::GetBda(BT_NONE, &BdaPt100_80Om, sizeof(BdaPt100_80Om)) == NOERROR)
             {
               sum += BdaPt100_80Om.Pt100;
               QThread::msleep(500);
             }
             else
             return GENERALERROR;
           }
           BdaPt100_80Om.Pt100 = sum/N; // усредняем
       }

       if(Show120() == GENERALERROR)
       return GENERALERROR;
       else
       {
           sum = 0;
           for(i = 0; i<N; i++)
           {
              if(Commands::GetBda(BT_NONE, &BdaPt100_120Om, sizeof(BdaPt100_120Om)) == NOERROR)
              {
                sum += BdaPt100_120Om.Pt100;
                QThread::msleep(500);
              }
              else
              return GENERALERROR;
           }
           BdaPt100_120Om.Pt100 = sum/N; // усредняем
       }

       Bda_block = BdaPt100_120Om;
       FillBd1(this);
       CalcNewPt100Coefs();
       FillNewBac();
       WaitNSeconds(5);

       if(Commands::WriteBac(1, &Bac_newblock1, sizeof(Bac_newblock1)) != NOERROR)
       return GENERALERROR;

       TimeFunc::Wait(100);

        if(Commands::WriteBac(2, &Bac_newblock2, sizeof(Bac_newblock2)) == NOERROR)
       {
          EMessageBox::information(this, "Настройка", "Настройка завершена");
          return NOERROR;
       }
       else
       return GENERALERROR;


}

int TuneDialogKDV::Show80()
{
    if (EMessageBox::question(this,"Настройка",\
                                 "Подключите банк сопротивлений к модулю\n"
                                 "На банке сопротивлений задайте напряжение 80 Ом"\
                                 " и нажмите OK", nullptr, "Ok" , "Close"))
        return NOERROR;
    else
        return GENERALERROR;
}

int TuneDialogKDV::Show120()
{

    if (EMessageBox::question(this,"Настройка",\
                                 "Подключите банк сопротивлений к модулю\n"
                                 "На банке сопротивлений задайте напряжение 120 Ом"\
                                 " и нажмите OK", nullptr, "Ok" , "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

void TuneDialogKDV::CalcNewPt100Coefs()
{
    if (StdFunc::FloatInRange(BdaPt100_120Om.Pt100, BdaPt100_80Om.Pt100))
    {
        WARNMSG("Ошибка в настроечных коэффициентах, деление на ноль");
    }
    else
    {
        Bac_newblock1.Art = ((BdaPt100_120Om.Pt100 - BdaPt100_80Om.Pt100)/40); //[ед.АЦП/Ом],
        Bac_newblock1.Brt = (2*BdaPt100_120Om.Pt100 - 3*BdaPt100_80Om.Pt100);  //[ед.АЦП]
    }
}

int TuneDialogKDV::TuneTemp()
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Количество усреднений");
    ledit = new QLineEdit;
    ledit->setObjectName("N");
    QPushButton* pb = new QPushButton;
    ask = new QDialog();
    ask->setAttribute(Qt::WA_DeleteOnClose);

    if (EAbstractTuneDialog::CheckPassword() == NOERROR)
    {
        //StdFunc::ClearCancel();
        //QEventLoop EnterLoop;
        glyout->addWidget(lbl,0,1,1,1);
        glyout->addWidget(ledit,1,1,1,1);
        pb = new QPushButton("Ok");
        connect(pb,SIGNAL(clicked()),this,SLOT(TuneTempCor()));
        //connect(pb,SIGNAL(clicked()),&EnterLoop,SLOT(quit()));
        glyout->addWidget(pb,2,1,1,1);
        vlyout->addLayout(glyout);
        ask->setLayout(vlyout);
        ask->show();
        //ask->setModal(false);


       return NOERROR;

    }
    return GENERALERROR;
}

int TuneDialogKDV::TuneTempCor()
{

    //int i;
    QString tmps;
    WDFunc::LE_read_data(ask, "N", tmps);
    N=tmps.toInt();
    ask->close();
    //float sum = 0.0;
    SaveWorkConfig();

    if (Commands::GetBac(1, &Bac_block1, sizeof(Bac1)) != NOERROR)
    {
        WARNMSG("Ошибка при приёме данных");
        return GENERALERROR;
    }
    Bac_newblock1 = Bac_block1;

    TimeFunc::Wait(100);

    if (Commands::GetBac(2, &Bac_block2, sizeof(Bac2)) != NOERROR)
    {
        WARNMSG("Ошибка при приёме данных");
        return GENERALERROR;
    }
    Bac_newblock2 = Bac_block2;

    /*for(i=0; i<3; i++)
    {
      /Bac_block.TKPsi_a[i] = 0;
      Bac_block.TKPsi_b[i] = 0;
      Bac_block.TKUa[i] = 0;
      Bac_block.TKUb[i] = 0;
    }
    if (Commands::WriteBac(BT_MEZONIN, &Bac_block, sizeof(Bac)) != NOERROR)
    {
        WARNMSG("Ошибка при отправке данных");
        return GENERALERROR;
    }*/
    // обновление коэффициентов в соответствующих полях на экране
    FillBac(0);

    WaitNSeconds(5);
    Start7_3_4_11();
    //Average();

       if(Show125mA() == GENERALERROR)
       return GENERALERROR;
       else
       {
           if(AskTemp20() == GENERALERROR)
               return GENERALERROR;
           else
           {

               Enter20Data();

               if(ReadAnalogTemp20() != NOERROR)
               return GENERALERROR;

               if (Commands::GetBd(0, &Bd_block20, sizeof(Bd_block20)) != NOERROR)
               {
                   WARNMSG("Ошибка при приёме данных");
                   return GENERALERROR;
               }

           }

           if(AskTemp60() == GENERALERROR)
               return GENERALERROR;
           else
           {

               Enter60Data();

               if(ReadAnalogTemp60() != NOERROR)
               return GENERALERROR;

               if (Commands::GetBd(0, &Bd_block_plus60, sizeof(Bd_block_plus60)) != NOERROR)
               {
                   WARNMSG("Ошибка при приёме данных");
                   return GENERALERROR;
               }

           }

           if(AskTempMinus20() == GENERALERROR)
               return GENERALERROR;
           else
           {

               Enterminus20Data();

               if(ReadAnalogTempMinus20() != NOERROR)
               return GENERALERROR;

               if (Commands::GetBd(0, &Bd_block_minus20, sizeof(Bd_block_minus20)) != NOERROR)
               {
                   WARNMSG("Ошибка при приёме данных");
                   return GENERALERROR;
               }

           }


       }


       //FillBd1(this);
       CalcTempCorCoefs();
       FillNewBac();
       WaitNSeconds(5);

       /*if (Commands::WriteBac(BT_MEZONIN, &Bac_newblock, sizeof(Bac_newblock)) == NOERROR)
       {
          EMessageBox::information(this, "Настройка", "Настройка завершена");
          WaitNSeconds(5);
          LoadWorkConfig();
          return NOERROR;
       }
       else
       return GENERALERROR;*/
       return NOERROR;

}

int TuneDialogKDV::Show125mA()
{

    if (EMessageBox::question(this,"Настройка",\
                                 "Задайте ток источника равным 125 мА"\
                                 " и нажмите OK", nullptr, "Ok" , "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneDialogKDV::AskTemp20()
{

    if (EMessageBox::question(this,"Настройка",\
                                 "Установите в камере температуру 20±2°С"\
                                 " и через 30 минут нажмите OK", nullptr, "Ok" , "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneDialogKDV::AskTempMinus20()
{

    if (EMessageBox::question(this,"Настройка",\
                                 "Установите в камере температуру минус 20±2°С"\
                                 " и через 30 минут нажмите OK", nullptr, "Ok" , "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneDialogKDV::AskTemp60()
{

    if (EMessageBox::question(this,"Настройка",\
                                 "Установите в камере температуру 60±2°С"\
                                 " и через 30 минут нажмите OK", nullptr, "Ok" , "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneDialogKDV::Average()
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Количество усреднений");
    ledit = new QLineEdit;
    ledit->setObjectName("N");
    QPushButton* pb = new QPushButton;
    ask = new QDialog();
    ask->setAttribute(Qt::WA_DeleteOnClose);

    glyout->addWidget(lbl,0,1,1,1);
    glyout->addWidget(ledit,1,1,1,1);
    pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadN()));
    glyout->addWidget(pb,2,1,1,1);
    vlyout->addLayout(glyout);
    ask->setLayout(vlyout);
    ask->exec();

    return NOERROR;
}

void TuneDialogKDV::CalcTempCorCoefs()
{
    //int i;
    float deltaTplus, deltaTminus, dUplus[3], dUminus[3], deltaPhiplus[3], deltaPhiminus[3];

    deltaTplus = Bd_block_plus60.Tmk - Bd_block20.Tmk;
    deltaTminus = Bd_block_minus20.Tmk - Bd_block20.Tmk;

    /*for (i=0; i<3; i++)
    {
        dUplus[i] = ((Bda_block60.IUefNat_filt[i] * Uet[i])/(Bda_block20.IUefNat_filt[i] * Uet60[i])) - 1;
        dUminus[i] = ((Bda_blockMinus20.IUefNat_filt[i] * Uet[i])/(Bda_block20.IUefNat_filt[i] * UetMinus20[i])) - 1;
        Bac_newblock.TKUa[i] = ((dUminus[i]*deltaTplus*deltaTplus) - (dUplus[i]*deltaTminus*deltaTminus))/(deltaTplus*deltaTminus*(deltaTplus - deltaTminus));
        Bac_newblock.TKUb[i] = ((dUplus[i]*deltaTminus) - (dUminus[i]*deltaTplus))/(deltaTplus*deltaTminus*(deltaTplus - deltaTminus));
    }

    for (i=0; i<6; i++)
    {
      deltaPhiplus[i] = (Bda_block60.phi_next_f[i] - Bda_block20.phi_next_f[i]) - (PHIet60[i] - PHIet[i]);
      deltaPhiminus[i] = (Bda_blockMinus20.phi_next_f[i] - Bda_block20.phi_next_f[i]) - (PHIetMinus20[i] - PHIet[i]);
      Bac_newblock.TKPsi_a[i] = ((deltaPhiminus[i]*deltaTplus*deltaTplus) - (deltaPhiplus[i]*deltaTminus*deltaTminus))/(deltaTplus*deltaTminus*(deltaTplus - deltaTminus));
      Bac_newblock.TKPsi_b[i] = ((deltaPhiplus[i]*deltaTminus) - (deltaPhiminus[i]*deltaTplus))/(deltaTplus*deltaTminus*(deltaTplus - deltaTminus));
    }*/

}

void TuneDialogKDV::GenerateReport()
{
    /*// данные в таблицу уже получены или из файла, или в процессе работы
    // отобразим таблицу
   // ShowTable();
   // QString GOST = (PovType == GOST_1983) ? "1983" : "23625";
    report = new LimeReport::ReportEngine(this);
    QString path = StdFunc::GetSystemHomeDir()+"82report.lrxml";
    report->loadFromFile(path);
    report->dataManager()->addModel("maindata", ReportModel, false);

    ReportHeader.Organization = "ООО АСУ-ВЭИ";
    report->dataManager()->setReportVariable("Organization", ReportHeader.Organization);
    QString day = QDateTime::currentDateTime().toString("dd");
    QString month = QDateTime::currentDateTime().toString("MM");
    QString yr = QDateTime::currentDateTime().toString("yy");
    report->dataManager()->setReportVariable("Day", day);
    report->dataManager()->setReportVariable("Month", month);
    report->dataManager()->setReportVariable("Yr", yr);

    for(int i=0; i<21; i++) // 21 таблица!
    {
        if(i==0)
        {
            if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == NOERROR)
            {
               WaitNSeconds(1);
               Start7_3_7_2();  // Переход на конфигурацию 1А
            }
        }

        if(i==6)
        {
            if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == NOERROR)
            {
               WaitNSeconds(1);
               Start7_3_7_6();  // Переход на конфигурацию 5А
            }
        }

            QDialog *dlg = new QDialog;
            QVBoxLayout *lyout = new QVBoxLayout;
            QLabel *lbl = new QLabel;
            lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "\
                           "нагрузки по всем фазам " +QString::number(PhiLoad[i])+ " град. и частотой 51 Гц;");
            lyout->addWidget(lbl);
            lbl=new QLabel("Значения напряжений по фазам " +QString::number(U[i])+ " В;");
            lyout->addWidget(lbl);
            if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != Config::MTM_83)
            {
                lbl=new QLabel("Значения токов по фазам " +QString::number(I[i])+ " А;");
                lyout->addWidget(lbl);
            }
            QPushButton *pb = new QPushButton("Готово");
            connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
            lyout->addWidget(pb);
            pb = new QPushButton("Отмена");
            connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
            connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
            lyout->addWidget(pb);
            dlg->setLayout(lyout);
            dlg->exec();

           if(Cancelled)
           break;

        TuneControlType = 0;
        GetExternalData();
        ReadAnalogMeasurements();
        WaitNSeconds(1);
        FillBd1(this);

        if(PhiLoad[i] >= 180)
        {
            ReportHeader.PhiloadA = QString::number(360 + Bda_block.phi_next_f[3], 'f', 3);
            RealData.d[0] = 360 - RealData.d[0];
            RealData.d[1] = 360 - RealData.d[1];
            RealData.d[2] = 360 - RealData.d[2];
        }
        else
        {
            ReportHeader.PhiloadA = QString::number(Bda_block.phi_next_f[3], 'f', 3);
        }

        ReportHeader.PhiloadB = QString::number(Bda_block.phi_next_f[4] - Bda_block.phi_next_f[1], 'f', 3);
        if(PhiLoad[i] >= 90)
        {
           ReportHeader.PhiloadC = QString::number(360 + Bda_block.phi_next_f[5] - Bda_block.phi_next_f[2], 'f', 3);
        }
        else
        {
           ReportHeader.PhiloadC = QString::number(Bda_block.phi_next_f[5] - Bda_block.phi_next_f[2], 'f', 3);
        }
        ReportHeader.PhiUAB   = QString::number((-Bda_block.phi_next_f[1]), 'f', 3);
        ReportHeader.PhiUBC   = QString::number((360 - Bda_block.phi_next_f[2] + Bda_block.phi_next_f[1]), 'f', 3);
        ReportHeader.OffsetF  = QString::number(100*((Bda_block.Frequency/RealData.f[0])-1), 'f', 3);
        ReportHeader.OffsetUA = QString::number(100*((Bda_block.IUefNat_filt[0]/RealData.u[0])-1), 'f', 3);
        ReportHeader.OffsetUB = QString::number(100*((Bda_block.IUefNat_filt[1]/RealData.u[1])-1), 'f', 3);
        ReportHeader.OffsetUC = QString::number(100*((Bda_block.IUefNat_filt[2]/RealData.u[2])-1), 'f', 3);
        ReportHeader.OffsetIA = QString::number(100*((Bda_block.IUefNat_filt[3]/RealData.i[0])-1), 'f', 3);
        ReportHeader.OffsetIB = QString::number(100*((Bda_block.IUefNat_filt[4]/RealData.i[1])-1), 'f', 3);
        ReportHeader.OffsetIC = QString::number(100*((Bda_block.IUefNat_filt[5]/RealData.i[2])-1), 'f', 3);

        // Играемся с углами, чтобы все было в одних значениях и с одинаковыми знаками
        if((RealData.d[0]>0 && ReportHeader.PhiloadA.toFloat() < 0) || (RealData.d[0]<0 && ReportHeader.PhiloadA.toFloat() > 0))
        {
           ReportHeader.OffsetPhiloadA = QString::number(RealData.d[0] + ReportHeader.PhiloadA.toFloat(), 'f', 3);
           RealData.d[0] = -RealData.d[0];
        }
        else
           ReportHeader.OffsetPhiloadA = QString::number(RealData.d[0] - ReportHeader.PhiloadA.toFloat(), 'f', 3);

        if((RealData.d[1]>0 && ReportHeader.PhiloadB.toFloat() < 0) || (RealData.d[1]<0 && ReportHeader.PhiloadB.toFloat() > 0))
        {
           ReportHeader.OffsetPhiloadB = QString::number(RealData.d[1] + ReportHeader.PhiloadB.toFloat(), 'f', 3);
           RealData.d[1] = -RealData.d[1];
        }
        else
           ReportHeader.OffsetPhiloadB = QString::number(RealData.d[1] - ReportHeader.PhiloadB.toFloat(), 'f', 3);

        if((RealData.d[2]>0 && ReportHeader.PhiloadC.toFloat() < 0) || (RealData.d[2]<0 && ReportHeader.PhiloadC.toFloat() > 0))
        {
           ReportHeader.OffsetPhiloadC = QString::number(RealData.d[2] + ReportHeader.PhiloadC.toFloat(), 'f', 3);
           RealData.d[2] = -RealData.d[2];
        }
        else
           ReportHeader.OffsetPhiloadC = QString::number(RealData.d[2] - ReportHeader.PhiloadC.toFloat(), 'f', 3);

        ReportHeader.OffsetPhiUAB = QString::number(RealData.dpsiU[0] - ReportHeader.PhiUAB.toFloat(), 'f', 3);
        ReportHeader.OffsetPhiUBC = QString::number(RealData.dpsiU[1] - ReportHeader.PhiUBC.toFloat(), 'f', 3);

        report->dataManager()->setReportVariable("FreqMIP", QString::number(RealData.f[0], 'f', 3));
        report->dataManager()->setReportVariable("UA_MIP."+QString::number(i), QString::number(RealData.u[0], 'f', 3));
        report->dataManager()->setReportVariable("UB_MIP."+QString::number(i), QString::number(RealData.u[1], 'f', 3));
        report->dataManager()->setReportVariable("UC_MIP."+QString::number(i), QString::number(RealData.u[2], 'f', 3));
        report->dataManager()->setReportVariable("IA_MIP."+QString::number(i), QString::number(RealData.i[0], 'f', 3));
        report->dataManager()->setReportVariable("IB_MIP."+QString::number(i), QString::number(RealData.i[1], 'f', 3));
        report->dataManager()->setReportVariable("IC_MIP."+QString::number(i), QString::number(RealData.i[2], 'f', 3));
        report->dataManager()->setReportVariable("PhiLA_MIP."+QString::number(i), QString::number(RealData.d[0], 'f', 3));
        report->dataManager()->setReportVariable("PhiLB_MIP."+QString::number(i), QString::number(RealData.d[1], 'f', 3));
        report->dataManager()->setReportVariable("PhiLC_MIP."+QString::number(i), QString::number(RealData.d[2], 'f', 3));
        report->dataManager()->setReportVariable("PhiUab_MIP."+QString::number(i), QString::number(RealData.dpsiU[0], 'f', 3));
        report->dataManager()->setReportVariable("PhiUbc_MIP."+QString::number(i), QString::number(RealData.dpsiU[1], 'f', 3));
        report->dataManager()->setReportVariable("Freq."+QString::number(i), QString::number(Bda_block.Frequency, 'f', 3));
        report->dataManager()->setReportVariable("UA."+QString::number(i), QString::number(Bda_block.IUefNat_filt[0], 'f', 3));
        report->dataManager()->setReportVariable("UB."+QString::number(i), QString::number(Bda_block.IUefNat_filt[1], 'f', 3));
        report->dataManager()->setReportVariable("UC."+QString::number(i), QString::number(Bda_block.IUefNat_filt[2], 'f', 3));
        report->dataManager()->setReportVariable("IA."+QString::number(i), QString::number(Bda_block.IUefNat_filt[3], 'f', 3));
        report->dataManager()->setReportVariable("IB."+QString::number(i), QString::number(Bda_block.IUefNat_filt[4], 'f', 3));
        report->dataManager()->setReportVariable("IC."+QString::number(i), QString::number(Bda_block.IUefNat_filt[5], 'f', 3));
        report->dataManager()->setReportVariable("PhiLA."+QString::number(i), ReportHeader.PhiloadA);
        report->dataManager()->setReportVariable("PhiLB."+QString::number(i), ReportHeader.PhiloadB);
        report->dataManager()->setReportVariable("PhiLC."+QString::number(i), ReportHeader.PhiloadC);
        report->dataManager()->setReportVariable("PhiUab."+QString::number(i), ReportHeader.PhiUAB);
        report->dataManager()->setReportVariable("PhiUbc."+QString::number(i), ReportHeader.PhiUBC);
        report->dataManager()->setReportVariable("OffsetF."+QString::number(i), ReportHeader.OffsetF);
        report->dataManager()->setReportVariable("OffsetUA."+QString::number(i), ReportHeader.OffsetUA);
        report->dataManager()->setReportVariable("OffsetUB."+QString::number(i), ReportHeader.OffsetUB);
        report->dataManager()->setReportVariable("OffsetUC."+QString::number(i), ReportHeader.OffsetUC);
        report->dataManager()->setReportVariable("OffsetIA."+QString::number(i), ReportHeader.OffsetIA);
        report->dataManager()->setReportVariable("OffsetIB."+QString::number(i), ReportHeader.OffsetIB);
        report->dataManager()->setReportVariable("OffsetIC."+QString::number(i), ReportHeader.OffsetIC);
        report->dataManager()->setReportVariable("OffsetPhiloadA."+QString::number(i), ReportHeader.OffsetPhiloadA);
        report->dataManager()->setReportVariable("OffsetPhiloadB."+QString::number(i), ReportHeader.OffsetPhiloadB);
        report->dataManager()->setReportVariable("OffsetPhiloadC."+QString::number(i), ReportHeader.OffsetPhiloadC);
        report->dataManager()->setReportVariable("OffsetPhiUAB."+QString::number(i), ReportHeader.OffsetPhiUAB);
        report->dataManager()->setReportVariable("OffsetPhiUBC."+QString::number(i), ReportHeader.OffsetPhiUBC);

    }

    if (EMessageBox::question(this,"Сохранить","Сохранить протокол поверки?"))
    {
        QString filename = Files::ChooseFileForSave(this, "*.pdf", "pdf");
        if (!filename.isEmpty())
        {
            report->designReport();
            report->printToPDF(filename);
    //        report->previewReport();
          //  report->designReport();
            EMessageBox::information(this, "Успешно!", "Записано успешно!");
        }
        else
            EMessageBox::information(this, "Отменено", "Действие отменено");
    }
    delete report;*/
}
#endif

