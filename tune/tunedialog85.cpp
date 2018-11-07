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
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

TuneDialog85::TuneDialog85(QVector<S2::DataRec> &S2Config, QWidget *parent) : EAbstractTuneDialog(parent)
{
    C85 = new Config85(S2Config);
    SetBac(&Bac_block, BoardTypes::BT_BASE, sizeof(Bac_block));
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
    QWidget *cp1 = TuneUI();
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QGridLayout *glyout = new QGridLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Настройка");
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
    hlyout = MipPars(14, "Фазовый угол напряжения");
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
    QPushButton *pb = new QPushButton("Запустить связь с МИП");
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
        ReadTuneCoefs(); // считать их из модуля и показать на экране
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
        FillBac();
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
            FillBac();
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
/*    GED_Type = TD_GED_D;
    GetExternalData();
    Bac_newblock.DPsi[0] = 0;
    int k = (pc.ModuleBsi.MTypeM == MTM_82) ? 3 : 6;
    for (int i=1; i<k; ++i)
        Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - Bda_block.phi_next_f[i];
    if (pc.ModuleBsi.MTypeM == MTM_82)
    {
        for (int i=3; i<6; ++i)
            Bac_newblock.DPsi[i] += RealData.d[i-3];
    } */
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_4()
{
   GED_Type = TD_GED_F;
    if (!(GetExternalData() == Error::ER_GENERALERROR))
        New_Bac_block.K_freq = Bac_block.K_freq*RealData.f[0] / Bda_Block.Frequency;
    else
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_5()
{
    return ShowRetomDialog(60.0, 1.0);
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
        New_Bac_block.KmU[i] = Bac_block.KmU[i] * RealData.u[i] / Bda_Block.IUefNat_filt[i];
        New_Bac_block.KmU2[i] = Bac_block.KmU2[i] * RealData.u[i] / Bda_Block.IUefNat_filt[i+6];
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_2()
{
   // Установить в конфигурации токи i2nom в 1 А
        C85->Bci_block.ITT2nom = I1;
    // послать новые коэффициенты по току в конфигурацию
    if (Commands::WriteFile(&C85->Bci_block, 2, S2Config) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    WaitNSeconds(2);
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_3()
{
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
      New_Bac_block.KmI_1[i] = Bac_block.KmI_1[i] * RealData.i[i] / Bda_Block.IUefNat_filt[i];
      //New_Bac_block.KmI_4[i] = Bac_block.KmI_1[i] * RealData.i[i] / Bda_Block.IUefNat_filt[i];
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_6()
{
        C85->Bci_block.ITT2nom = I4;
    if (Commands::WriteFile(&C85->Bci_block, 2, S2Config) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    WaitNSeconds(2);
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_7_7()
{
    return ShowRetomDialog(V60, I4);
}

int TuneDialog85::Start7_3_7_8()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    int res = StartCheckAnalogValues(V60, I4, S0, false);
    if (res == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    GED_Type = TD_GED_D | TD_GED_U;
    return GetExternalData();
}

int TuneDialog85::Start7_3_7_10()
{
    for (int i=0; i<3; ++i)
    {
       New_Bac_block.KmI_4[i] = Bac_block.KmI_1[i] * RealData.i[i] / Bda_Block.IUefNat_filt[i];
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_8_1()
{
    // 1. Отправляем настроечные параметры в модуль
    return Commands::WriteBac(BT_NONE, &New_Bac_block, sizeof(Bac));
}

int TuneDialog85::Start7_3_8_2()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I4, S0, true);
}

int TuneDialog85::Start7_3_9()
{
    if (EMessageBox::question(this,"Закончить?","Закончить настройку?"))
    {
        if (!LoadWorkConfig())
            return Error::ER_GENERALERROR;
        // переходим на прежнюю конфигурацию
        // измеряем и проверяем
        ShowRetomDialog(V57, C85->Bci_block.ITT2nom); // I = 1.0 or 5 or 4.0 A???
        WaitNSeconds(15);
        ReadAnalogMeasurements();
        return StartCheckAnalogValues(V57, C85->Bci_block.ITT2nom, S0, true);
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


#endif

void TuneDialog85::FillBac()
{
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_block.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(Bac_block.KmI_1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_block.KmI_4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(Bac_block.KmU2[i], 'f', 5));
    }
    WDFunc::SetLEData(this, "tune13", QString::number(Bac_block.K_freq, 'f', 5));

}

void TuneDialog85::FillBackBac()
{
    QString tmps;
    for (int i = 0; i < 3; i++)
    {
        WDFunc::LEData(this, "tune"+QString::number(i), tmps);
        Bac_block.KmU[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+3), tmps);
        Bac_block.KmI_4[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+6), tmps);
        Bac_block.KmI_1[i]=ToFloat(tmps);
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



    FillBac();
}



void TuneDialog85::CancelTune()
{
    Cancelled = true;
}

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
    if (Commands::GetFile(CM_CONFIGFILE,S2Config) == Error::ER_NOERROR)
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
                RealData.f[i] = MipDat[i];
                RealData.u[i] = MipDat[i+3];
                RealData.i[i] = MipDat[i+6];
                RealData.d[i] = MipDat[i+10];
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
    for (int i = 1; i < 10; i++)
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


#if PROGSIZE != PROGSIZE_EMUL
int TuneDialog85::CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct)
{
    double ValuesToCheck[30] = {TD_TMK,TD_VBAT,TD_FREQ,u,u,u,u,u,u,i,i,i,p,p,p,s,s,s,q,q,q, \
                                p,p,p,q,q,q,s,s,s};
    double ThresholdsToCheck[30] = {T25,TH05,TH0005,utol,utol,utol,utol,utol,utol,itol,itol,itol,pt,pt,pt,pt,pt,pt,pt,pt,pt,\
                                    pt,pt,pt,pt,pt,pt,pt,pt,pt};
    double *VTC = ValuesToCheck;
    double *TTC = ThresholdsToCheck;

    for (int i = 0; i < 30; i++)
    {
        QString tmps;
        WDFunc::LBLText(this, "value"+QString::number(i), tmps);
        bool ok;
        double tmpd = tmps.toDouble(&ok);
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
    }


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
     if (Commands::GetBda(BT_NONE, &Bda_Block, sizeof(Bda_Block)) != Error::ER_NOERROR)
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
    static int Precisions[34] = {0,4,4,4,3,3,3,4,4,4,4,3,3,3,3,3,3,1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.SigNum;
    if (index != quint32(-1))
    {
        if ((index >= 11) && (index <= 13))
            MipDat[index] = -MipDat[index]; // у МИП-а знак угла отрицательный
        WDFunc::SetLBLText(this, "mip"+QString::number(index), QString::number(Signal.SigVal, 'f', Precisions[index]));
        MipDat[index] = Signal.SigVal;
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
    if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != MTM_83)
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

int TuneDialog85::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&C85->Bci_block,&Bci_block_work,sizeof(Config85::Bci));
    if (Commands::WriteFile(&C85->Bci_block, CM_CONFIGFILE, S2Config) != Error::ER_NOERROR)
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

#endif
