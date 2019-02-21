#include <QTime>
#include <QtMath>
#include <QInputDialog>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QCoreApplication>
#include "tunedialog85.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/maindef.h"
#include "../check/check85.h"
#include "../gen/maindef.h"
#include "../gen/modulebsi.h"
#include "../gen/files.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

TuneDialog85::TuneDialog85(QVector<S2::DataRec> &S2Config, QWidget *parent) : EAbstractTuneDialog(parent)
{
    this->S2ConfigForTune = &S2Config;

    C85 = new Config85(*S2ConfigForTune);
    ReportModel = new QStandardItemModel;
    ViewModel = new QStandardItemModel;
    SetBac(&Bac_block, BoardTypes::BT_NONE, sizeof(Bac_block));
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}


void TuneDialog85::SetupUI()
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *cp1 = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать поверку");
    connect(pb,SIGNAL(clicked()),this,SLOT(GenerateReport()));
    lyout->addWidget(TuneUI());
    //lyout->addStretch(10);
    lyout->addWidget(pb, Qt::AlignRight|Qt::AlignTop);
    cp1->setLayout(lyout);

    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
//    #if PROGSIZE != PROGSIZE_EMUL
    QWidget *cp4 = Bd1W(this);
//    #endif
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);

    QLabel *lbl;
    QGridLayout *glyout = new QGridLayout;
    QTabWidget *TuneTW = new QTabWidget;

    TuneTW->addTab(cp1,"Настройка");
//    #if PROGSIZE != PROGSIZE_EMUL
    TuneTW->addTab(cp4,"Измеренные параметры");
//    #endif
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные МИП");

    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("KmU["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,1,i,1,1);
        lbl = new QLabel("KmI_1["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+3));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,3,i,1,1);
        lbl = new QLabel("KmI_4["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+6));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,5,i,1,1);
        lbl = new QLabel("KmU2["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+9));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,7,i,1,1);
    }
    lbl=new QLabel("K_freq:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,0,1,1);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName("tune13");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,8,1,1,2);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addWidget(BottomUI(BoardTypes::BT_NONE));
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

#if PROGSIZE != PROGSIZE_EMUL
     if ((!(ModuleBSI::GetHealth() & HTH_REGPARS)) && !StdFunc::IsInEmulateMode()) // есть настроечные коэффициенты в памяти модуля
        ReadTuneCoefsByBac(0); // считать их из модуля и показать на экране
#endif
}

QHBoxLayout *TuneDialog85::MipPars(int parnum, const QString &groupname)
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
void TuneDialog85::SetLbls()
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
    lbls.append("11. 7.3.3. Расчёт коррекции по частоте...");
    lbls.append("12. 7.3.4. Расчёт настроечных коэффициентов по напряжениям 1-ой тройки...");
    lbls.append("13. 7.3.5. Отображение ввода трёхфазных значений...");
    lbls.append("14. 7.3.6.1. Получение текущих аналоговых данных...");
    lbls.append("15. 7.3.6.2. Расчёт коррекции взаимного влияния каналов...");
    lbls.append("16. 7.3.7.1. Расчёт настроечных коэффициентов по напряжениям 2-ой тройки...");
    lbls.append("17. 7.3.7.2. Сохранение конфигурации...");
    lbls.append("18. 7.3.7.3. Получение текущих аналоговых данных...");
    lbls.append("19. 7.3.7.4. Ввод измеренных значений...");
    lbls.append("20. 7.3.7.5. Расчёт настроечных коэффициентов по токам...");
    lbls.append("21. 7.3.7.6. Сохранение конфигурации...");
    lbls.append("22. 7.3.7.7. Отображение ввода трёхфазных значений...");
    lbls.append("23. 7.3.7.8. Получение текущих аналоговых данных...");
    lbls.append("24. 7.3.7.10. Расчёт настроечных коэффициентов по токам...");
    lbls.append("25. 7.3.8.1. Запись настроечных коэффициентов и переход на новую конфигурацию...");
    lbls.append("26. 7.3.8.2. Проверка аналоговых данных...");
    lbls.append("27. 7.3.9. Восстановление сохранённой конфигурации и проверка...");
}

