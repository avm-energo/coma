#include <QTime>
#include <QtMath>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QCoreApplication>
#include "tunedialog80.h"
#include "../publicclass.h"
#include "../canal.h"
#include "../config/config80.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

TuneDialog80::TuneDialog80(QWidget *parent) :
    QDialog(parent)
{
    C80 = new Config80(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
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
    QWidget *cp1 = new QWidget;
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
    gb = new QGroupBox("Измеряемые параметры");
    QGroupBox *gb2 = MipPars(1, "Частота");
    vlyout->addWidget(gb2);
    gb2 = MipPars(4, "Фазное напряжение");
    vlyout->addWidget(gb2);
    gb2 = MipPars(7, "Фазный ток");
    vlyout->addWidget(gb2);
    gb2 = MipPars(11, "Угол нагрузки");
    vlyout->addWidget(gb2);
    gb2 = MipPars(14, "Фазовый угол напряжения");
    vlyout->addWidget(gb2);
    QHBoxLayout *hlyout = new QHBoxLayout;
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
    vlyout->addWidget(gb);
    gb = new QGroupBox("Вычисляемые параметры");
    gb2 = MipPars(22, "Активная мощность");
    QVBoxLayout *gblyout = new QVBoxLayout;
    gblyout->addWidget(gb2);
    gb2 = MipPars(26, "Реактивная мощность");
    gblyout->addWidget(gb2);
    gb2 = MipPars(30, "Полная мощность");
    gblyout->addWidget(gb2);
    gb2 = MipPars(19, "Линейное напряжение");
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
    vlyout->addWidget(gb);
    pb = new QPushButton("Запустить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMip()));
    vlyout->addWidget(pb);
    pb = new QPushButton("Остановить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMip()));
    vlyout->addWidget(pb);
    cp3->setLayout(vlyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

void TuneDialog80::StartTune()
{
    WDFunc::SetEnabled(this, "starttune", false);

    // подготовка констант для проверки данных МИПа
    MVTC.u = (pc.ModuleBsi.MTypeM == MTM_83) ? S0 : V60;
    MTTC.u = (pc.ModuleBsi.MTypeM == MTM_83) ? FLT_MAX : T005;
    if (pc.ModuleBsi.MTypeM == MTM_81)
    {
        MVTC.i[0] = MVTC.i[1] = MVTC.i[2] = S0;
        MTTC.i = FLT_MAX;
    }
    else
    {
        MVTC.i[0] = C80->Bci_block.inom2[0];
        MVTC.i[1] = C80->Bci_block.inom2[1];
        MVTC.i[2] = C80->Bci_block.inom2[2];
        MTTC.i = T005;
    }

    DefConfig = pc.ModuleBsi.Hth & HTH_REGPARS; // наличие настроечных коэффициентов в памяти модуля

    int count = 0;
    pf[lbls().at(count++)] = &TuneDialog80::SaveWorkConfig; // 1. Сохранение текущей конфигурации
    pf[lbls().at(count++)] = &TuneDialog80::ShowControlChooseDialog; // 2. Отображение диалога выбора режима контроля показаний
    pf[lbls().at(count++)] = &TuneDialog80::Show3PhaseScheme; // 3. Отображение схемы подключения
    pf[lbls().at(count++)] = &TuneDialog80::Start7_2_3; // 7.2.3. Проверка связи РЕТОМ и МИП
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_1; // 7.3.1. Получение настроечных коэффициентов
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_1_1; // 7.3.1.1. Установка настроечных коэффициентов по умолчанию
    pf[lbls().at(count++)] = &TuneDialog80::SetNewTuneCoefs; // 4. Установка коэффициентов
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_2; // 7.3.2. Получение текущих аналоговых данных
    pf[lbls().at(count++)] = &TuneDialog80::SaveUeff; // 5. Сохранение значений фильтра
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_3; // 7.3.3. расчёт коррекции по фазе
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_4; // 7.3.4. расчёт коррекции по частоте
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_5; // 7.3.5. Отображение ввода трёхфазных значений
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_2; // 7.3.6.1. Получение текущих аналоговых данных
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_6_2; // 7.3.6.2. Расчёт коррекции взаимного влияния каналов
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_7_1; // 7.3.7.1. Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_7_2; // 7.3.7.2. Сохранение конфигурации
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_7_3; // 7.3.7.3. Получение текущих аналоговых данных
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_7_4; // 7.3.7.4. Ввод измеренных значений
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_7_5; // 7.3.7.5. Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_7_6; // 7.3.7.6. Сохранение конфигурации
    pf[lbls().at(count++)] = &TuneDialog80::Start7_3_7_8; // 7.3.7.8. Получение текущих аналоговых данных

    Cancelled = false;
    StopMip(); // останавливаем измерения МИП
    MsgClear(); // очистка экрана с сообщениями
    count = 0;
    for (QHash<QString, int (TuneDialog80::*)()>::iterator it = pf.begin(); it != pf.end(); ++it)
    {
        MsgSetVisible(count);
        int res = (this->*pf[it.key()])();
        if ((res == GENERALERROR) || (Cancelled))
        {
            ErMsgSetVisible(count);
            WDFunc::SetEnabled(this, "starttune", false);
            WARNMSG(it.key());
            LoadWorkConfig();
            return;
        }
        else if (res == ER_RESEMPTY)
            SkMsgSetVisible(count);
        else
            OkMsgSetVisible(count);
        ++count;
    }

/*    int res = true;

    res = Start7_3_8();
    if (!res)
    {
        SetTunePbEnabled(true);
        WARNMSG("");
        LoadWorkConfig();
        return;
    }
    res = Start7_3_9();
    if (res)
    {
        SetTunePbEnabled(true);
        MessageBox2::information(this, "Внимание", "Настройка проведена успешно!");
        OkMsgSetVisible(MSG_END);
    }*/
}

int TuneDialog80::Start7_2_3()
{
    if (TuneControlType == TUNEMIP)
    {
        GED_Type = TD_GED_U; // любой параметр для проверки связи сгодится
        return GetExternalData();
    }
    return NOERROR;
}

int TuneDialog80::Start7_3_1()
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

int TuneDialog80::Start7_3_1_1()
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

int TuneDialog80::Start7_3_2()
{
    ReadAnalogMeasurements();

    int res;
    switch (pc.ModuleBsi.MTypeM)
    {
    case MTM_81: // 2t0n
        res = CheckAnalogValues(I5, I5, S0, S0, S0, S0, S0, T005, T005, T05, T1, T005);
        break;
    case MTM_82:
        res = CheckAnalogValues(V60, I5, V60, S0, V60, S0, S1, T01, T005, T05, T1, T005);
        break;
    case MTM_83: // 0t2n
        res = CheckAnalogValues(V60, V60, S0, S0, S0, S0, S0, T01, T01, T05, T1, T005);
        break;
    default:
        return GENERALERROR;
    }
    return res;
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

int TuneDialog80::SaveUeff()
{
    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i=0; i<6; i++)
        IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i];
}

int TuneDialog80::Start7_3_3()
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

int TuneDialog80::Start7_3_4()
{
    GED_Type = TD_GED_F;
    if (!(GetExternalData() == GENERALERROR))
        Bac_newblock.K_freq = Bac_block.K_freq*RealData.f[0]/Bda_block.Frequency;
    else
        RETURN GENERALERROR;
}

int TuneDialog80::Start7_3_5()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "\
                   "сдвига по фазам: А - 0 град., В - 240 град., С - 120 град. (НЕ МЕНЯЯ ЗНАЧЕНИЯ НАПРЯЖЕНИЯ И ТОКА)");
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

int TuneDialog80::Start7_3_6_2()
{
    double fTmp = Bda_block.IUefNat_filt[0] / IUefNat_filt_old[0] + Bda_block.IUefNat_filt[3] / IUefNat_filt_old[3];
    fTmp /= 2;
    Bac_newblock.Kinter = (fTmp * (1 + 6 * Bac_block.Kinter) - 1) / 6;
}

int TuneDialog80::Start7_3_7_1()
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

int TuneDialog80::Start7_3_7_2()
{
    if (pc.ModuleBsi.MTypeM == MTM_81)
        return ER_RESEMPTY;

   // Установить в конфигурации токи i2nom в 1 А
    for (int i=0; i<6; i++)
        C80->Bci_block.inom2[i] = 1.0;
    // послать новые коэффициенты по току в конфигурацию
    cn->Send(CN_WF, Canal::BT_NONE, &C80->Bci_block, sizeof(Config80::Bci), 2, &S2Config);
    WaitNSeconds(2);
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialog80::Start7_3_7_3()
{
    if (pc.ModuleBsi == MTM_81)
        return ER_RESEMPTY;
    // 2. выдать сообщение об установке 60 В 1 А
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lbl=new QLabel("1. Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "\
                   "сдвига по фазам: А - 0 град., В - 240 град., С - 120 град.\n"\
                   "2. Задайте значение напряжений по фазам А, В, С = 60 В и токи = 1 А");
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    if (Start7_3_2() == GENERALERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialog80::Start7_3_7_4()
{
    if (pc.ModuleBsi == MTM_81)
        return ER_RESEMPTY;
    GED_Type = (pc.ModuleBsi.MTypeM == MTM_82) ? (TD_GED_D | TD_GED_I | TD_GED_U) : TD_GED_I;
    return GetExternalData();
}

int TuneDialog80::Start7_3_7_5()
{
    MsgSetVisible(MSG_7_3_7_5);
    for (int i=0; i<3; i++)
    {
//*        if (pc.ModuleBsi.MType1 == MTE_1T1N)
        {
            Bac_newblock.KmU[i] = Bac_block.KmU[i] * mipd[i] / Bda_block.IUefNat_filt[i];
            Bac_newblock.DPsi[i+3] = Bac_block.DPsi[i+3] + Bac_newblock.DPsi[i] - Bac_block.DPsi[i] \
                    + mipd[i+6] - (180*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        }
        else
            Bac_newblock.KmI_1[i] = Bac_block.KmI_1[i] * mipd[i+3] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_1[i+3] = Bac_block.KmI_1[i+3] * mipd[i+3] / Bda_block.IUefNat_filt[i+3];**
    }
    OkMsgSetVisible(MSG_7_3_7_5);
    // 6. установить в конфигурации токи i2nom в 5 А и перезагрузить модуль
    MsgSetVisible(MSG_7_3_7_6);
    for (int i=0; i<6; i++)
        econf->Bci_block.inom2[i] = 5.0;
    cn->Send(CN_WF, Canal::BT_NONE, &econf->Bci_block, sizeof(config_80::Bci), 2, econf->Config);
    if (cn->result != NOERROR)
    {
        ErMsgSetVisible(MSG_7_3_7_6);
        return false;
    }
    // перейти на новую конфигурацию
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
/*        if (pc.ModuleBsi.MType1 == MTE_2T0N)
            Bac_newblock.KmI_5[i] = Bac_block.KmI_5[i] * mipd[i+3] / Bda_block.IUefNat_filt[i];
        Bac_newblock.KmI_5[i+3] = Bac_block.KmI_5[i+3] * mipd[i+3] / Bda_block.IUefNat_filt[i+3];**
    }
    OkMsgSetVisible(MSG_7_3_7_10); */
    return true;
}

bool TuneDialog80::Start7_3_8()
{
/*    MsgSetVisible(MSG_7_3_8_1);
    // 1. Отправляем настроечные параметры в модуль
    cn->Send(CN_WBac, Canal::BT_NONE, &Bac_newblock, sizeof(Bac));
    if (cn->result != NOERROR)
    {
        ErMsgSetVisible(MSG_7_3_8_1);
        return false;
    }
    // переходим на новую конфигурацию
    // 2. Проверяем измеренные напряжения
    MsgSetVisible(MSG_7_3_8_2);
    WaitNSeconds(15);
    if (!Start7_3_2(MSG_7_3_8_2))
    {
        ErMsgSetVisible(MSG_7_3_8_2);
        return false;
    }
    OkMsgSetVisible(MSG_7_3_8_2);*/
    return true;
}

bool TuneDialog80::Start7_3_9()
{
/*    MsgSetVisible(MSG_7_3_9);
    if (QMessageBox::question(this,"Закончить?","Закончить настройку?",QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == \
            QMessageBox::Yes)
    {
        if (!LoadWorkConfig())
        {
            ErMsgSetVisible(MSG_7_3_9);
            return false;
        }
        // переходим на прежнюю конфигурацию
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
        pc.PrbMessage = "Стёрто записей: ";
        cn->Send(CN_OscEr);
        if (cn->result == NOERROR)
            MessageBox2::information(this, "Внимание", "Стёрто успешно");
        else
            ERMSG("Ошибка при стирании");
    } */
    return true;
}

int TuneDialog80::SaveWorkConfig()
{
    cn->Send(CN_GF,Canal::BT_NONE,NULL,0,1,&S2Config);
    if (cn->result == NOERROR)
        memcpy(&Bci_block_work,&C80->Bci_block,sizeof(Config80::Bci));
    else
        return GENERALERROR;
    return NOERROR;
}

int TuneDialog80::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&C80->Bci_block,&Bci_block_work,sizeof(Config80::Bci));
    cn->Send(CN_WF, Canal::BT_NONE, &C80->Bci_block, sizeof(Config80::Bci), 2, &S2Config);
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

QGroupBox *TuneDialog80::MipPars(int parnum, const QString &groupname)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QGroupBox *gb = new QGroupBox(QString::number(parnum)+"-"+QString::number(i+2)+". "+groupname);
    for (i = 0; i < 3; i++)
    {
        QLabel *lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+parnum));
        lbl->setToolTip("Параметр "+QString::number(i+parnum));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    gb->setLayout(hlyout);
    return gb;
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

int TuneDialog80::GetExternalData()
{
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
                s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i), 0, 200, 0.1, 5);
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
                s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i+3), 0, 6, 0.001, 5);
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
                s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb7371"+QString::number(i+6), -360, 360, 0.01, 3);
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
            s_tqSpinBox *spb = WDFunc::NewSPB(this, "spb73719", 40.0, 60.0, 0.01, 5);
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
            RealData.i[i] = C80->Bci_block.inom2[i];
            RealData.d[i] = 0.0;
            RealData.f[i] = 50.0;
        }
        return NOERROR;
    }
    }
    return GENERALERROR;
}

