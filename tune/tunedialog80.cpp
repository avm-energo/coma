#include <QTime>
#include <QtMath>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QInputDialog>
#include <QCoreApplication>
#include <QTableView>
#include "tunedialog80.h"
#include "../config/config80.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/modulebsi.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../check/check80.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../config/confdialog80.h"
#include "../iec104/iec104.h"

TuneDialog80::TuneDialog80(QVector<S2::DataRec> &S2Config, QWidget *parent) :
    EAbstractTuneDialog(parent)
{
    this->S2ConfigForTune = &S2Config;

    C80 = new Config80(*S2ConfigForTune);
    ReportModel = new QStandardItemModel;
    ViewModel = new QStandardItemModel;
//    Ch80 = new Check80;
    SetBac(&Bac_block, BoardTypes::BT_MEZONIN, sizeof(Bac_block));
    setAttribute(Qt::WA_DeleteOnClose);
    PrepareConsts();
    SetupUI();
    //GenerateReport();
}

void TuneDialog80::SetupUI()
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *cp1 = new QWidget;// = TuneUI();

    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать поверку");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(GenerateReport()));
#endif
    lyout->addWidget(TuneUI());
    //lyout->addStretch(10);
    lyout->addWidget(pb, Qt::AlignRight|Qt::AlignTop);

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
    QGridLayout *glyout = new QGridLayout;
    QTabWidget *TuneTW = new QTabWidget;

    TuneTW->addTab(cp1,"Настройка");
    #if PROGSIZE != PROGSIZE_EMUL
    TuneTW->addTab(cp4,"Измеренные параметры");
    #endif
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные МИП");


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
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addWidget(BottomUI());
    lyout->addStretch(1);
    cp2->setLayout(lyout);

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

QHBoxLayout *TuneDialog80::MipPars(int parnum, const QString &groupname)
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
void TuneDialog80::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Сохранение текущей конфигурации...");
    lbls.append("3. Отображение диалога выбора режима контроля показаний...");
    lbls.append("4. Отображение схемы подключения...");
    lbls.append("5. 7.2.3. Проверка связи РЕТОМ и МИП...");
    lbls.append("6. 7.3.1. Получение настроечных коэффициентов...");
    lbls.append("7. 7.3.1.1. Установка настроечных коэффициентов по умолчанию...");
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

void TuneDialog80::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::SaveWorkConfig); // Сохранение текущей конфигурации
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::ShowControlChooseDialog); // Отображение диалога выбора режима контроля показаний
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Show3PhaseScheme); // Отображение схемы подключения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_2_3); // Проверка связи РЕТОМ и МИП
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_1); // Получение настроечных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_1_1); // Установка настроечных коэффициентов по умолчанию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::SetNewTuneCoefs); // Установка коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::SaveUeff); // Сохранение значений фильтра
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_3); // Расчёт коррекции по фазе
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_4); // Расчёт коррекции по частоте
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_5); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_6_2); // Расчёт коррекции взаимного влияния каналов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_1); // Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_2); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_3); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_4); // Ввод измеренных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_5); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_6); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_7); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_8); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_10); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_8_1); // Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_8_2); // Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_9); // Восстановление сохранённой конфигурации и проверка
    pf[lbls.at(count++)] = func;
}

void TuneDialog80::GetBdAndFillMTT()
{

}
#endif

void TuneDialog80::FillBac()
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_block.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_block.KmI_5[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+12), QString::number(Bac_block.KmI_1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+18), QString::number(Bac_block.DPsi[i], 'f', 5));
    }
    WDFunc::SetLEData(this, "tune24", QString::number(Bac_block.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune25", QString::number(Bac_block.Kinter, 'f', 5));
}

void TuneDialog80::FillNewBac()
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_newblock.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_newblock.KmI_5[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+12), QString::number(Bac_newblock.KmI_1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+18), QString::number(Bac_newblock.DPsi[i], 'f', 5));
    }
    WDFunc::SetLEData(this, "tune24", QString::number(Bac_newblock.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune25", QString::number(Bac_newblock.Kinter, 'f', 5));
}

void TuneDialog80::FillBackBac()
{
    QString tmps;
    for (int i = 0; i < 6; i++)
    {
        WDFunc::LEData(this, "tune"+QString::number(i), tmps);
        Bac_block.KmU[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+6), tmps);
        Bac_block.KmI_5[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+12), tmps);
        Bac_block.KmI_1[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+18), tmps);
        Bac_block.DPsi[i]=ToFloat(tmps);
    }
    WDFunc::LEData(this, "tune24", tmps);
    Bac_block.K_freq=ToFloat(tmps);
    WDFunc::LEData(this, "tune25", tmps);
    Bac_block.Kinter=ToFloat(tmps);
}