void TuneDialog85::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::SaveWorkConfig); // Сохранение текущей конфигурации
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::ShowControlChooseDialog); // Отображение диалога выбора режима контроля показаний
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Show3PhaseScheme); // Отображение схемы подключения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_2_3); // Проверка связи РЕТОМ и МИП
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_1); // Получение настроечных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_1_1); // Установка настроечных коэффициентов по умолчанию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::SetNewTuneCoefs); // Установка коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::SaveUeff); // Сохранение значений фильтра
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_3); // Расчёт коррекции по фазе
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_4); // Расчёт коррекции по частоте
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_5); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_6_2); // Расчёт коррекции взаимного влияния каналов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_1); // Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_2); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_3); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_4); // Ввод измеренных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_5); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_6); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_7); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_8); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_7_10); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_8_1); // Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_8_2); // Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::Start7_3_9); // Восстановление сохранённой конфигурации и проверка
    pf[lbls.at(count++)] = func;
}
#endif
#if PROGSIZE != PROGSIZE_EMUL
int TuneDialog85::Start7_2_3()
{
    if (TuneControlType == TUNEMIP)
    {
        GED_Type = TD_GED_U; // любой параметр для проверки связи сгодится
        return GetExternalData();
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_1()
{
    if (ModuleBSI::GetHealth() & HTH_REGPARS) // если нет настроечных параметров в памяти модуля
        return Error::ER_RESEMPTY;
    else
    {
        // получение настроечных коэффициентов от модуля
        if (Commands::GetBac(BT_NONE, &Bac_block, sizeof(Bac_block)) != Error::ER_NOERROR)
        {
            EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
            return Error::ER_GENERALERROR;
        }
        // обновление коэффициентов в соответствующих полях на экране
        FillBac(0);
        // проверка коэффициентов на правильность в соотв. с п. 7.3.1 "Д2"
        if (CheckTuneCoefs())
            return Error::ER_NOERROR;
        else
            return Error::ER_GENERALERROR;
    }
}

int TuneDialog85::Start7_3_1_1()
{
    if (ModuleBSI::GetHealth() & HTH_REGPARS) // если нет настроечных параметров в памяти модуля
        return Error::ER_RESEMPTY;
    else
    {
        // запись настроечных коэффициентов в модуль
        SetDefCoefs();
        if (Commands::WriteBac(BT_NONE, &Bac_block, sizeof(Bac)) == Error::ER_NOERROR)
        {
            // получение настроечных коэффициентов от модуля
            if (Commands::GetBac(BT_NONE, &Bac_block, sizeof(Bac)) != Error::ER_NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return false;
            }
            // обновление коэффициентов в соответствующих полях на экране
            FillBac(0);
            return Error::ER_NOERROR;
        }
        else
            return Error::ER_GENERALERROR;
    }
}

int TuneDialog85::Start7_3_2()
{
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I1, S0, false);
}

int TuneDialog85::Start7_3_3()
{
    EMessageBox::question(this,"Сообщение","Подключите напряженческие выходы РЕТОМа ко входам ПЕРВОЙ тройки напряжений модуля!");
    ShowRetomDialog(V60, I1);
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_4()
{
   GED_Type = TD_GED_F;
    if (!(GetExternalData() == Error::ER_GENERALERROR))
    {
        New_Bac_block.K_freq = Bac_block.K_freq*RealData.f[0] / Bda_block.Frequency;

        for (int i=0; i<3; i++)
        {
            New_Bac_block.KmU[i] = Bac_block.KmU[i] * RealData.u[i] / Bda_block.IUefNat_filt[i];
        }
    }
    else
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_5()
{
    EMessageBox::question(this,"Сообщение","Подключите напряженческие выходы РЕТОМа ко входам ВТОРОЙ тройки напряжений модуля!");

    return ShowRetomDialog2(V60);
}

int TuneDialog85::Start7_3_6_2()
{
/*    double fTmp = Bda_block.IUefNat_filt[0] / IUefNat_filt_old[0] + Bda_block.IUefNat_filt[3] / IUefNat_filt_old[3];
    fTmp /= 2;
    Bac_newblock.Kinter = (fTmp * (1 + 6 * Bac_block.Kinter) - 1) / 6; */
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_1()
{
    GED_Type = TD_GED_U;
    if (GetExternalData() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    if (Start7_3_2() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    for (int i=0; i<3; i++)
    {
        New_Bac_block.KmU2[i] = Bac_block.KmU2[i] * RealData.u[i] / Bda_block.IUefNat_filt[i+6];
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_2()
{
   // Установить в конфигурации токи i2nom в 1 А
        C85->Bci_block.ITT2nom = I1;
    // послать новые коэффициенты по току в конфигурацию
    if (Commands::WriteFile(&C85->Bci_block, CM_CONFIGFILE, S2ConfigForTune) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    WaitNSeconds(2);
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_3()
{
    EMessageBox::question(this,"Сообщение","Подключите напряженческие выходы РЕТОМа ко входам ПЕРВОЙ тройки напряжений модуля!");
    ShowRetomDialog(V60, I1);
    if (Start7_3_2() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_4()
{
    GED_Type = (TD_GED_I | TD_GED_U);
    return GetExternalData();
}

int TuneDialog85::Start7_3_7_5()
{
    for (int i=0; i<3; ++i)
    {
      New_Bac_block.KmI_4[i] = Bac_block.KmI_4[i] * RealData.i[i] / Bda_block.IUefNat_filt[i+3];
      //New_Bac_block.KmI_4[i] = Bac_block.KmI_1[i] * RealData.i[i] / Bda_Block.IUefNat_filt[i];
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_6()
{
        C85->Bci_block.ITT2nom = I5;
    if (Commands::WriteFile(&C85->Bci_block, CM_CONFIGFILE, S2ConfigForTune) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    WaitNSeconds(2);
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_7()
{
    return ShowRetomDialog(V60, I5);
}

int TuneDialog85::Start7_3_7_8()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    int res = StartCheckAnalogValues(V60, I5, S0, false);
    if (res == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    GED_Type = TD_GED_D | TD_GED_U;
    return GetExternalData();
}

int TuneDialog85::Start7_3_7_10()
{
    for (int i=0; i<3; ++i)
    {
       New_Bac_block.KmI_1[i] = Bac_block.KmI_1[i] * RealData.i[i] / Bda_block.IUefNat_filt[i+3];
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_8_1()
{
    FillNewBac();
    // 1. Отправляем настроечные параметры в модуль
    return Error::ER_NOERROR;//Commands::WriteBac(BT_BASE, &New_Bac_block, sizeof(Bac));
}

int TuneDialog85::Start7_3_8_2()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I5, S0, true);
}

int TuneDialog85::Start7_3_9()
{
    if (EMessageBox::question(this,"Закончить?","Закончить настройку и записать коэффициенты в модуль?"))
    {
        if (!LoadWorkConfig())
            return Error::ER_GENERALERROR;
        // Пишем в модуль посчитанные регулировочные коэффициенты
        WaitNSeconds(5);
        if (Commands::WriteBac(BT_NONE, &New_Bac_block.K_freq, sizeof(Bac)) != Error::ER_NOERROR)  // Григорий Матвеевич попросил писать коэффициенты сразу в модуль
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
   /* if (EMessageBox::question(this,"Вопрос","Очистить память осциллограмм?"))
    {
        StdFunc::SetPrbMessage("Стёрто записей: ");
        if (Commands::EraseTechBlock(TECH_Bo) == Error::ER_NOERROR)
            EMessageBox::information(this, "Внимание", "Стёрто успешно");
        else
            ERMSG("Ошибка при стирании");
    }
    return true;*/
}

#endif

void TuneDialog85::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_block.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(Bac_block.KmI_1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_block.KmI_4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(Bac_block.KmU2[i], 'f', 5));
    }
    WDFunc::SetLEData(this, "tune13", QString::number(Bac_block.K_freq, 'f', 5));

}

void TuneDialog85::FillNewBac()
{
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(New_Bac_block.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(New_Bac_block.KmI_1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(New_Bac_block.KmI_4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(New_Bac_block.KmU2[i], 'f', 5));
    }
    WDFunc::SetLEData(this, "tune13", QString::number(New_Bac_block.K_freq, 'f', 5));

}

void TuneDialog85::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum);
    QString tmps;
    for (int i = 0; i < 3; i++)
    {
        WDFunc::LEData(this, "tune"+QString::number(i), tmps);
        Bac_block.KmU[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+3), tmps);
        Bac_block.KmI_1[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+6), tmps);
        Bac_block.KmI_4[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+9), tmps);
        Bac_block.KmU2[i]=ToFloat(tmps);
    }
    WDFunc::LEData(this, "tune13", tmps);
    Bac_block.K_freq=ToFloat(tmps);
}

void TuneDialog85::SetDefCoefs()
{
    Bac_block.K_freq = 0.99999999999999999;
    for (int i=0; i<3; i++)
    {
        Bac_block.KmI_1[i] = 1.0;
        Bac_block.KmI_4[i] = 1.0;
    }
    Bac_block.KmU[0] = static_cast<float>(0.978);
    Bac_block.KmU[1] = static_cast<float>(0.977);
    Bac_block.KmU[2] = static_cast<float>(0.977);

    Bac_block.KmU2[0] = static_cast<float>(0.97507);
    Bac_block.KmU2[1] = static_cast<float>(0.97507);
    Bac_block.KmU2[2] = static_cast<float>(0.97507);



    FillBac(BoardTypes::BT_BASE);
}

void TuneDialog85::PrepareConsts()
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
        MVTC.i[0] = C85->Bci_block.Inom;
        MVTC.i[1] = C85->Bci_block.Inom;
        MVTC.i[2] = C85->Bci_block.Inom;
        MTTC.i = TH005;
    }
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialog85::CancelTune()
{
    Cancelled = true;
}
#endif

void TuneDialog85::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

float TuneDialog85::ToFloat(QString text)
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
int TuneDialog85::SaveWorkConfig()
{
    if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == Error::ER_NOERROR)
        memcpy(&Bci_block_work,&C85->Bci_block,sizeof(Config85::Bci));
    else
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog85::StartCheckAnalogValues(double u, double i, double deg, bool tol)
{
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

    res = CheckAnalogValues(u, u, S0, S0, S0, S0, S0, utol, itol, ptol, dtol, ctol);


    return res;
}

#endif

int TuneDialog85::GetExternalData()
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
                spb->setValue(C85->Bci_block.ITT2nom);
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
            RealData.i[i] = C85->Bci_block.ITT2nom;
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

int TuneDialog85::CheckTuneCoefs()
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

bool TuneDialog85::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

int TuneDialog85::CheckMip()
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

QWidget *TuneDialog85::Bd1W(QWidget *parent)
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
        glyout->addWidget(WDFunc::NewLBL(parent, "IUNF_1GR"+IndexStr),0,(i-1),1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i), ValuesFormat, \
                                          QString::number(i)+"IUNF_1GR"+IndexStr+".Истинные действующие значения сигналов 1-й группы"),1,(i-1),1,1);
    }

    for (i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "UNF_2GR"+IndexStr),2,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+7), ValuesFormat, \
                                          QString::number(i+7)+"UNF_2GR"+IndexStr+".Действующие значения сигналов напряжений 2-й группы"),3,i,1,1);
    }

    for (i = 0; i < 6; ++i)
    {

        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "UNF_LIN"+IndexStr),4,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+10), ValuesFormat, \
                                          QString::number(i+10)+"UNF_LIN"+IndexStr+".Истинные действующие значения линейных напряжений 1-й и 2-й групп"),5,i,1,1);
    }

    for (i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "PNF"+IndexStr),6,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+16), ValuesFormat, \
                                          QString::number(i+16)+".Истинная активная мощность"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SNF"+IndexStr),6,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+19), ValuesFormat, \
                                          QString::number(i+19)+".Кажущаяся полная мощность"),7,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QNF"+IndexStr),8,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+22), ValuesFormat, \
                                          QString::number(i+22)+".Реактивная мощность"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Cos"+IndexStr),8,i+3,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+25), ValuesFormat, \
                                          QString::number(i+25)+".Cos phi по истинной активной мощности"),9,i+3,1,1);
    }

    /*for (i = 0; i < 14; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "DD_in"+IndexStr),10,i,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "value"+QString::number(i+27), ValuesFormat, \
                                          QString::number(i+27)+".Дискреты"),11,i,1,1);
    }*/
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

