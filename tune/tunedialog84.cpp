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
#include "tunedialog84.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/maindef.h"
#include "../check/check84.h"
#include "../gen/maindef.h"
#include "../gen/modulebsi.h"
#include "../gen/files.h"
#include <QScrollArea>
#include <QScrollBar>
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

TuneDialog84::TuneDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent) : EAbstractTuneDialog(parent)
{
    this->S2ConfigForTune = &S2Config;

    C84 = new Config84(*S2ConfigForTune);
    ReportModel = new QStandardItemModel;
    ViewModel = new QStandardItemModel;
    ledit = new QLineEdit;
    ask = new QDialog;
//    Ch80 = new Check80;
    SetBac(&Bac_block, BoardTypes::BT_MEZONIN, sizeof(Bac_block));
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void TuneDialog84::SetupUI()
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

    pb = new QPushButton("Начать поверку");
    #if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(GenerateReport()));
    #endif

    glyout->addWidget(TuneUI(), 2,1,1,1);
    //lyout->addStretch(10);
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
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    TuneTW->addTab(cp1,"Настройка");
    #if PROGSIZE != PROGSIZE_EMUL
    TuneTW->addTab(cp4,"Измеренные параметры");
    #endif
    TuneTW->addTab(area,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные МИП");


    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ
    lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("N1_TT["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,1,i,1,1);
        lbl = new QLabel("KmU["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+3));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,1,i+3,1,1);
        lbl = new QLabel("KmI1["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+6));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,3,i,1,1);
        lbl = new QLabel("KmI2["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+9));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,3,i+3,1,1);
        lbl = new QLabel("KmI4["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+12));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,5,i,1,1);
        lbl = new QLabel("KmI8["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+15));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,5,i+3,1,1);
        lbl = new QLabel("KmI16["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+18));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,7,i,1,1);
        lbl = new QLabel("KmI32["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+21));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,7,i+3,1,1);
        lbl = new QLabel("TKPsi_a["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+24));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,9,i,1,1);
        lbl = new QLabel("TKPsi_b["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i+3,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+27));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,9,i+3,1,1);
    }

    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("DPsi["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i,1,1);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+30));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,11,i,1,1);
        lbl = new QLabel("TKUa["+QString::number(i)+"]");
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
        glyout->addWidget(le,15,i,1,1);
    }

    lbl=new QLabel("K_freq");
    //lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,16,0,1,1);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName("tune48");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,17,0,1,1);
    lbl=new QLabel("Art");
    //lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,16,1,1,1);
    le = new QLineEdit("");
    le->setObjectName("tune49");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,17,1,1,1);
    lbl=new QLabel("Brt");
    //lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,16,2,1,1);
    le = new QLineEdit("");
    le->setObjectName("tune50");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,17,2,1,1);
    lbl=new QLabel("Tmk0");
    //lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,16,3,1,1);
    le = new QLineEdit("");
    le->setObjectName("tune51");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,17,3,1,1);
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

    QVBoxLayout *vlyout = new QVBoxLayout;
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
    cp3->setLayout(vlyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);

}

QHBoxLayout *TuneDialog84::MipPars(int parnum, const QString &groupname)
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
void TuneDialog84::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Сохранение текущей конфигурации...");
    lbls.append("3. Отображение диалога выбора режима контроля показаний...");
    lbls.append("4. Отображение схемы подключения...");
    lbls.append("5. 7.2.3. Проверка связи РЕТОМ и МИП...");
    lbls.append("6. 7.3.1. Получение настроечных коэффициентов...");
    lbls.append("8. Установка коэффициентов...");
    lbls.append("9. 7.3.2. Получение текущих аналоговых данных...");
    lbls.append("10. Сохранение значений фильтра...");
    lbls.append("11. 7.3.3. Расчёт коррекции смещений сигналов по фазе...");
    lbls.append("12. 7.3.4. Расчёт коррекции по частоте...");
    lbls.append("13. 7.3.5. Отображение ввода трёхфазных значений...");
    lbls.append("14. 7.3.6.1. Получение текущих аналоговых данных...");
    lbls.append("15. 7.3.6.2. Расчёт коррекции взаимного влияния каналов...");
    lbls.append("16. 7.3.7.1. Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям...");
    lbls.append("17. 7.3.7.2. Сохранение конфигурации...");
    lbls.append("18. 7.3.7.3. Получение текущих аналоговых данных...");
    lbls.append("19. 7.3.7.4. Ввод измеренных значений...");
    lbls.append("20. 7.3.7.5. Расчёт настроечных коэффициентов по токам, напряжениям и углам...");
    lbls.append("21. 7.3.7.6. Сохранение конфигурации...");
    lbls.append("22. 7.3.7.7. Отображение ввода трёхфазных значений...");
    lbls.append("23. 7.3.7.8. Получение текущих аналоговых данных...");
    lbls.append("24. 7.3.7.10. Расчёт настроечных коэффициентов по токам, напряжениям и углам...");
    lbls.append("25. 7.3.8.1. Запись настроечных коэффициентов и переход на новую конфигурацию...");
    lbls.append("26. 7.3.8.2. Проверка аналоговых данных...");
    lbls.append("27. 7.3.9. Восстановление сохранённой конфигурации и проверка...");
}