void TuneDialog80::SetDefCoefs()
{
    Bac_block.Kinter = 0.0;
    Bac_block.K_freq = 1.0;
    for (int i=0; i<6; i++)
    {
        Bac_block.DPsi[i] = 0.0;
        Bac_block.KmI_1[i] = 1.0;
        Bac_block.KmI_5[i] = 1.0;
        Bac_block.KmU[i] = 1.0;
    }
    FillBac();
}

void TuneDialog80::PrepareConsts()
{
    // подготовка констант для проверки данных МИПа
    MVTC.u = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_83) ? S0 : V60;
    MTTC.u = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_83) ? FLT_MAX : TH005;
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81)
    {
        MVTC.i[0] = MVTC.i[1] = MVTC.i[2] = S0;
        MTTC.i = FLT_MAX;
    }
    else
    {
       /*MVTC.i[0] = ConfDialog80::C80->Bci_block.inom2[0];
        MVTC.i[1] = ConfDialog80::C80->Bci_block.inom2[1];
        MVTC.i[2] = ConfDialog80::C80->Bci_block.inom2[2];*/
        MVTC.i[0] = C80->Bci_block.inom2[0];
        MVTC.i[1] = C80->Bci_block.inom2[1];
        MVTC.i[2] = C80->Bci_block.inom2[2];
        MTTC.i = TH005;
    }
}

#if PROGSIZE != PROGSIZE_EMUL
int TuneDialog80::Start7_2_3()
{
    if (TuneControlType == TUNEMIP)
    {
        GED_Type = TD_GED_U; // любой параметр для проверки связи сгодится
        return GetExternalData();
    }
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_1()
{
    if (ModuleBSI::GetHealth() & HTH_REGPARS) // если нет настроечных параметров в памяти модуля
        return Error::ER_RESEMPTY;
    else
    {
        // получение настроечных коэффициентов от модуля
        if (Commands::GetBac(BT_MEZONIN, &Bac_block, sizeof(Bac_block)) != Error::ER_NOERROR)
        {
            EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
            return Error::ER_GENERALERROR;
        }
        // обновление коэффициентов в соответствующих полях на экране
        FillBac();
        // проверка коэффициентов на правильность в соотв. с п. 7.3.1 "Д2"
        if (CheckTuneCoefs())
            return Error::ER_NOERROR;
        else
            return Error::ER_GENERALERROR;
    }
}

int TuneDialog80::Start7_3_1_1()
{
    if (ModuleBSI::GetHealth() & HTH_REGPARS) // если нет настроечных параметров в памяти модуля
        return Error::ER_RESEMPTY;
    else
    {
        // запись настроечных коэффициентов в модуль
        SetDefCoefs();
        if (Commands::WriteBac(BT_MEZONIN, &Bac_block, sizeof(Bac)) == Error::ER_NOERROR)
        {
            // получение настроечных коэффициентов от модуля
            if (Commands::GetBac(BT_NONE, &Bac_block, sizeof(Bac)) != Error::ER_NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return false;
            }
            // обновление коэффициентов в соответствующих полях на экране
            FillBac();
            return Error::ER_NOERROR;
        }
        else
            return Error::ER_GENERALERROR;
    }
}

int TuneDialog80::Start7_3_2()
{
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I1, S0, false);
}