void TuneDialog80::SetExtData()
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

void TuneDialog80::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == 0)
        return;
    Cancelled = true;
    dlg->close();
}

// CHECKING

int TuneDialog80::CheckTuneCoefs()
{
    double ValuesToCheck[26] = {TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,TU1,\
                               TU0,TU0,TU0,TU0,TU0,TU0,TU1,TU0};
    double ThresholdsToCheck[26] = {T002,T002,T002,T002,T002,T002,T002,T002,T002,T002,T002,T002,\
                                   T002,T002,T002,T002,T002,T002,T1,T1,T1,T1,T1,T1,T002,T0005};
    double *VTC = ValuesToCheck;
    double *TTC = ThresholdsToCheck;
    int res = NOERROR;
    for (int i = 0; i < 26; i++)
    {
        QString tmps;
        WDFunc::LBLText(this, "tune"+QString::number(i), tmps);
        bool ok;
        double tmpd = tmps.toDouble(&ok);
        if (!ok)
            return GENERALERROR;
        if (!IsWithinLimits(tmpd, *VTC, *TTC))
        {
            MessageBox2::information(this, "Внимание", "Настроечные по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+\
                      " +/- "+QString::number(*TTC,'f',4));
            res=GENERALERROR;
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
    double ThresholdsToCheck[10] = {S0,T005,T005,T005,MTTC.u,MTTC.u,MTTC.u,MTTC.i,MTTC.i,MTTC.i};
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
            return GENERALERROR;
        if (!IsWithinLimits(tmpd, *VTC, *TTC))
        {
            MessageBox2::information(this, "Внимание", "Несовпадение МИП по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+\
                      " +/- "+QString::number(*TTC,'f',4));
            return GENERALERROR;
        }
        ++VTC;
        ++TTC;
    }
    return NOERROR;
}

int TuneDialog80::CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct)
{
    double it = (pc.ModuleBsi.MTypeM == MTM_83) ? utol : itol; // 0t2n
    double ut = (pc.ModuleBsi.MTypeM == MTM_81) ? itol : utol; // 2t0n
    double ValuesToCheck[45] = {TD_TMK,TD_VBAT,TD_FREQ,u,u,u,i,i,i,u,u,u,i,i,i,phi,phi,phi,phi,phi,phi,p,p,p,s,s,s,q,q,q, \
                                cosphi,cosphi,cosphi,p,p,p,q,q,q,s,s,s,cosphi,cosphi,cosphi};
    double ThresholdsToCheck[45] = {T25,T05,T0005,ut,ut,ut,it,it,it,ut,ut,ut,it,it,it,pht,pht,pht,pht,pht,pht,pt,pt,pt,pt,pt,pt,pt,pt,pt,\
                                    ct,ct,ct,pt,pt,pt,pt,pt,pt,pt,pt,pt,ct,ct,ct};
    double *VTC = ValuesToCheck;
    double *TTC = ThresholdsToCheck;

    for (int i = 0; i < 45; i++)
    {
        QString tmps;
        WDFunc::LBLText(this, "value"+QString::number(i), tmps);
        bool ok;
        double tmpd = tmps.toDouble(&ok);
        if (!ok)
            return GENERALERROR;

        if (!IsWithinLimits(tmpd,*VTC,*TTC))
        {
            MessageBox2::information(this, "Внимание", "Несовпадение по параметру "+QString::number(i)+". Измерено: "+QString::number(tmpd,'f',4)+\
                      ", должно быть: "+QString::number(*VTC,'f',4)+ " +/- " + QString::number(*TTC,'f',4));
            return GENERALERROR;
        }
        ++VTC;
        ++TTC;
    }

/*    switch (ntest)
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
*/
    return NOERROR;
}

void TuneDialog80::ReadTuneCoefs()
{
    cn->Send(CN_GBac, Canal::BT_NONE, &Bac_block, sizeof(Bac));
    if (cn->result == NOERROR)
        WriteTuneCoefsToGUI();
}

void TuneDialog80::WriteTuneCoefs()
{
    ReadTuneCoefsFromGUI();
    cn->Send(CN_WBac, Canal::BT_NONE, &Bac_block, sizeof(Bac));
    if (cn->result == NOERROR)
        MessageBox2::information(this, "Внимание", "Записано успешно!");
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
    return NOERROR;
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
    WriteTuneCoefsToGUI();
}

void TuneDialog80::WriteTuneCoefsToGUI()
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

void TuneDialog80::ReadTuneCoefsFromGUI()
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

void TuneDialog80::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBd, Canal::BT_NONE, &Bda_block, sizeof(Bda_block));
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме данных");
        return;
    }
}