void TuneDialog84::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::SaveWorkConfig); // Сохранение текущей конфигурации
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::ShowControlChooseDialog); // Отображение диалога выбора режима контроля показаний
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Show3PhaseScheme); // Отображение схемы подключения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_2_3); // Проверка связи РЕТОМ и МИП
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_1); // Получение настроечных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::SetNewTuneCoefs); // Установка коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::SaveUeff); // Сохранение значений фильтра
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_3); // Расчёт коррекции по фазе
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_4); // Расчёт коррекции по частоте
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_5); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_6_2); // Расчёт коррекции взаимного влияния каналов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_1); // Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_2); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_3); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_4); // Ввод измеренных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_5); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_6); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_7); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_8); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_7_10); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_8_1); // Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_8_2); // Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog84::Start7_3_9); // Восстановление сохранённой конфигурации и проверка
    pf[lbls.at(count++)] = func;
}


#endif

void TuneDialog84::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_block.N1_TT[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(Bac_block.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_block.KmI1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(Bac_block.KmI2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+12), QString::number(Bac_block.KmI4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+15), QString::number(Bac_block.KmI8[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+18), QString::number(Bac_block.KmI16[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+21), QString::number(Bac_block.KmI32[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+24), QString::number(Bac_block.TKPsi_a[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+27), QString::number(Bac_block.TKPsi_b[i], 'f', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i+30), QString::number(Bac_block.DPsi[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+36), QString::number(Bac_block.TKUa[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+42), QString::number(Bac_block.TKUb[i], 'f', 5));

    }

    WDFunc::SetLEData(this, "tune48", QString::number(Bac_block.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune49", QString::number(Bac_block.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune50", QString::number(Bac_block.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune51", QString::number(Bac_block.Tmk0, 'f', 5));

}

void TuneDialog84::FillNewBac()
{
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_newblock.N1_TT[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(Bac_newblock.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_newblock.KmI1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(Bac_newblock.KmI2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+12), QString::number(Bac_newblock.KmI4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+15), QString::number(Bac_newblock.KmI8[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+18), QString::number(Bac_newblock.KmI16[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+21), QString::number(Bac_newblock.KmI32[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+24), QString::number(Bac_newblock.TKPsi_a[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+27), QString::number(Bac_newblock.TKPsi_b[i], 'f', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i+30), QString::number(Bac_newblock.DPsi[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+36), QString::number(Bac_newblock.TKUa[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+42), QString::number(Bac_newblock.TKUb[i], 'f', 5));
    }

    WDFunc::SetLEData(this, "tune48", QString::number(Bac_newblock.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune49", QString::number(Bac_newblock.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune50", QString::number(Bac_newblock.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune51", QString::number(Bac_newblock.Tmk0, 'f', 5));
}

void TuneDialog84::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum);
    QString tmps;
    for (int i = 0; i < 3; i++)
    {
        WDFunc::LEData(this, "tune"+QString::number(i), tmps);
        Bac_block.N1_TT[i]=tmps.toUInt();
        WDFunc::LEData(this, "tune"+QString::number(i+3), tmps);
        Bac_block.KmU[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+6), tmps);
        Bac_block.KmI1[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+9), tmps);
        Bac_block.KmI2[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+12), tmps);
        Bac_block.KmI4[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+15), tmps);
        Bac_block.KmI8[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+18), tmps);
        Bac_block.KmI16[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+21), tmps);
        Bac_block.KmI32[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+24), tmps);
        Bac_block.TKPsi_a[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+27), tmps);
        Bac_block.TKPsi_b[i]=ToFloat(tmps);
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::LEData(this, "tune"+QString::number(i+30), tmps);
        Bac_block.DPsi[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+36), tmps);
        Bac_block.TKUa[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+42), tmps);
        Bac_block.TKUb[i]=ToFloat(tmps);
    }

    WDFunc::LEData(this, "tune48", tmps);
    Bac_block.K_freq=ToFloat(tmps);
    WDFunc::LEData(this, "tune49", tmps);
    Bac_block.Art=ToFloat(tmps);
    WDFunc::LEData(this, "tune50", tmps);
    Bac_block.Brt=ToFloat(tmps);
    WDFunc::LEData(this, "tune51", tmps);
    Bac_block.Tmk0=ToFloat(tmps);

}

void TuneDialog84::SetDefCoefs()
{
    Bac_block.Art = 44.65;
    Bac_block.Brt = 3345.75;
    Bac_block.Tmk0 = 24;
    Bac_block.K_freq = 1;

    for (int i=0; i<6; i++)
    {
        Bac_block.DPsi[i] = 0;
        Bac_block.TKUa[i] = 0;
        Bac_block.TKUb[i] = 0;
    }

    for (int i=0; i<3; i++)
    {
        Bac_block.KmU[i] = 1;
        Bac_block.KmI1[i] = 1;
        Bac_block.KmI16[i] = 1;
        Bac_block.KmI2[i] = 1;
        Bac_block.KmI32[i] = 1;
        Bac_block.KmI8[i] = 1;
        Bac_block.N1_TT[i] = 10;
        Bac_block.TKPsi_a[i] = 0;
        Bac_block.TKPsi_b[i] = 0;
    }
    Bac_block.KmI4[0] =  0.997060814;
    Bac_block.KmI4[1] =  0.993458061;
    Bac_block.KmI4[2] =  0.992731500;

    FillBac(BoardTypes::BT_BASE);
}

int TuneDialog84::SetNewTuneCoefs()
{
    Bac_newblock.Art = Bac_block.Art;
    Bac_newblock.Brt = Bac_block.Brt;
    Bac_newblock.Tmk0 = Bac_block.Tmk0;
    Bac_newblock.K_freq = Bac_block.K_freq;

    for (int i=0; i<6; i++)
    {
        Bac_newblock.DPsi[i] = Bac_block.DPsi[i];
        Bac_newblock.TKUa[i] = Bac_block.TKUa[i];
        Bac_newblock.TKUb[i] = Bac_block.TKUb[i];
    }

    for (int i=0; i<3; i++)
    {
        Bac_newblock.KmU[i] = Bac_block.KmU[i];
        Bac_newblock.KmI1[i] = Bac_block.KmI1[i];
        Bac_newblock.KmI16[i] = Bac_block.KmI16[i];
        Bac_newblock.KmI2[i] = Bac_block.KmI2[i];
        Bac_newblock.KmI32[i] = Bac_block.KmI32[i];
        Bac_newblock.KmI4[i] = Bac_block.KmI4[i];
        Bac_newblock.KmI8[i] = Bac_block.KmI8[i];
        Bac_newblock.N1_TT[i] = Bac_block.N1_TT[i];
        Bac_newblock.TKPsi_a[i] = Bac_block.TKPsi_a[i];
        Bac_newblock.TKPsi_b[i] = Bac_block.TKPsi_b[i];
    }
    return Error::ER_NOERROR;
}

float TuneDialog84::ToFloat(QString text)
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
void TuneDialog84::StartMip()
{
    mipcanal = new iec104;
    connect(mipcanal,SIGNAL(signalsready(Parse104::Signals104&)),this,SLOT(ParseMipData(Parse104::Signals104&)));
    connect(this,SIGNAL(stopall()),mipcanal,SLOT(Stop()));
}

void TuneDialog84::ParseMipData(Parse104::Signals104 &Signal)
{
    // precision
    static int Precisions[46] = {0,4,4,4,3,3,3,4,4,4,4,3,3,3,3,3,3,1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.SigNum;
    if (index != quint32(-1))
    {
        MipDat[index] = Signal.SigVal;
        /*if ((index >= 11) && (index <= 13))
            MipDat[index] = -MipDat[index]; // у МИП-а знак угла отрицательный*/
        WDFunc::SetLBLText(this, "mip"+QString::number(index), QString::number(Signal.SigVal, 'f', Precisions[index]));

    }
}

int TuneDialog84::CheckMip()
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
            return Error::ER_GENERALERROR;
        if (!IsWithinLimits(tmpd, *VTC, *TTC))
        {
            EMessageBox::information(this, "Внимание", "Несовпадение МИП по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+\
                      " +/- "+QString::number(*TTC,'f',4));
            return Error::ER_GENERALERROR;
        }
        ++VTC;
        ++TTC;
    }
    return Error::ER_NOERROR;
}

bool TuneDialog84::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

void TuneDialog84::SetTuneMode()
{
    TuneControlType = sender()->objectName().toInt();
}

void TuneDialog84::StopMip()
{
    emit stopall();
}

int TuneDialog84::ShowControlChooseDialog()
{
    TuneControlType = TUNERET; // по-умолчанию тип контроля - по РЕТОМу
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Выберите метод подтверждения измеряемых данных:");
    lyout->addWidget(lbl);
    QPushButton *pb = new QPushButton("Автоматически по показаниям МИП-02");
    pb->setObjectName(QString::number(TUNEMIP));
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneMode()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Вручную");
    pb->setObjectName(QString::number(TUNEMAN));
    connect(pb,SIGNAL(clicked()),this,SLOT(SetTuneMode()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Автоматически по прибору РЕТОМ");
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
        return Error::ER_GENERALERROR;
    else
        return Error::ER_NOERROR;
}

int TuneDialog84::Show3PhaseScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPixmap pmp;

    pmp.load("../tune81.png");

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
    dlg->exec();
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_2_3()
{
    if (TuneControlType == TUNEMIP)
    {
        GED_Type = TD_GED_U; // любой параметр для проверки связи сгодится
        return GetExternalData();
    }
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_1()
{

    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl=new QLabel("Для регулировки необходимо поместить прибор в термокамеру" \
                           "с диапазоном регулирования температуры от минус 20 до +60°С."\
                           "Установить нормальное значение температуры в камере 20±5°С."\
                           "Источники сигналов и эталонный прибор остаются вне камеры при нормальной температуре.");
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

    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_2()
{
   // if (ModuleBSI::GetHealth() & HTH_REGPARS) // если нет настроечных параметров в памяти модуля
   //     return Error::ER_RESEMPTY;
   // else
   // {
        // запись настроечных коэффициентов в модуль
       // SetDefCoefs();
       // if (Commands::WriteBac(BT_MEZONIN, &Bac_block, sizeof(Bac)) == Error::ER_NOERROR)
       // {
            // получение настроечных коэффициентов от модуля
            if (Commands::GetBac(BT_NONE, &Bac_block, sizeof(Bac)) != Error::ER_NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return Error::ER_GENERALERROR;
            }
            // обновление коэффициентов в соответствующих полях на экране
            FillBac(0);
            return Error::ER_NOERROR;
       // }
       // else
       //     return Error::ER_GENERALERROR;
   // }
}

int TuneDialog84::Start7_3_3()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_4()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_5()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_6_2()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_1()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_2()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_3()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_4()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_5()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_6()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_7()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_8()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_7_10()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_8_1()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_8_2()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::Start7_3_9()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::GetExternalData()
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
            return Error::ER_NOERROR;
        }
        return Error::ER_GENERALERROR;
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
                spb->setValue(C84->Bci_block.Imax[i]);
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
            return Error::ER_GENERALERROR;
        return Error::ER_NOERROR;
    }
    case TUNERET:
    {
        for (int i=0; i<3; i++)
        {
            RealData.u[i] = 60.0;
            RealData.i[i] = C84->Bci_block.Imax[i];
            RealData.d[i] = 0.0;
            RealData.f[i] = 50.0;
        }
        return Error::ER_NOERROR;
    }
    }
    return Error::ER_GENERALERROR;
#else
    return Error::ER_NOERROR;
#endif
}

void TuneDialog84::GetBdAndFillMTT()
{

}

int TuneDialog84::SaveWorkConfig()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::ReadAnalogMeasurements()
{
    return Error::ER_NOERROR;
}

int TuneDialog84::SaveUeff()
{
/*    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i=0; i<6; i++)
        IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i]; */
    return Error::ER_NOERROR;
}

int TuneDialog84::ShowRetomDialog(double U, double I)
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим токов и напряжений 1-ой тройки (Uabc, Iabc) с углами "\
                   "сдвига по фазам: А - 0 град., В - 240 град., С - 120 град.,\n"\
                   "Значения напряжений: "+QString::number(U, 'g', 2)+" В, токов: "+QString::number(I, 'g', 2)+" А");
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
    return Error::ER_NOERROR;
}

/*int TuneDialog84::ShowRetomDialog2(double U)
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
    return Error::ER_NOERROR;
}*/

int TuneDialog84::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&C84->Bci_block,&Bci_block_work,sizeof(Config84::Bci));
    if (Commands::WriteFile(&C84->Bci_block, CM_CONFIGFILE, S2ConfigForTune) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

void TuneDialog84::SetExtData()
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

void TuneDialog84::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == nullptr)
        return;
    StdFunc::Cancel();
    dlg->close();
}

QWidget *TuneDialog84::Bd1W(QWidget *parent)
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

void TuneDialog84::FillBd1(QWidget *parent)
{

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i), WDFunc::StringValueWithCheck(Bda_block.Ueff_ADC[i], 4));
    }

     WDFunc::SetLBLText(parent, "value6", WDFunc::StringValueWithCheck(Bda_block.Frequency, 4));
     WDFunc::SetLBLText(parent, "value7", WDFunc::StringValueWithCheck(Bda_block.Pt100, 4));

}

