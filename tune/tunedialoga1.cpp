#include <QTime>
#include <QtMath>
#include <QTabWidget>
#include <QEventLoop>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QInputDialog>
#include <QCoreApplication>
#include "tunedialoga1.h"
#include "../publicclass.h"
#include "../canal.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

TuneDialogA1::TuneDialogA1(QWidget *parent) :
    QDialog(parent)
{
    CA1 = new ConfigA1(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void TuneDialogA1::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

void TuneDialogA1::SetupUI()
{
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QTabWidget *TuneTW = new QTabWidget;
    int i;

    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(UCONFWCLR)+"; font: bold 10px;}";
    tmps = "QWidget {background-color: "+QString(UCONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    QLabel *lbl;

    TuneTW->addTab(cp1,"Настройка");
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные измерений");

    // CP1 - НАСТРОЙКА МОДУЛЯ

    lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать настройку");
    pb->setObjectName("starttune");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    for (int i = 0; i < lbls().size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        lbl=new QLabel(lbls().at(i));
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
    glyout->addWidget(WDFunc::NewLBL(this, "KmU[0]"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune0", "", ValuesLEFormat), 0, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "KmU[1]"), 0, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune1", "", ValuesLEFormat), 0, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "K_freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune2", "", ValuesLEFormat), 1, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "DPhy"), 1, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune3", "", ValuesLEFormat), 1, 4, 1, 2);
    for (i = 0; i < 6; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "U1kDN["+QString::number(i)+"]"),2,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+4), "", ValuesLEFormat),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "U2kDN["+QString::number(i)+"]"),4,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+10), "", ValuesLEFormat),5,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "PhyDN["+QString::number(i)+"]"),6,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+16), "", ValuesLEFormat),7,i,1,1);
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Art"), 8, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune22", "", ValuesLEFormat), 8, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Brt"), 8, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune23", "", ValuesLEFormat), 8, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama1"), 9, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune24", "", ValuesLEFormat), 9, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma1"), 9, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune25", "", ValuesLEFormat), 9, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama2"), 10, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune26", "", ValuesLEFormat), 10, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma2"), 10, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune27", "", ValuesLEFormat), 10, 4, 1, 2);

    pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefCoefs()));
    glyout->addWidget(pb, 11, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    glyout->addWidget(pb, 12, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    glyout->addWidget(pb, 13, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    glyout->addWidget(pb, 14, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    glyout->addWidget(pb, 15, 0, 1, 6);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(1);
    cp2->setLayout(lyout);

    // CP3 - Данные измерений

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Данные измерений без настройки (Bda)");
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1.BdaW(this));
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    gb = new QGroupBox("Напряжения в масштабе входных сигналов (Bda_in)");
    vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1.Bd1W(this));
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    lyout->addStretch(1);
    cp3->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

void TuneDialogA1::StartTune()
{
    WDFunc::SetEnabled(this, "starttune", false);

    DefConfig = pc.ModuleBsi.Hth & HTH_REGPARS; // наличие настроечных коэффициентов в памяти модуля

    int count = 0;

    pf[lbls().at(count++)] = &TuneDialogA1::CheckPassword; // 1. Ввод пароля
    pf[lbls().at(count++)] = &TuneDialogA1::ShowScheme; // 2. Отображение схемы подключения
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_2; // 6.2. Проверка правильности измерения сигналов переменного напряжения
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_1; // 6.3.1. Получение настроечных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_2_1; // 6.3.2.1. КПТ: получение блока данных и усреднение
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_2_2; // 6.3.2.2. КПТ: ввод данных от энергомонитора
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_2_3; // 6.3.2.3. КПТ: расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_1; // 6.3.3.1. КТС: подтверждение установки 80 Ом
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_2; // 6.3.3.2. КТС: получение блока данных
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_3; // 6.3.3.3. КТС: подтверждение установки 120 Ом
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_4; // 6.3.3.4. КТС: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_4; // 6.3.4. КМТ2: подтверждение установки 4 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_5_1; // 6.3.5.1. КМТ2: получение блока данных
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_5_2; // 6.3.5.2. КМТ2: подтверждение установки 20 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_5_3; // 6.3.5.3. КМТ2: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_6; // 6.3.6. КМТ1: подтверждение установки 4 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_7_1; // 6.3.7.1. КМТ1: получение блока данных
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_7_2; // 6.3.7.2. КМТ1: подтверждение установки 20 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_7_3; // 6.3.7.3. КМТ1: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_8; // 6.3.8. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_9; // 6.3.9. Проверка аналоговых данных

    Cancelled = false;
    MsgClear(); // очистка экрана с сообщениями
    count = 0;
    for (QHash<QString, int (TuneDialogA1::*)()>::iterator it = pf.begin(); it != pf.end(); ++it)
    {
        MsgSetVisible(count);
        int res = (this->*pf[it.key()])();
        if ((res == GENERALERROR) || (Cancelled))
        {
            ErMsgSetVisible(count);
            WDFunc::SetEnabled(this, "starttune", false);
            WARNMSG(it.key());
            return;
        }
        else if (res == ER_RESEMPTY)
            SkMsgSetVisible(count);
        else
            OkMsgSetVisible(count);
        ++count;
    }
}

int TuneDialogA1::CheckPassword()
{
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc"));
    connect(dlg,SIGNAL(Cancelled()),this,SLOT(CancelTune()));
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(PasswordCheck(QString)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->setLayout(vlyout);
    dlg->show();
    PasswordLoop.exec();
    if (!ok)
    {
        MessageBox2::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

int TuneDialogA1::Start7_2_3()
{
    if (TuneControlType == TUNEMIP)
    {
        GED_Type = TD_GED_U; // любой параметр для проверки связи сгодится
        return GetExternalData();
    }
    return NOERROR;
}

int TuneDialogA1::Start7_3_1()
{
    if (!DefConfig) // если есть настроечные параметры в памяти модуля
    {
        // получение настроечных коэффициентов от модуля
        cn->Send(CN_GBac, Canal::BT_NONE, &Bac_block, sizeof(Bac_block));
        if (cn->result != NOERROR)
        {
            MessageBox2::information(this, "Внимание", "Ошибка при приёме данных");
            return GENERALERROR;
        }
        // обновление коэффициентов в соответствующих полях на экране
        WriteTuneCoefsToGUI();
        // проверка коэффициентов на правильность в соотв. с п. 7.3.1 "Д2"
        if (CheckTuneCoefs())
            return NOERROR;
        else
            return GENERALERROR;
    }
    else
        return ER_RESEMPTY;
}

int TuneDialogA1::Start7_3_1_1()
{
    if (DefConfig)
    {
        // запись настроечных коэффициентов в модуль
        SetDefCoefs();
        cn->Send(CN_WBac, Canal::BT_NONE, &Bac_block, sizeof(Bac));
        if (cn->result == NOERROR)
        {
            // получение настроечных коэффициентов от модуля
            cn->Send(CN_GBac, Canal::BT_NONE, &Bac_block, sizeof(Bac));
            if (cn->result != NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return false;
            }
            // обновление коэффициентов в соответствующих полях на экране
            WriteTuneCoefsToGUI();
            return NOERROR;
        }
        else
            return GENERALERROR;
    }
    else
        return ER_RESEMPTY;
}

int TuneDialogA1::Start7_3_2()
{
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I1, S0, false);
}

/*    if (num == MSG_7_3_2) maxval=602; // 3~7.3.2, 6~7.3.6.1, 12~7.3.7.3
/*    else if (num == MSG_7_3_7_8)
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

int TuneDialogA1::Start7_3_3()
{
    GED_Type = TD_GED_D;
    GetExternalData();
    Bac_newblock.DPsi[0] = 0;
    int k = (pc.ModuleBsi.MTypeM == MTM_82) ? 3 : 6;
    for (int i=1; i<k; ++i)
        Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - Bda_block.phi_next_f[i];
    if (pc.ModuleBsi.MTypeM == MTM_82)
    {
        for (int i=3; i<6; ++i)
            Bac_newblock.DPsi[i] += RealData.d[i-3];
    }
    return NOERROR;
}

int TuneDialogA1::Start7_3_4()
{
    GED_Type = TD_GED_F;
    if (!(GetExternalData() == GENERALERROR))
        Bac_newblock.K_freq = Bac_block.K_freq*RealData.f[0]/Bda_block.Frequency;
    else
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start7_3_5()
{
    return ShowRetomDialog(60.0, 1.0);
}

int TuneDialogA1::Start7_3_6_2()
{
    double fTmp = Bda_block.IUefNat_filt[0] / IUefNat_filt_old[0] + Bda_block.IUefNat_filt[3] / IUefNat_filt_old[3];
    fTmp /= 2;
    Bac_newblock.Kinter = (fTmp * (1 + 6 * Bac_block.Kinter) - 1) / 6;
    return NOERROR;
}

int TuneDialogA1::Start7_3_7_1()
{
    if (pc.ModuleBsi.MTypeM != MTM_81)
        return ER_RESEMPTY;
    GED_Type = TD_GED_U;
    if (GetExternalData() == GENERALERROR)
        return GENERALERROR;
    if (Start7_3_2() == GENERALERROR)
        return GENERALERROR;
    for (int i=0; i<3; i++)
    {
        Bac_newblock.KmU[i] = Bac_block.KmU[i] * RealData.u[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmU[i+3] = Bac_block.KmU[i+3] * RealData.u[i] / Bda_block.IUefNat_filt[i+3];
    }
    return NOERROR;
}

int TuneDialogA1::Start7_3_7_2()
{
    if (pc.ModuleBsi.MTypeM == MTM_81)
        return ER_RESEMPTY;

   // Установить в конфигурации токи i2nom в 1 А
    for (int i=0; i<6; i++)
        C80->Bci_block.inom2[i] = I1;
    // послать новые коэффициенты по току в конфигурацию
    cn->Send(CN_WF, Canal::BT_NONE, &C80->Bci_block, sizeof(Config80::Bci), 2, &S2Config);
    WaitNSeconds(2);
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start7_3_7_3()
{
    if (pc.ModuleBsi.MTypeM == MTM_81)
        return ER_RESEMPTY;
    ShowRetomDialog(V60, I1);
    if (Start7_3_2() == GENERALERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start7_3_7_4()
{
    if (pc.ModuleBsi.MTypeM == MTM_81)
        return ER_RESEMPTY;
    GED_Type = (pc.ModuleBsi.MTypeM == MTM_82) ? (TD_GED_I | TD_GED_U) : TD_GED_I;
    return GetExternalData();
}

int TuneDialogA1::Start7_3_7_5()
{
    if (pc.ModuleBsi.MTypeM == MTM_81)
        return ER_RESEMPTY;
    for (int i=0; i<3; ++i)
    {
        if (pc.ModuleBsi.MTypeM == MTM_82)
            Bac_newblock.KmU[i] = Bac_block.KmU[i] * RealData.u[i] / Bda_block.IUefNat_filt[i];
        else
            Bac_newblock.KmI_1[i] = Bac_block.KmI_1[i] * RealData.i[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_1[i+3] = Bac_block.KmI_1[i+3] * RealData.i[i] / Bda_block.IUefNat_filt[i+3];
    }
    return NOERROR;
}

int TuneDialogA1::Start7_3_7_6()
{
    if (pc.ModuleBsi.MTypeM == MTM_81)
        return ER_RESEMPTY;
    for (int i=0; i<6; ++i)
        C80->Bci_block.inom2[i] = I5;
    cn->Send(CN_WF, Canal::BT_NONE, &C80->Bci_block, sizeof(Config80::Bci), 2, &S2Config);
    WaitNSeconds(2);
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start7_3_7_7()
{
    return ShowRetomDialog(V60, I5);
}

int TuneDialogA1::Start7_3_7_8()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    int res = StartCheckAnalogValues(V60, I5, S0, false);
    if (res == GENERALERROR)
        return GENERALERROR;
    GED_Type = TD_GED_D | TD_GED_U;
    return GetExternalData();
}

int TuneDialogA1::Start7_3_7_10()
{
    for (int i=0; i<3; ++i)
    {
        if (pc.ModuleBsi.MTypeM == MTM_81)
            Bac_newblock.KmI_5[i] = Bac_block.KmI_5[i] * RealData.i[i] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_5[i+3] = Bac_block.KmI_5[i+3] * RealData.i[i] / Bda_block.IUefNat_filt[i+3];
    }
    return NOERROR;
}

int TuneDialogA1::Start7_3_8_1()
{
    // 1. Отправляем настроечные параметры в модуль
    cn->Send(CN_WBac, Canal::BT_NONE, &Bac_newblock, sizeof(Bac));
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start7_3_8_2()
{
    WaitNSeconds(15);
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I5, S0, true);
}

int TuneDialogA1::Start7_3_9()
{
    if (MessageBox2::question(this,"Закончить?","Закончить настройку?"))
    {
        if (!LoadWorkConfig())
            return GENERALERROR;
        // переходим на прежнюю конфигурацию
        // измеряем и проверяем
        ShowRetomDialog(V57, C80->Bci_block.inom2[0]); // I = 1.0 or 5.0 A
        WaitNSeconds(15);
        ReadAnalogMeasurements();
        return StartCheckAnalogValues(V57, C80->Bci_block.inom2[0], S0, true);
    }
    else
        return false;
    if (MessageBox2::question(this,"Вопрос","Очистить память осциллограмм?"))
    {
        pc.PrbMessage = "Стёрто записей: ";
        cn->Send(CN_OscEr);
        if (cn->result == NOERROR)
            MessageBox2::information(this, "Внимание", "Стёрто успешно");
        else
            ERMSG("Ошибка при стирании");
    }
    return true;
}

int TuneDialogA1::GetExternalData()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("dlg7371");
    QGridLayout *glyout = new QGridLayout;
    QLabel *lbl = new QLabel("Введите значения сигналов по приборам");
    glyout->addWidget(lbl,0,0,1,6);
    int row = 1;
    int column = 0;
    for (int i=0; i<3; i++) // for A to C
    {
        lbl = new QLabel("Uф" + QString::number(i+10,36).toUpper()+", В");
        glyout->addWidget(lbl,row,column++,1,1);
        s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i), 0, 200, 0.1, 5);
        spb->setValue(60.0);
        glyout->addWidget(spb,row,column++,1,1);
    }
    ++row;
    column = 0;
    for (int i=0; i<3; i++) // for A to C
    {
        lbl = new QLabel("Iф" + QString::number(i+10,36).toUpper()+", A");
        glyout->addWidget(lbl,row, column++,1,1);
        s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i+3), 0, 6, 0.001, 5);
        spb->setValue(C80->Bci_block.inom2[i]);
        glyout->addWidget(spb,row, column++,1,1);
    }
    ++row;
    column = 0;
    for (int i=0; i<3; i++) // for A to C
    {
        lbl = new QLabel("Уг.нагр. ф" + QString::number(i+10,36).toUpper()+", град");
        glyout->addWidget(lbl,3,i*2,1,1);
        s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i+6), -360, 360, 0.01, 3);
        spb->setValue(0.0);
        glyout->addWidget(spb,row, column++,1,1);
    }
    ++row;
    column = 0;
    lbl = new QLabel("Частота ");
    glyout->addWidget(lbl,row,column++,1,1);
    s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb73719", 40.0, 60.0, 0.01, 5);
    spb->setValue(50.0);
    glyout->addWidget(spb,row, column++,1,1);
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

void TuneDialogA1::PasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit PasswordChecked();
}

void TuneDialogA1::SetExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == 0)
        return;
    for (int i=0; i<3; ++i)
    {
        s_tqSpinBox *spb = this->findChild<s_tqSpinBox *>("spb7371"+QString::number(i));
        if (spb != 0)
            RealData.u[i] = spb->value();
        spb = this->findChild<s_tqSpinBox *>("spb7371"+QString::number(i+3));
        if (spb != 0)
            RealData.i[i] = spb->value();
        spb = this->findChild<s_tqSpinBox *>("spb7371"+QString::number(i+6));
        if (spb != 0)
            RealData.d[i] = spb->value();
    }
    s_tqSpinBox *spb = this->findChild<s_tqSpinBox *>("spb73719");
    if (spb != 0)
        RealData.f[0] = spb->value();
    Cancelled = false;
    dlg->close();
}

void TuneDialogA1::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == 0)
        return;
    Cancelled = true;
    dlg->close();
}

bool TuneDialogA1::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
        return false;
}

void TuneDialogA1::ReadTuneCoefs()
{
    cn->Send(CN_GBac, Canal::BT_NONE, &Bac_block, sizeof(Bac));
    if (cn->result == NOERROR)
        FillBac();
}

void TuneDialogA1::WriteTuneCoefs()
{
    FillBackBac();
    cn->Send(CN_WBac, Canal::BT_NONE, &Bac_block, sizeof(Bac));
    if (cn->result == NOERROR)
        MessageBox2::information(this, "Внимание", "Записано успешно!");
}

void TuneDialogA1::SetDefCoefs()
{
    Bac_block = {0.974, 0.98307, 1.0, 0.00919, 12.0, 24.0, 36.0, 48.0, 60.0, 71.0, 12.0, 24.0, 36.0, 48.0, 60.0, 71.0, \
                 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 82.0875, 6023.3, 163.839, -0.4125, 163.6494, -0.8425};
    FillBac();
}

int TuneDialogA1::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBd, Canal::BT_NONE, &ChA1.Bda_block, sizeof(ChA1.Bda));
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bda");
        return GENERALERROR;
    }
    ChA1.FillBda(this);
    return NOERROR;
}

void TuneDialogA1::FillBac()
{
    WDFunc::SetLEData(this, "tune0", QString::number(Bac_block.KmU[0], 'f', 5));
    WDFunc::SetLEData(this, "tune1", QString::number(Bac_block.KmU[1], 'f', 5));
    WDFunc::SetLEData(this, "tune2", QString::number(Bac_block.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune3", QString::number(Bac_block.DPhy, 'f', 5));
    for (i = 0; i < 6; ++i)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i+4), QString::number(Bac_block.U1kDN[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+10), QString::number(Bac_block.U2kDN[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+16), QString::number(Bac_block.PhyDN[i], 'f', 5));
    }
    WDFunc::SetLEData(this, "tune22", QString::number(Bac_block.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune23", QString::number(Bac_block.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune24", QString::number(Bac_block.Ama1, 'f', 5));
    WDFunc::SetLEData(this, "tune25", QString::number(Bac_block.Bma1, 'f', 5));
    WDFunc::SetLEData(this, "tune26", QString::number(Bac_block.Ama2, 'f', 5));
    WDFunc::SetLEData(this, "tune27", QString::number(Bac_block.Bma2, 'f', 5));
}

void TuneDialogA1::FillBackBac()
{
    WDFunc::LEData(this, "tune0", Bac_block.KmU[0]);
    WDFunc::LEData(this, "tune1", Bac_block.KmU[1]);
    WDFunc::LEData(this, "tune2", Bac_block.K_freq);
    WDFunc::LEData(this, "tune3", Bac_block.DPhy);
    for (int i = 0; i < 6; i++)
    {
        WDFunc::LEData(this, "tune"+QString::number(i+4), Bac_block.U1kDN[i]);
        WDFunc::LEData(this, "tune"+QString::number(i+10), Bac_block.U2kDN[i]);
        WDFunc::LEData(this, "tune"+QString::number(i+16), Bac_block.PhyDN[i]);
    }
    WDFunc::LEData(this, "tune22", Bac_block.Art);
    WDFunc::LEData(this, "tune23", Bac_block.Brt);
    WDFunc::LEData(this, "tune24", Bac_block.Ama1);
    WDFunc::LEData(this, "tune25", Bac_block.Bma1);
    WDFunc::LEData(this, "tune26", Bac_block.Ama2);
    WDFunc::LEData(this, "tune27", Bac_block.Bma2);
}

int TuneDialogA1::ShowScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap(":/../pic/tunea1.png")));
    lbl = new QLabel();
    lyout->addWidget(WDFunc::NewLBL(this, "1. На выходах РЕТОМ задайте частоту 50,0 Гц, уровень напряжения фазы А 60 В с фазой 0 градусов, включите режим однофазного выхода;"));
    lyout->addWidget(WDFunc::NewLBL(this, "2. Включите питание прибора Энергомонитор-3.1КМ и настройте его на режим измерения напряжений в диапазоне 0...100 В и частоты;"));
    lyout->addWidget(WDFunc::NewLBL(this, "3. На магазине сопротивлений установите значение сопротивления 100,0 Ом;"));
    lyout->addWidget(WDFunc::NewLBL(this, "4. Включите калибратор токовой петли и установите выходной ток 20 мА;"));
    lyout->addWidget(WDFunc::NewLBL(this, "5. Включите выходы РЕТОМ."));
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    if (Cancelled == true)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start6_2()
{
    WaitNSeconds(10);
    if (ReadAnalogMeasurements() == GENERALERROR)
        return GENERALERROR;
    return CheckAnalogValues();
}

int TuneDialogA1::CheckAnalogValues()
{
    if (!IsWithinLimits(ChA1.Bda_block.Ueff_ADC[0], 3900000.0, 200000.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.Ueff_ADC[1], 3900000.0, 200000.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.Frequency, 50.0, 0.05))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.Pt100, 2125.0, 75.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.EXTmA1, 25.0, 25.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.EXTmA2, 3275.0, 75.0))
        return GENERALERROR;
}

int TuneDialogA1::Start6_3_1()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBac, Canal::BT_NONE, &Bac_block, sizeof(Bac));
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bac");
        return GENERALERROR;
    }
    FillBac();
    return NOERROR;
}

int TuneDialogA1::Start6_3_2_1()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBd, 1, &ChA1.Bda_in, sizeof(ChA1.A1_Bd1));
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bda_in");
        return GENERALERROR;
    }

}

void TuneDialogA1::MsgClear()
{
    for (int i=0; i<lbls().size(); ++i)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    }
}

void TuneDialogA1::MsgSetVisible(int msg, bool Visible)
{
    WDFunc::SetVisible(this, "tunemsg"+QString::number(msg), Visible);
}

void TuneDialogA1::OkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/ok.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialogA1::ErMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/cross.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialogA1::SkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/hr.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialogA1::CancelTune()
{
    Cancelled = true;
}

void TuneDialogA1::LoadFromFile()
{
    QByteArray ba = pc.LoadFile("Tune files (*.tn)");
    memcpy(&Bac_block,&(ba.data()[0]),sizeof(Bac_block));
    WriteTuneCoefsToGUI();
    MessageBox2::information(this, "Внимание", "Загрузка прошла успешно!");
}

void TuneDialogA1::SaveToFile()
{
    ReadTuneCoefsFromGUI();
    int res = pc.SaveFile("Tune files (*.tn)", &Bac_block, sizeof(Bac_block));
    switch (res)
    {
    case NOERROR:
        MessageBox2::information(this, "Внимание", "Записано успешно!");
        break;
    case ER_FILEWRITE:
        MessageBox2::error(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case ER_FILENAMEEMP:
        MessageBox2::error(this, "Ошибка", "Пустое имя файла!");
        break;
    case ER_FILEOPEN:
        MessageBox2::error(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void TuneDialogA1::WaitNSeconds(int Seconds)
{
    QTime tme;
    SecondsToEnd15SecondsInterval = Seconds;
    WaitWidget *w = new WaitWidget;
    QTimer *tmr = new QTimer;
    tmr->setInterval(1000);
    connect(tmr,SIGNAL(timeout()),this,SLOT(UpdateNSecondsWidget()));
    connect(this,SIGNAL(SecondsRemaining(QString)),w,SLOT(SetMessage(QString)));
    tmr->start();
    w->Start();
    tme.start();
    while (SecondsToEnd15SecondsInterval > 0);
    tmr->stop();
    w->Stop();
}

void TuneDialogA1::UpdateNSecondsWidget()
{
    QString tmps = "Подождите " + QString::number(--SecondsToEnd15SecondsInterval) + " с";
    emit SecondsRemaining(tmps);
}