void TuneDialog80::StartMip()
{
    mipcanal = new iec104;
    connect(mipcanal,SIGNAL(signalsready(Parse104::Signals104&)),this,SLOT(ParseMipData(Parse104::Signals104&)));
    connect(this,SIGNAL(stopall()),mipcanal,SLOT(Stop()));
}

void TuneDialog80::ParseMipData(Parse104::Signals104 &Signal)
{
    // precision
    static int Precisions[34] = {0,4,4,4,3,3,3,4,4,4,4,3,3,3,3,3,3,1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
    // приём из mipcanal::Signal номера сигнала (SigNum) и его значения (SigVal) и его дальнейшая обработка
    quint32 index = Signal.SigNum;
    if (index != -1)
    {
        if ((index >= 11) && (index <= 13))
            MipDat[index] = -MipDat[index]; // у МИП-а знак угла отрицательный
        WDFunc::SetLBLText(this, "mip"+QString::number(index), QString::number(Signal.SigVal, 'f', Precisions[index]));
        MipDat[index] = Signal.SigVal;
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

void TuneDialog80::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

int TuneDialog80::ShowControlChooseDialog()
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
        return GENERALERROR;
    else
        return NOERROR;
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
    switch(pc.ModuleBsi.MTypeM) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case MTM_81: // 2t0n
    {
        pmp.load(":/tune81.png");
        break;
    }
    case MTM_82:
    {
        pmp.load(":/tune82.png");
        break;
    }
    case MTM_83:
    {
        pmp.load(":/tune83.png");
        break;
    }
    default:
        return GENERALERROR;
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
    if (pc.ModuleBsi.MTypeM != MTM_83)
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
    return NOERROR;
}

void TuneDialog80::MsgClear()
{
/*    for (int i=0; i<MSG_COUNT; i++)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    } */
}

void TuneDialog80::MsgSetVisible(int msg, bool Visible)
{
    WDFunc::SetVisible(this, "tunemsg"+QString::number(msg), Visible);
}

void TuneDialog80::OkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/ok.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialog80::ErMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/cross.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialog80::SkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/hr.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialog80::CancelTune()
{
    Cancelled = true;
}

void TuneDialog80::LoadFromFile()
{
    QByteArray ba = pc.LoadFile("Tune files (*.etn)");
    memcpy(&Bac_block,&(ba.data()[0]),sizeof(Bac_block));
    WriteTuneCoefsToGUI();
    MessageBox2::information(this, "Внимание", "Загрузка прошла успешно!");
}

void TuneDialog80::SaveToFile()
{
    ReadTuneCoefsFromGUI();
    int res = pc.SaveFile("Tune files (*.etn)", &Bac_block, sizeof(Bac_block));
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

void TuneDialog80::WaitNSeconds(int Seconds)
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

void TuneDialog80::UpdateNSecondsWidget()
{
    QString tmps = "Подождите " + QString::number(--SecondsToEnd15SecondsInterval) + " с";
    emit SecondsRemaining(tmps);
}