#if PROGSIZE != PROGSIZE_EMUL
int TuneDialog85::CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct)
{
    double ValuesToCheck[30] = {/*TD_TMK,TD_VBAT,*/TD_FREQ,u,u,u,i,i,i,u,u,u,p,p,p,s,s,s,q,q,q, \
                                p,p,p,q,q,q,s,s,s};
    double ThresholdsToCheck[30] = {/*T25,TH05,*/TH0005,utol,utol,utol,itol,itol,itol,utol,utol,utol,pt,pt,pt,pt,pt,pt,pt,pt,pt,\
                                    pt,pt,pt,pt,pt,pt,pt,pt,pt};
    double *VTC = ValuesToCheck;
    double *TTC = ThresholdsToCheck;
    QLocale german(QLocale::German);

    FillBd1(this);

    /*for (int i = 0; i < 28; i++)
    {
        QString tmps;
        WDFunc::LBLText(this, "value"+QString::number(i), tmps);
        bool ok;
        double tmpd = german.toDouble(tmps, &ok);
        if (!ok)
            return Error::ER_GENERALERROR;

        if (!IsWithinLimits(tmpd,*VTC,*TTC))
        {
            EMessageBox::information(this, "Внимание", "Несовпадение по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+ " +/- " + QString::number(*TTC,'f',4));
            return Error::ER_GENERALERROR;
        }
        ++VTC;
        ++TTC;
    }*/


    return Error::ER_NOERROR;
}