/*    if (num == MSG_7_3_2) maxval=602; // 3~7.3.2, 6~7.3.6.1, 12~7.3.7.3
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
    } */
    /*    if (CheckAnalogValues(maxval-pc.ModuleBsi.MTypeB)) // MType: 0 = 2T0N, 1 = 1T1N, 2 = 0T2N; NTest: 600 = 0T2N, 601 = 1T1N, 602 = 2T0N
        {
            OkMsgSetVisible(num);
            return true;
        }
        else
        {
            ErMsgSetVisible(num);
            return false;
        }*/

int TuneDialog80::Start7_3_3()
{
    float phiMip[6];
    GED_Type = TD_GED_D;
    GetExternalData();
    Bac_newblock.DPsi[0] = 0;
    phiMip[0] = 0;
    phiMip[1] = RealData.dpsiU[0];
    phiMip[2] = RealData.dpsiU[0]+RealData.dpsiU[1];
    phiMip[3] = RealData.d[0];
    phiMip[4] = RealData.d[1]+RealData.dpsiU[0];
    phiMip[5] = RealData.d[2]+RealData.dpsiU[0]+RealData.dpsiU[1];
    int k = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_82) ? 3 : 6;
    for (int i=1; i<k; ++i)
        Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - phiMip[i] - Bda_block.phi_next_f[i];
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_82)
    {
        for (int i=3; i<6; ++i)
             Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - phiMip[i] - Bda_block.phi_next_f[i];

    }

    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_4()
{
    GED_Type = TD_GED_F;
    if (!(GetExternalData() == Error::ER_GENERALERROR))
        Bac_newblock.K_freq = Bac_block.K_freq*RealData.f[0]/Bda_block.Frequency;
    else
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_5()
{
    return ShowRetomDialog(V60, I1);
}

int TuneDialog80::Start7_3_6_2()
{
    double fTmp = Bda_block.IUefNat_filt[0] / IUefNat_filt_old[0] + Bda_block.IUefNat_filt[3] / IUefNat_filt_old[3];
    fTmp /= 2;
    Bac_newblock.Kinter = (fTmp * (1 + 6 * Bac_block.Kinter) - 1) / 6;
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_7_1()
{
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != Config::MTM_81)
        return Error::ER_RESEMPTY;
    GED_Type = TD_GED_U;
    if (GetExternalData() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    if (Start7_3_2() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    for (int i=0; i<3; i++)
    {
        Bac_newblock.KmU[i] = Bac_block.KmU[i] * RealData.u[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmU[i+3] = Bac_block.KmU[i+3] * RealData.u[i] / Bda_block.IUefNat_filt[i+3];
    }
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_7_2()
{
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81)
        return Error::ER_RESEMPTY;

   // Установить в конфигурации токи i2nom в 1 А
    for (int i=0; i<6; i++)
        C80->Bci_block.inom2[i] = I1;
    // послать новые коэффициенты по току в конфигурацию
    if (Commands::WriteFile(&C80->Bci_block, 1, S2ConfigForTune) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    WaitNSeconds(2);
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_7_3()
{
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81)
        return Error::ER_RESEMPTY;
    //ShowRetomDialog(V60, I1);
    if (Start7_3_2() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_7_4()
{
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81)
        return Error::ER_RESEMPTY;
    GED_Type = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_82) ? (TD_GED_I | TD_GED_U) : TD_GED_I;
    return GetExternalData();
}

int TuneDialog80::Start7_3_7_5()
{
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81)
        return Error::ER_RESEMPTY;
    for (int i=0; i<3; ++i)
    {
        if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_82)
            Bac_newblock.KmU[i] = Bac_block.KmU[i] * RealData.u[i] / Bda_block.IUefNat_filt[i];
        else
            Bac_newblock.KmI_1[i] = Bac_block.KmI_1[i] * RealData.i[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_1[i+3] = Bac_block.KmI_1[i+3] * RealData.i[i] / Bda_block.IUefNat_filt[i+3];
    }
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_7_6()
{
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81)
        return Error::ER_RESEMPTY;
    for (int i=0; i<6; ++i)
        C80->Bci_block.inom2[i] = I5;
    if (Commands::WriteFile(&C80->Bci_block, 1, S2ConfigForTune) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    WaitNSeconds(2);
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_7_7()
{
    return ShowRetomDialog(V60, I5);
}

int TuneDialog80::Start7_3_7_8()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    int res = StartCheckAnalogValues(V60, I5, S0, false);
    if (res == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    GED_Type = TD_GED_D | TD_GED_U;
    return GetExternalData();
}

int TuneDialog80::Start7_3_7_10()
{
    for (int i=0; i<3; ++i)
    {
        if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81)
            Bac_newblock.KmI_5[i] = Bac_block.KmI_5[i] * RealData.i[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_5[i+3] = Bac_block.KmI_5[i+3] * RealData.i[i] / Bda_block.IUefNat_filt[i+3];
    }
    return Error::ER_NOERROR;
}

int TuneDialog80::Start7_3_8_1()
{
    FillNewBac();
    // 1. Отправляем настроечные параметры в модуль
    return Error::ER_NOERROR;//Commands::WriteBac(BT_MEZONIN, &Bac_newblock, sizeof(Bac));
}

int TuneDialog80::Start7_3_8_2()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I5, S0, true);
}

int TuneDialog80::Start7_3_9()
{
    if (EMessageBox::question(this,"Закончить?","Закончить настройку и записать коэффициенты в модуль?"))
    {
        if (!LoadWorkConfig())
            return Error::ER_GENERALERROR;
        // Пишем в модуль посчитанные регулировочные коэффициенты
        WaitNSeconds(5);
        if (Commands::WriteBac(BT_MEZONIN, &Bac_newblock, sizeof(Bac)) != Error::ER_NOERROR)  // Григорий Матвеевич попросил писать коэффициенты сразу в модуль
            return Error::ER_GENERALERROR;
        // переходим на прежнюю конфигурацию
        // измеряем и проверяем
        if (EMessageBox::question(this,"Протокол поверки","Начать поверку?"))
        {
           GenerateReport();
        }
        //ShowRetomDialog(V57, C80->Bci_block.inom2[0]); // I = 1.0 or 5.0 A
        //WaitNSeconds(15);
        //ReadAnalogMeasurements();
        //return StartCheckAnalogValues(V57, C80->Bci_block.inom2[0], S0, true);
        return Error::ER_NOERROR;
    }
    else
        return false;
    if (EMessageBox::question(this,"Вопрос","Очистить память осциллограмм?"))
    {
        StdFunc::SetPrbMessage("Стёрто записей: ");
        if (Commands::EraseTechBlock(TECH_Bo) == Error::ER_NOERROR)
            EMessageBox::information(this, "Внимание", "Стёрто успешно");
        else
            ERMSG("Ошибка при стирании");
    }
    return true;
}

int TuneDialog80::SaveUeff()
{
    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i=0; i<6; i++)
        IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i];
    return Error::ER_NOERROR;
}

int TuneDialog80::ShowRetomDialog(double U, double I)
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "\
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

int TuneDialog80::StartCheckAnalogValues(double u, double i, double deg, bool tol)
{
    double phi = qDegreesToRadians(deg);
    double p = u * i * qCos(phi);
    double q = u * i * qSin(phi);
    double s = qSqrt(p*p + q*q);
    double utol, itol, ptol, dtol, ctol;
    if (tol) // точные погрешности
    {
        utol = TH01; itol = TH0005; ptol = TH01; dtol = TH01; ctol = TH0005;
    }
    else
    {
        utol = TH05; itol = TH005; ptol = TH05; dtol = TH1; ctol = TH005;
    }
    int res;
    switch (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN))
    {
    case Config::MTM_81: // 2t0n
        res = CheckAnalogValues(i, i, S0, S0, S0, S0, S0, utol, itol, ptol, dtol, ctol);
        break;
    case Config::MTM_82:
        res = CheckAnalogValues(u, i, p, q, s, phi, qCos(phi), utol, itol, ptol, dtol, ctol);
        break;
    case Config::MTM_83: // 0t2n
        res = CheckAnalogValues(u, u, S0, S0, S0, S0, S0, utol, itol, ptol, dtol, ctol);
        break;
    default:
        return Error::ER_GENERALERROR;
    }
    return res;
}

int TuneDialog80::GetExternalData()
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
                spb->setValue(C80->Bci_block.inom2[i]);
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
            RealData.i[i] = C80->Bci_block.inom2[i];
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

int TuneDialog80::SaveWorkConfig()
{
    if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == Error::ER_NOERROR)
        memcpy(&Bci_block_work,&C80->Bci_block,sizeof(Config80::Bci));
    else
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog80::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&C80->Bci_block,&Bci_block_work,sizeof(Config80::Bci));
    if (Commands::WriteFile(&C80->Bci_block, CM_CONFIGFILE, S2ConfigForTune) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

void TuneDialog80::SetExtData()
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
    Cancelled = false;
    dlg->close();
}