void TuneDialog84::CancelTune()
{
    StdFunc::Cancel();
}

void TuneDialog84::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {

    case C84_BDA_IN: // Блок #1
        FillBd1(this);
        break;

    default:
        return;
    }
}


int TuneDialog84::TunePt100Channel()
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Количество усреднений");
    ledit->setObjectName("N");
    QPushButton* pb = new QPushButton;
    ask->setAttribute(Qt::WA_DeleteOnClose);

    if (EAbstractTuneDialog::CheckPassword() == Error::ER_NOERROR)
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
        ask->setAttribute(Qt::WA_DeleteOnClose);
        ask->setLayout(vlyout);
        ask->show();
        //ask->setModal(false);


       return Error::ER_NOERROR;

    }
    return Error::ER_GENERALERROR;
}


int TuneDialog84::TuneChannel()
{

    int i;
    QString tmps;
    WDFunc::LEData(ask, "N", tmps);
    N=tmps.toUInt();
    ask->close();
    float sum = 0.0;

    Start7_3_2();

       if (Show80() == Error::ER_GENERALERROR)
       return Error::ER_GENERALERROR;
       else
       {
           for(i = 0; i<N; i++)
           {
             if(Commands::GetBda(1, &BdaPt100_80Om, sizeof(BdaPt100_80Om)) == Error::ER_NOERROR)
             sum += BdaPt100_80Om.Pt100;
             else
             return Error::ER_GENERALERROR;
           }
           BdaPt100_80Om.Pt100 = sum/N; // усредняем
       }

                                          if (Show120() == Error::ER_GENERALERROR)
       return Error::ER_GENERALERROR;
       else
       {
           for(i = 0; i<N; i++)
           {
             Commands::GetBda(1, &BdaPt100_120Om, sizeof(BdaPt100_120Om));
           }
       }

       CalcNewPt100Coefs();

       FillBac(0);
       return Error::ER_NOERROR;

}

int TuneDialog84::Show80()
{
    if (EMessageBox::question(this,"Настройка",\
                                 "Подключите банк сопротивлений к модулю\n"
                                 "На банке сопротивлений задайте напряжение 80 Ом"\
                                 " и нажмите OK", nullptr, "Ok" , "Close"))
        return Error::ER_NOERROR;
    else
        return Error::ER_GENERALERROR;
}

int TuneDialog84::Show120()
{

    if (EMessageBox::question(this,"Настройка",\
                                 "Подключите банк сопротивлений к модулю\n"
                                 "На банке сопротивлений задайте напряжение 120 Ом"\
                                 " и нажмите OK", nullptr, "Ok" , "Close"))
    {

        return Error::ER_NOERROR;
    }
    else
        return Error::ER_GENERALERROR;
}

void TuneDialog84::CalcNewPt100Coefs()
{


}



void TuneDialog84::GenerateReport()
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
            if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == Error::ER_NOERROR)
            {
               WaitNSeconds(1);
               Start7_3_7_2();  // Переход на конфигурацию 1А
            }
        }

        if(i==6)
        {
            if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == Error::ER_NOERROR)
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