int TuneDialog85::SetNewTuneCoefs()
{
    New_Bac_block.K_freq = Bac_block.K_freq;
    for (int i=0; i<3; i++)
    {
        New_Bac_block.KmU[i] = Bac_block.KmU[i];
        New_Bac_block.KmI_1[i] = Bac_block.KmI_1[i];
        New_Bac_block.KmI_4[i] = Bac_block.KmI_4[i];
        New_Bac_block.KmU2[i] = Bac_block.KmU2[i];
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
     if (Commands::GetBda(BT_NONE, &Bda_block, sizeof(Bda_block)) != Error::ER_NOERROR)
     {
         EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
         return Error::ER_GENERALERROR;
     }
    return Error::ER_NOERROR;
}

#endif

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialog85::StartMip()
{
    mipcanal = new iec104;
    connect(mipcanal,SIGNAL(signalsready(Parse104::Signals104&)),this,SLOT(ParseMipData(Parse104::Signals104&)));
    connect(this,SIGNAL(stopall()),mipcanal,SLOT(Stop()));
}

void TuneDialog85::ParseMipData(Parse104::Signals104 &Signal)
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

void TuneDialog85::SetTuneMode()
{
    TuneControlType = sender()->objectName().toInt();
}

void TuneDialog85::StopMip()
{
    emit stopall();
}

int TuneDialog85::ShowControlChooseDialog()
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

int TuneDialog85::Show3PhaseScheme()
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

int TuneDialog85::SaveUeff()
{
/*    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i=0; i<6; i++)
        IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i]; */
    return Error::ER_NOERROR;
}

int TuneDialog85::ShowRetomDialog(double U, double I)
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

int TuneDialog85::ShowRetomDialog2(double U)
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
}