void TuneDialog80::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == nullptr)
        return;
    Cancelled = true;
    dlg->close();
}

// CHECKING

int TuneDialog80::CheckTuneCoefs()
{
    double ValuesToCheck[26] = {S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,\
                               S0,S0,S0,S0,S0,S0,S1,S0};
    double ThresholdsToCheck[26] = {TH002,TH002,TH002,TH002,TH002,TH002,TH002,TH002,TH002,TH002,TH002,TH002,\
                                   TH002,TH002,TH002,TH002,TH002,TH002,TH1,TH1,TH1,TH1,TH1,TH1,TH002,TH0005};
    double *VTC = ValuesToCheck;
    double *TTC = ThresholdsToCheck;
    int res = Error::ER_NOERROR;
    for (int i = 0; i < 26; i++)
    {
        QString tmps;
        WDFunc::LBLText(this, "tune"+QString::number(i), tmps);
        bool ok;
        double tmpd = tmps.toDouble(&ok);
        if (!ok)
            return Error::ER_GENERALERROR;
        if (!IsWithinLimits(tmpd, *VTC, *TTC))
        {
            EMessageBox::information(this, "Внимание", "Настроечные по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+\
                      " +/- "+QString::number(*TTC,'f',4));
            res=Error::ER_GENERALERROR;
            WDFunc::SetLBLColor(this, "tune"+QString::number(i), "red");
            VTC++;
            TTC++;
        }
    }
    return res;
}

bool TuneDialog80::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

int TuneDialog80::CheckMip()
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

int TuneDialog80::CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct)
{
    //double it = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_83) ? utol : itol; // 0t2n
    //double ut = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_81) ? itol : utol; // 2t0n
    //double ValuesToCheck[45] = {/*TD_TMK,TD_VBAT,*/TD_FREQ,u,u,u,i,i,i,u,u,u,i,i,i,phi,phi,phi,phi,phi,phi,p,p,p,s,s,s,q,q,q, \
                                cosphi,cosphi,cosphi,p,p,p,q,q,q,s,s,s,cosphi,cosphi,cosphi};
    //double ThresholdsToCheck[45] = {/*T25,TH05,*/TH0005,ut,ut,ut,it,it,it,ut,ut,ut,it,it,it,pht,pht,pht,pht,pht,pht,pt,pt,pt,pt,pt,pt,pt,pt,pt,\
                                    ct,ct,ct,pt,pt,pt,pt,pt,pt,pt,pt,pt,ct,ct,ct};
    //double *VTC = ValuesToCheck;
    //double *TTC = ThresholdsToCheck;
    QLocale german(QLocale::German);

    FillBd1(this);

    /*for (int i = 0; i < 43; i++)
    {
        QString tmps;
        WDFunc::LBLText(this, "value"+QString::number(i), tmps);
        bool ok = false;

        //double tmpd = german.toDouble( tmps, &ok );   //для чисел с запятой
        //double tmpd = tmps.toDouble(&ok);           //для чисел с точкой
        if (!ok)
            return Error::ER_GENERALERROR;

        (if (!IsWithinLimits(tmpd,*VTC,*TTC))
        {
            EMessageBox::information(this, "Внимание", "Несовпадение по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+ " +/- " + QString::number(*TTC,'f',4));
            return Error::ER_GENERALERROR;
        }
        ++VTC;
        ++TTC;
    }*/