int TuneDialog85::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&C85->Bci_block,&Bci_block_work,sizeof(Config85::Bci));
    if (Commands::WriteFile(&C85->Bci_block, CM_CONFIGFILE, S2ConfigForTune) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

void TuneDialog85::SetExtData()
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

void TuneDialog85::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == nullptr)
        return;
    Cancelled = true;
    dlg->close();
}


void TuneDialog85::GetBdAndFillMTT()
{


}

void TuneDialog85::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {

    case C85_BDA_IN: // Блок #1
        FillBd1(this);
        break;

    default:
        return;
    }
}


void TuneDialog85::FillBd1(QWidget *parent)
{
    //WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bd_block0.Tmk));
    //WDFunc::SetLBLText(parent, "value1", WDFunc::StringValueWithCheck(Bd_block0.Vbat));
    WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bda_block.Frequency, 3));
    for (int i = 1; i < 7; i++)
    {
        int Precision = 4;
        WDFunc::SetLBLText(parent, "value"+QString::number(i), WDFunc::StringValueWithCheck(Bda_block.IUefNat_filt[i-1], Precision));
    }

    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i+7), WDFunc::StringValueWithCheck(Bda_block.IUefNat_filt[i+7], 4));
    }

    for (int i = 0; i < 6; ++i)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i+10), WDFunc::StringValueWithCheck(Bda_block.UefNatLin_filt[i], 4));

    }

    for (int i=0; i<3; i++)
    {
        WDFunc::SetLBLText(parent, "value"+QString::number(i+16), WDFunc::StringValueWithCheck(Bda_block.PNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+19), WDFunc::StringValueWithCheck(Bda_block.SNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+22), WDFunc::StringValueWithCheck(Bda_block.QNatf[i], 3));
        WDFunc::SetLBLText(parent, "value"+QString::number(i+25), WDFunc::StringValueWithCheck(Bda_block.CosPhiNat[i], 4));
    }


}