/*    switch (ntest)

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
*/
    return Error::ER_NOERROR;
}

int TuneDialog80::SetNewTuneCoefs()
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
    return Error::ER_NOERROR;
}

int TuneDialog80::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
     if (Commands::GetBd(BT_BASE, &Bda_block, sizeof(Bda_block)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialog80::StartMip()
{
    mipcanal = new iec104;
    connect(mipcanal,SIGNAL(signalsready(Parse104::Signals104&)),this,SLOT(ParseMipData(Parse104::Signals104&)));
    connect(this,SIGNAL(stopall()),mipcanal,SLOT(Stop()));
}
#endif
void TuneDialog80::ParseMipData(Parse104::Signals104 &Signal)
{
    // precision
    static int Precisions[46] = {0,4,4,4,3,3,3,4,4,4,4,3,3,3,3,3,3,1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.SigNum;
    if (index != -1)
    {
         MipDat[index] = Signal.SigVal;
        //if ((index >= 11) && (index <= 13))
        //   MipDat[index] = -MipDat[index]; // у МИП-а знак угла отрицательный
        WDFunc::SetLBLText(this, "mip"+QString::number(index), QString::number(Signal.SigVal, 'f', Precisions[index]));

    }
}

void TuneDialog80::SetTuneMode()
{
    TuneControlType = sender()->objectName().toInt();
}

void TuneDialog80::StopMip()
{
    emit stopall();
}

int TuneDialog80::ShowControlChooseDialog()
{
    Cancelled = 0;
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

void TuneDialog80::Show1RetomDialog(float U, float A)
{
    Q_UNUSED(U);
    Q_UNUSED(A);
/*    QDialog *dlg = new QDialog;
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
    dlg->exec();*/
}

int TuneDialog80::Show3PhaseScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPixmap pmp;
    switch(ModuleBSI::GetMType(BoardTypes::BT_MEZONIN)) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case Config::MTM_81: // 2t0n
    {
        pmp.load("../tune81.png");
        break;
    }
    case Config::MTM_82:
    {
        pmp.load("../tune82.png");
        break;
    }
    case Config::MTM_83:
    {
        pmp.load("../tune83.png");
        break;
    }
    default:
        return Error::ER_GENERALERROR;
    }
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

void TuneDialog80::MsgClear()
{
/*    for (int i=0; i<MSG_COUNT; i++)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    } */
}

void TuneDialog80::CancelTune()
{
    Cancelled = true;
}
#endif


void TuneDialog80::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}




float TuneDialog80::ToFloat(QString text)
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
QWidget *TuneDialog80::Bd1W(QWidget *parent)
{
    int i;
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";

    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    /*hlyout->addWidget(WDFunc::NewLBL(parent, "Tmk, °С:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value0", ValuesFormat, "Температура кристалла микроконтроллера, °С"), 0);
    hlyout->addWidget(WDFunc::NewLBL(parent, "VBAT, В:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value1", ValuesFormat, "Напряжение аккумуляторной батареи, В"), 0);*/
    hlyout->addWidget(WDFunc::NewLBL(parent, "Частота:"));
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "value0", ValuesFormat, "Частота сигналов, Гц"), Qt::AlignLeft);
    lyout->addLayout(hlyout);
    for (i = 1; i < 7; ++i)
    {
        QString IndexStr = "[" + QString::number(i-1) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "IUNF"+IndexStr),0,(i-1),1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i), ValuesFormat, \
                                          QString::number(i)+"IUNF"+IndexStr+".Истинные действующие значения сигналов"),1,(i-1),1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "IUF"+IndexStr),2,(i-1),1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+6), ValuesFormat, \
                                          QString::number(i+6)+"IUF"+IndexStr+".Действующие значения сигналов по 1-й гармонике\nотносительно ф. А 1-й группы"),3,(i-1),1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PHF"+IndexStr),4,(i-1),1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+12), ValuesFormat, \
                                          QString::number(i+12)+"PHF"+IndexStr+".Угол сдвига между сигналами по первой гармонике\nотносительно ф. А 1-й группы"),5,(i-1),1,1);
    }
    for (i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "PNF"+IndexStr),6,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+19), ValuesFormat, \
                                          QString::number(i+19)+".Истинная активная мощность"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SNF"+IndexStr),6,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+22), ValuesFormat, \
                                          QString::number(i+22)+".Кажущаяся полная мощность"),7,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QNF"+IndexStr),8,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+25), ValuesFormat, \
                                          QString::number(i+25)+".Реактивная мощность"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Cos"+IndexStr),8,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+28), ValuesFormat, \
                                          QString::number(i+28)+".Cos phi по истинной активной мощности"),9,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PF"+IndexStr),10,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+31), ValuesFormat, \
                                          QString::number(i+31)+".Активная мощность по 1-й гармонике"),11,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QF"+IndexStr),10,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+34), ValuesFormat, \
                                          QString::number(i+34)+".Реактивная мощность по 1-й гармонике"),11,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SF"+IndexStr),12,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+37), ValuesFormat, \
                                          QString::number(i+37)+".Полная мощность по 1-й гармонике"),13,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "CosPhi"+IndexStr),12,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+40), ValuesFormat, \
                                          QString::number(i+40)+".Cos phi по 1-й гармонике"),13,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "PHI"+IndexStr),14,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+43), ValuesFormat, \
                                          QString::number(i+43)+".Угол между током и напряжением"),15,i,1,1);
    }
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void TuneDialog80::FillBd1(QWidget *parent)
{
    //WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bd_block0.Tmk));
    //WDFunc::SetLBLText(parent, "value1", WDFunc::StringValueWithCheck(Bd_block0.Vbat));
    WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bda_block.Frequency, 3));
    for (int i = 1; i < 4; i++)
    {
        int Precision = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != Config::MTM_81) ? 3 : 4;
        WDFunc::SetLBLText(parent, "value"+QString::number(i), WDFunc::StringValueWithCheck(Bda_block.IUefNat_filt[i-1], Precision));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+6), WDFunc::StringValueWithCheck(Bda_block.IUeff_filtered[i-1], Precision));
        Precision = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != Config::MTM_83) ? 4 : 3;
        WDFunc::SetLBLText(parent, "value"+QString::number(i+3), WDFunc::StringValueWithCheck(Bda_block.IUefNat_filt[i+2], Precision));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+9), WDFunc::StringValueWithCheck(Bda_block.IUeff_filtered[i+2], Precision));

    }
    for (int i = 1; i < 7; i++)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i+12), WDFunc::StringValueWithCheck(Bda_block.phi_next_f[i-1], 4));
    }

    for (int i=0; i<3; i++)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i+19), WDFunc::StringValueWithCheck(Bda_block.PNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+22), WDFunc::StringValueWithCheck(Bda_block.SNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+25), WDFunc::StringValueWithCheck(Bda_block.QNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+28), WDFunc::StringValueWithCheck(Bda_block.CosPhiNat[i], 4));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+31), WDFunc::StringValueWithCheck(Bda_block.Pf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+34), WDFunc::StringValueWithCheck(Bda_block.Qf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+37), WDFunc::StringValueWithCheck(Bda_block.Sf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+40), WDFunc::StringValueWithCheck(Bda_block.CosPhi[i], 4));
        float PHI = (180*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        WDFunc::SetLBLText(parent, "value"+QString::number(i+43), WDFunc::StringValueWithCheck(PHI, 4));
    }


}

void TuneDialog80::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {

    case C80_BDA_IN: // Блок #1
        FillBd1(this);
        break;

    default:
        return;
    }
}


void TuneDialog80::GenerateReport()
{
    // данные в таблицу уже получены или из файла, или в процессе работы
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
    delete report;
}
#endif