void TuneDialog85::GenerateReport()
{
    // данные в таблицу уже получены или из файла, или в процессе работы
    // отобразим таблицу
   // ShowTable();
   // QString GOST = (PovType == GOST_1983) ? "1983" : "23625";
    report = new LimeReport::ReportEngine(this);
    QString path = StdFunc::GetSystemHomeDir()+"85report.lrxml";
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

    for(int i=0; i<22; i++)
    {
        if(i==0)
        {
            if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == Error::ER_NOERROR)
            {
               WaitNSeconds(1);
               Start7_3_7_2();  // Переход на конфигурацию 1А
            }

            EMessageBox::question(this,"Сообщение","Подключите напряженческие выходы РЕТОМа ко входам ПЕРВОЙ тройки напряжений модуля!");
        }

        if(i==6)
        {
            if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == Error::ER_NOERROR)
            {
               WaitNSeconds(1);
               Start7_3_7_6();  // Переход на конфигурацию 5А
            }
        }

        if(i==17)
        {
            EMessageBox::question(this,"Сообщение","Подключите напряженческие выходы РЕТОМа ко входам ВТОРОЙ тройки напряжений модуля!");

        }

            QDialog *dlg = new QDialog;
            QVBoxLayout *lyout = new QVBoxLayout;
            QLabel *lbl = new QLabel;
            lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "\
                           "нагрузки по всем фазам 0 град. и частотой 51 Гц;");
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

        /*if(PhiLoad[i] >= 180)
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
        ReportHeader.PhiUBC   = QString::number((360 - Bda_block.phi_next_f[2] + Bda_block.phi_next_f[1]), 'f', 3);*/
        ReportHeader.OffsetF  = QString::number(100*((Bda_block.Frequency/RealData.f[0])-1), 'f', 3);

        if(i < 17)
        {
            ReportHeader.OffsetUA = QString::number(100*((Bda_block.IUefNat_filt[0]/RealData.u[0])-1), 'f', 3);
            ReportHeader.OffsetUB = QString::number(100*((Bda_block.IUefNat_filt[1]/RealData.u[1])-1), 'f', 3);
            ReportHeader.OffsetUC = QString::number(100*((Bda_block.IUefNat_filt[2]/RealData.u[2])-1), 'f', 3);
        }
        else
        {
            ReportHeader.OffsetUA = QString::number(100*((Bda_block.IUefNat_filt[6]/RealData.u[0])-1), 'f', 3);
            ReportHeader.OffsetUB = QString::number(100*((Bda_block.IUefNat_filt[7]/RealData.u[1])-1), 'f', 3);
            ReportHeader.OffsetUC = QString::number(100*((Bda_block.IUefNat_filt[8]/RealData.u[2])-1), 'f', 3);
        }
        ReportHeader.OffsetIA = QString::number(100*((Bda_block.IUefNat_filt[3]/RealData.i[0])-1), 'f', 3);
        ReportHeader.OffsetIB = QString::number(100*((Bda_block.IUefNat_filt[4]/RealData.i[1])-1), 'f', 3);
        ReportHeader.OffsetIC = QString::number(100*((Bda_block.IUefNat_filt[5]/RealData.i[2])-1), 'f', 3);

        /*// Играемся с углами, чтобы все было в одних значениях и с одинаковыми знаками
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
*/
        report->dataManager()->setReportVariable("FreqMIP", QString::number(RealData.f[0], 'f', 3));
        report->dataManager()->setReportVariable("UA_MIP."+QString::number(i), QString::number(RealData.u[0], 'f', 3));
        report->dataManager()->setReportVariable("UB_MIP."+QString::number(i), QString::number(RealData.u[1], 'f', 3));
        report->dataManager()->setReportVariable("UC_MIP."+QString::number(i), QString::number(RealData.u[2], 'f', 3));
        report->dataManager()->setReportVariable("IA_MIP."+QString::number(i), QString::number(RealData.i[0], 'f', 3));
        report->dataManager()->setReportVariable("IB_MIP."+QString::number(i), QString::number(RealData.i[1], 'f', 3));
        report->dataManager()->setReportVariable("IC_MIP."+QString::number(i), QString::number(RealData.i[2], 'f', 3));
        report->dataManager()->setReportVariable("Freq."+QString::number(i), QString::number(Bda_block.Frequency, 'f', 3));
        if(i < 17)
        {
            report->dataManager()->setReportVariable("UA."+QString::number(i), QString::number(Bda_block.IUefNat_filt[0], 'f', 3));
            report->dataManager()->setReportVariable("UB."+QString::number(i), QString::number(Bda_block.IUefNat_filt[1], 'f', 3));
            report->dataManager()->setReportVariable("UC."+QString::number(i), QString::number(Bda_block.IUefNat_filt[2], 'f', 3));
        }
        else
        {
            report->dataManager()->setReportVariable("UA."+QString::number(i), QString::number(Bda_block.IUefNat_filt[6], 'f', 3));
            report->dataManager()->setReportVariable("UB."+QString::number(i), QString::number(Bda_block.IUefNat_filt[7], 'f', 3));
            report->dataManager()->setReportVariable("UC."+QString::number(i), QString::number(Bda_block.IUefNat_filt[8], 'f', 3));
        }
        report->dataManager()->setReportVariable("IA."+QString::number(i), QString::number(Bda_block.IUefNat_filt[3], 'f', 3));
        report->dataManager()->setReportVariable("IB."+QString::number(i), QString::number(Bda_block.IUefNat_filt[4], 'f', 3));
        report->dataManager()->setReportVariable("IC."+QString::number(i), QString::number(Bda_block.IUefNat_filt[5], 'f', 3));
        report->dataManager()->setReportVariable("OffsetF."+QString::number(i), ReportHeader.OffsetF);
        report->dataManager()->setReportVariable("OffsetUA."+QString::number(i), ReportHeader.OffsetUA);
        report->dataManager()->setReportVariable("OffsetUB."+QString::number(i), ReportHeader.OffsetUB);
        report->dataManager()->setReportVariable("OffsetUC."+QString::number(i), ReportHeader.OffsetUC);
        report->dataManager()->setReportVariable("OffsetIA."+QString::number(i), ReportHeader.OffsetIA);
        report->dataManager()->setReportVariable("OffsetIB."+QString::number(i), ReportHeader.OffsetIB);
        report->dataManager()->setReportVariable("OffsetIC."+QString::number(i), ReportHeader.OffsetIC);

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
