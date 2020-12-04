#include "tunekivdialog.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"
#include "tunekivcheck.h"
#include "tunekivmain.h"
#include "tunekivtemp60.h"
#include "tunekivtemp_20.h"
//#include <QCoreApplication>
//#include <QFileDialog>
//#include <QGridLayout>
//#include <QGroupBox>
//#include <QInputDialog>
//#include <QLabel>
//#include <QMessageBox>
//#include <QPushButton>
//#include <QScrollArea>
//#include <QScrollBar>
//#include <QTabWidget>
//#include <QThread>
//#include <QTime>
//#include <QVBoxLayout>
//#include <QtMath>
//#include <functional>

TuneKIVDialog::TuneKIVDialog(ConfigKIV *ckiv, TuneKIV *tkiv, QWidget *parent) : UDialog(parent)
{
    TKIV = tkiv;
    CKIV = ckiv;
    // ReportModel = new QStandardItemModel;
    // ViewModel = new QStandardItemModel;
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void TuneKIVDialog::SetupUI()
{
    QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addLayout(newTunePBLayout("1. Проверка правильности измерения входных сигналов", [this]() {
        TuneKIVCheck *check = new TuneKIVCheck(1, TKIV);
        check->show();
    }));
    lyout->addLayout(newTunePBLayout("2. Основная регулировка", [this]() {
        TuneKIVMain *tkmain = new TuneKIVMain(2, CKIV, TKIV);
        tkmain->show();
    }));
    lyout->addLayout(newTunePBLayout("3. Настройка температурной коррекции +60 °С", [this]() {
        TuneKIVTemp60 *tk60 = new TuneKIVTemp60(3, TKIV);
        tk60->show();
    }));
    lyout->addLayout(newTunePBLayout("4. Настройка температурной коррекции -20 °С", [this]() {
        TuneKIVTemp_20 *tk_20 = new TuneKIVTemp_20(4, TKIV);
        tk_20->show();
    }));
    lyout->addLayout(newTunePBLayout("5. Генерация протокола регулировки", [this]() { close(); }));

    /*    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                               "background-color: "
            + QString(Colors::ACONFOCLR) + "; font: bold 10px;}";
        QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                                 "background-color: "
            + QString(Colors::ACONFOCLR) + "; font: bold 10px;}";
        QWidget *cp1 = new QWidget; // = TuneUI();

        QGridLayout *glyout = new QGridLayout;
        QVBoxLayout *lyout = new QVBoxLayout;
        // QHBoxLayout *Hlyout = new QHBoxLayout;
        QPushButton *pb = new QPushButton("Настроить канал Pt100");

        connect(pb, SIGNAL(clicked(bool)), this, SLOT(TunePt100Channel()));

        glyout->addWidget(pb, 1, 1, 1, 1);

        glyout->addWidget(TuneUI(), 3, 1, 1, 1);

        pb = new QPushButton("Настройка температурной коррекции");

        connect(pb, SIGNAL(clicked(bool)), this, SLOT(TuneTemp()));

        glyout->addWidget(pb, 2, 1, 1, 1);

        pb = new QPushButton("Начать поверку");

        connect(pb, SIGNAL(clicked()), this, SLOT(GenerateReport()));

        glyout->addWidget(pb, 15, 1, 1, 1);
        lyout->addLayout(glyout);

        cp1->setLayout(lyout);

        QWidget *cp2 = new QWidget;
        QWidget *cp3 = new QWidget;

        QWidget *cp4 = Bd1W(this);

        tmps = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
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

        TuneTW->addTab(cp1, "Настройка");

        TuneTW->addTab(cp4, "Измеренные параметры");

        // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ
        lyout->addWidget(BottomUI(BoardTypes::BT_MEZONIN));
        lyout->addStretch(1);
        lyout = new QVBoxLayout;
        lyout->addWidget(TuneTW); */
    setLayout(lyout);
}

/*void TuneKIVDialog::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Сохранение текущей конфигурации...");
    lbls.append("7.2.1. Задание максимальных токов 600 мА...");
    lbls.append("7.2.2. Отображение схемы подключения...");
    lbls.append("7.2.7. Получение результатов измерений...");

    lbls.append("5. Установка коэффициентов...");
    lbls.append("7. 7_3_2. Получение текущих аналоговых данных...");
    lbls.append("8. 7.3.4. Информация...");
    lbls.append(
        "9. 7.3.4.2. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=1...");
    lbls.append("10. 7.3.4.6 Переход на новую конфигурацию 400мА...");
    lbls.append(
        "11. 7.3.4.7. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=2...");
    lbls.append("12. 7.3.4.11. Переход на новую конфигурацию 200мА......");
    lbls.append(
        "13. 7.3.4.12. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=4...");
    lbls.append("14. 7.3.4.13. Переход на новую конфигурацию 100мА......");
    lbls.append(
        "15. 7.3.4.14. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=8...");
    lbls.append("16. 7.3.4.15. Переход на новую конфигурацию 50мА......");
    lbls.append(
        "17. 7.3.4.16. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=16...");
    lbls.append("18. 7.3.4.17. Переход на новую конфигурацию 25мА......");
    lbls.append(
        "19. 7.3.4.18. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=32...");
    lbls.append("20. 7.3.5. Восстановление сохранённой конфигурации и проверка...");
}

void TuneKIVDialog::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword;
    int (EAbstractTuneDialog::*func)()
        = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::SaveWorkConfig);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_2_1);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::ShowScheme);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start_7_2_7);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_3_1); // 4. Информация
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::SetNewTuneCoefs); // 5. Установка коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_3_4); // Информация
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_3_4_2); // Расчёт коррекции по фазе
    pf[lbls.at(count++)] = func;
    func
        = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_3_4_6); // Расчёт коррекции по частоте
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_4_7); // Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току
                                       // для Кацп=2
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_4_11); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_4_12); // Расчёт коррекции взаимного влияния каналов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_4_13); // Получение текущих аналоговых данных и расчёт настроечных коэффициентов по
                                        // напряжениям
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_3_4_14); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_4_15); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_3_4_16); // Ввод измеренных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_4_17); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneKIVDialog::Start7_3_4_18); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVDialog::Start7_3_5); // Восстановление сохранённой конфигурации и проверка
    pf[lbls.at(count++)] = func;
}

void TuneKIVDialog::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
}

void TuneKIVDialog::FillNewBac()
{
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune" + QString::number(i), QString::number(m_Bac_newblock.N1_TT[i], 'g', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 3), QString::number(m_Bac_newblock.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 6), QString::number(m_Bac_newblock.KmI1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 9), QString::number(m_Bac_newblock.KmI2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 12), QString::number(m_Bac_newblock.KmI4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 15), QString::number(m_Bac_newblock.KmI8[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 18), QString::number(m_Bac_newblock.KmI16[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 21), QString::number(m_Bac_newblock.KmI32[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 24), QString::number(m_Bac_newblock.TKPsi_a[i], 'e', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 27), QString::number(m_Bac_newblock.TKPsi_b[i], 'e', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune" + QString::number(i + 30), QString::number(m_Bac_newblock.DPsi[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 36), QString::number(m_Bac_newblock.TKUa[i], 'e', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 42), QString::number(m_Bac_newblock.TKUb[i], 'e', 5));
    }

    WDFunc::SetLEData(this, "tune48", QString::number(m_Bac_newblock.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune49", QString::number(m_Bac_newblock.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune50", QString::number(m_Bac_newblock.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune51", QString::number(m_Bac_newblock.Tmk0, 'f', 5));
}

void TuneKIVDialog::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum);
    QString tmps;
}

int TuneKIVDialog::SetNewTuneCoefs()
{
    m_Bac_newblock.Art = m_Bac_block.Art;
    m_Bac_newblock.Brt = m_Bac_block.Brt;
    m_Bac_newblock.Tmk0 = m_Bac_block.Tmk0;
    m_Bac_newblock.K_freq = m_Bac_block.K_freq;

    for (int i = 0; i < 6; i++)
    {
        m_Bac_newblock.DPsi[i] = m_Bac_block.DPsi[i];
        m_Bac_newblock.TKUa[i] = m_Bac_block.TKUa[i];
        m_Bac_newblock.TKUb[i] = m_Bac_block.TKUb[i];
    }

    for (int i = 0; i < 3; i++)
    {
        m_Bac_newblock.KmU[i] = m_Bac_block.KmU[i];
        m_Bac_newblock.KmI1[i] = m_Bac_block.KmI1[i];
        m_Bac_newblock.KmI16[i] = m_Bac_block.KmI16[i];
        m_Bac_newblock.KmI2[i] = m_Bac_block.KmI2[i];
        m_Bac_newblock.KmI32[i] = m_Bac_block.KmI32[i];
        m_Bac_newblock.KmI4[i] = m_Bac_block.KmI4[i];
        m_Bac_newblock.KmI8[i] = m_Bac_block.KmI8[i];
        m_Bac_newblock.N1_TT[i] = m_Bac_block.N1_TT[i];
        m_Bac_newblock.TKPsi_a[i] = m_Bac_block.TKPsi_a[i];
        m_Bac_newblock.TKPsi_b[i] = m_Bac_block.TKPsi_b[i];
    }
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_2()
{
    m_Kadc = 1;
    EnterDataTune();
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_6()
{
    int i;
    for (i = 0; i < 3; i++)
        CKIV->Bci_block.C_pasp[i] = C10024;
    // CKIV->Bci_block.Imax[i] = 400;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
        return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_7()
{
    ShowRetomDialog(57.5, 250, 89.9);
    m_Kadc = 2;
    EnterDataTune();
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_11()
{
    int i;
    for (i = 0; i < 3; i++)
        CKIV->Bci_block.C_pasp[i] = C5012;
    // CKIV->Bci_block.Imax[i] = 200;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
        return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_12()
{
    ShowRetomDialog(57.5, 125, 89.9);
    m_Kadc = 4;
    EnterDataTune();
    return NOERROR;
    ;
}

int TuneKIVDialog::Start7_3_4_13()
{
    int i;
    for (i = 0; i < 3; i++)
        CKIV->Bci_block.C_pasp[i] = C2506;
    // CKIV->Bci_block.Imax[i] = 100;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
        return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_14()
{
    ShowRetomDialog(57.5, 63, 89.9);
    m_Kadc = 8;
    EnterDataTune();
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_15()
{
    int i;
    for (i = 0; i < 3; i++)
        CKIV->Bci_block.C_pasp[i] = C1253;
    // CKIV->Bci_block.Imax[i] = 50;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
        return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_16()
{
    ShowRetomDialog(57.5, 32, 89.9);
    m_Kadc = 16;
    EnterDataTune();
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_17()
{
    int i;
    for (i = 0; i < 3; i++)
        CKIV->Bci_block.C_pasp[i] = C626;
    // CKIV->Bci_block.Imax[i] = 25;

    if (Commands::WriteFile(1, S2ConfigForTune) != NOERROR)
        return GENERALERROR;

    WaitNSeconds(2);
    return NOERROR;
}

int TuneKIVDialog::Start7_3_4_18()
{
    ShowRetomDialog(57.5, 16, 89.9);
    m_Kadc = 32;
    EnterDataTune();
    FillNewBac();
    return NOERROR;
}

int TuneKIVDialog::Start7_3_5()
{
    if (LoadWorkConfig())
        return GENERALERROR;
    WaitNSeconds(5);
    if (QMessageBox::question(this, "Закончить?", "Закончить настройку и записать коэффициенты в модуль?"))
    {

        // Пишем в модуль посчитанные регулировочные коэффициенты

        if (Commands::WriteBac(BT_MEZONIN, &m_Bac_newblock, sizeof(m_Bac_newblock))
            != NOERROR) // Григорий Матвеевич попросил писать коэффициенты сразу в модуль
            return GENERALERROR;

        if (QMessageBox::question(this, "Протокол поверки", "Начать поверку?"))
        {
            GenerateReport();
        }

        return NOERROR;
    }
    else
        return false;
}

void TuneKIVDialog::ReadN()
{
    QString tmps;
    WDFunc::LE_read_data(ask, "N", tmps);
    m_filterSteps = tmps.toInt();
    ask->close();
}

void TuneKIVDialog::EnterDataTune()
{
    int i;
    ask = new QDialog(this);
    // QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl, 0, 0, 1, 6);

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Uэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 1, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU" + QString::number(i));
        glyout->addWidget(ledit, 2, i, 1, 1);
    }

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Iэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 3, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI" + QString::number(i));
        glyout->addWidget(ledit, 4, i, 1, 1);
    }

    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("φэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 5, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi" + QString::number(i));
        glyout->addWidget(ledit, 6, i, 1, 1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl, 7, 0, 1, 1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit, 8, 0, 1, 1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb, SIGNAL(clicked()), this, SLOT(CalcTuneCoefs()));
    glyout->addWidget(pb, 9, 2, 1, 2);
//     pb = new QPushButton("Отмена");
//     connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(close()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
//     glyout->addWidget(pb,9,3,1,3);

    ask->setLayout(glyout);
    ask->exec();

//    lyout->addWidget(pb);
//    dlg->setLayout(lyout);
//    dlg->exec();
}

void TuneKIVDialog::Enter20Data()
{
    int i;
    ask = new QDialog(this);
    // QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl, 0, 0, 1, 6);

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Uэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 1, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU" + QString::number(i));
        glyout->addWidget(ledit, 2, i, 1, 1);
    }

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Iэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 3, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI" + QString::number(i));
        glyout->addWidget(ledit, 4, i, 1, 1);
    }

    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("φэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 5, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi" + QString::number(i));
        glyout->addWidget(ledit, 6, i, 1, 1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl, 7, 0, 1, 1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit, 8, 0, 1, 1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveValuesTemp20()));
    glyout->addWidget(pb, 9, 2, 1, 2);
//     pb = new QPushButton("Отмена");
//     connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(close()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
//     glyout->addWidget(pb,9,3,1,3);

    ask->setLayout(glyout);
    ask->exec();
}

void TuneKIVDialog::Enterminus20Data()
{
    int i;
    ask = new QDialog(this);
    // QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl, 0, 0, 1, 6);

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Uэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 1, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU" + QString::number(i));
        glyout->addWidget(ledit, 2, i, 1, 1);
    }

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Iэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 3, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI" + QString::number(i));
        glyout->addWidget(ledit, 4, i, 1, 1);
    }

    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("φэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 5, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi" + QString::number(i));
        glyout->addWidget(ledit, 6, i, 1, 1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl, 7, 0, 1, 1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit, 8, 0, 1, 1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveValuesTempMinus20()));
    glyout->addWidget(pb, 9, 2, 1, 2);
//     pb = new QPushButton("Отмена");
//     connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(close()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
//     glyout->addWidget(pb,9,3,1,3);

    ask->setLayout(glyout);
    ask->exec();
}

void TuneKIVDialog::Enter60Data()
{
    int i;
    ask = new QDialog(this);
    // QVBoxLayout *lyout = new QVBoxLayout;
    ask->setAttribute(Qt::WA_DeleteOnClose);
    ask->setObjectName("EnterDlg");
    QGridLayout *glyout = new QGridLayout;
    ledit = new QLineEdit();
    QLabel *lbl = new QLabel("Введите значения сигналов c Энергомонитора:");
    glyout->addWidget(lbl, 0, 0, 1, 6);

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Uэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 1, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneU" + QString::number(i));
        glyout->addWidget(ledit, 2, i, 1, 1);
    }

    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("Iэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 3, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetuneI" + QString::number(i));
        glyout->addWidget(ledit, 4, i, 1, 1);
    }

    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("φэт[" + QString::number(i) + "]:");
        glyout->addWidget(lbl, 5, i, 1, 1);
        ledit = new QLineEdit();
        ledit->setObjectName("ValuetunePhi" + QString::number(i));
        glyout->addWidget(ledit, 6, i, 1, 1);
    }

    lbl = new QLabel("fэт:");
    glyout->addWidget(lbl, 7, 0, 1, 1);
    ledit = new QLineEdit();
    ledit->setObjectName("ValuetuneF");
    glyout->addWidget(ledit, 8, 0, 1, 1);

    QPushButton *pb = new QPushButton("Настроить");
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveValuesTemp60()));
    glyout->addWidget(pb, 9, 2, 1, 2);
//     pb = new QPushButton("Отмена");
//     connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(close()));
//     connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
//     glyout->addWidget(pb,9,3,1,3);

    ask->setLayout(glyout);
    ask->exec();
}

void TuneKIVDialog::CloseAsk() { ask->close(); }

void TuneKIVDialog::SaveValuesTemp20()
{
    int i;
    QString tmps;
    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneU" + QString::number(i), tmps);
        m_Uet[i] = tmps.toFloat();
    }

    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneI" + QString::number(i), tmps);
        m_Iet[i] = tmps.toFloat();
    }

    for (i = 0; i < 6; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetunePhi" + QString::number(i), tmps);
        m_PHIet[i] = tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    m_FREQet = tmps.toFloat();

    ask->close();
}

void TuneKIVDialog::SaveValuesTempMinus20()
{
    int i;
    QString tmps;
    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneU" + QString::number(i), tmps);
        m_UetMinus20[i] = tmps.toFloat();
    }

    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneI" + QString::number(i), tmps);
        m_IetMinus20[i] = tmps.toFloat();
    }

    for (i = 0; i < 6; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetunePhi" + QString::number(i), tmps);
        m_PHIetMinus20[i] = tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    m_FREQetMinus20 = tmps.toFloat();

    ask->close();
}

void TuneKIVDialog::SaveValuesTemp60()
{
    int i;
    QString tmps;
    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneU" + QString::number(i), tmps);
        m_Uet60[i] = tmps.toFloat();
    }

    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneI" + QString::number(i), tmps);
        m_Iet60[i] = tmps.toFloat();
    }

    for (i = 0; i < 6; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetunePhi" + QString::number(i), tmps);
        m_PHIet60[i] = tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    m_FREQet60 = tmps.toFloat();

    ask->close();
}

int TuneKIVDialog::CalcTuneCoefs()
{
    int i;
    QString tmps;

    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneU" + QString::number(i), tmps);
        m_Uet[i] = tmps.toFloat();
    }

    for (i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetuneI" + QString::number(i), tmps);
        m_Iet[i] = tmps.toFloat();
    }

    for (i = 0; i < 6; i++)
    {
        WDFunc::LE_read_data(ask, "ValuetunePhi" + QString::number(i), tmps);
        m_PHIet[i] = tmps.toFloat();
    }

    WDFunc::LE_read_data(ask, "ValuetuneF", tmps);
    m_FREQet = tmps.toFloat();

    ask->close();

    if (() != NOERROR)
    {
        QMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
        return GENERALERROR;
    }

    if (m_Kadc == 1)
    {
        for (i = 0; i < 3; i++)
        {
            m_Bac_newblock.KmU[i] = m_Bac_block.KmU[i] * m_Uet[i] / m_Bda_in.IUefNat_filt[i];
            m_Bac_newblock.KmI1[i] = m_Bac_block.KmI1[i] * m_Iet[i] / m_Bda_in.IUefNat_filt[i + 3];
        }

        m_Bac_newblock.K_freq = m_Bac_block.K_freq * m_FREQet / m_Bda_in.Frequency;

        for (i = 1; i < 3; i++)
            m_Bac_newblock.DPsi[i] = m_Bac_block.DPsi[i] - m_Bda_in.phi_next_f[i];

        for (i = 3; i < 6; i++)
            m_Bac_newblock.DPsi[i] = m_Bac_block.DPsi[i] + m_PHIet[i] - m_Bda_in.phi_next_f[i];
    }
    else if (m_Kadc == 2)
    {
        for (i = 0; i < 3; i++)
        {
            m_Bac_newblock.KmI2[i] = m_Bac_block.KmI2[i] * m_Iet[i] / m_Bda_in.IUefNat_filt[i + 3];
        }
    }
    else if (m_Kadc == 4)
    {
        for (i = 0; i < 3; i++)
        {
            m_Bac_newblock.KmI4[i] = m_Bac_block.KmI4[i] * m_Iet[i] / m_Bda_in.IUefNat_filt[i + 3];
        }
    }
    else if (m_Kadc == 8)
    {
        for (i = 0; i < 3; i++)
        {
            m_Bac_newblock.KmI8[i] = m_Bac_block.KmI8[i] * m_Iet[i] / m_Bda_in.IUefNat_filt[i + 3];
        }
    }
    else if (m_Kadc == 16)
    {
        for (i = 0; i < 3; i++)
        {
            m_Bac_newblock.KmI16[i] = m_Bac_block.KmI16[i] * m_Iet[i] / m_Bda_in.IUefNat_filt[i + 3];
        }
    }
    else if (m_Kadc == 32)
    {
        for (i = 0; i < 3; i++)
        {
            m_Bac_newblock.KmI32[i] = m_Bac_block.KmI32[i] * m_Iet[i] / m_Bda_in.IUefNat_filt[i + 3];
        }
    }

    return NOERROR;
}

int TuneDialogKIV::CalcTuneCoefsKadc4()
{

    return NOERROR;
}

int TuneDialogKIV::CalcTuneCoefsKadc8()
{

    return NOERROR;
}

int TuneDialogKIV::CalcTuneCoefsKadc16()
{

    return NOERROR;
}

int TuneDialogKIV::CalcTuneCoefsKadc32()
{

    return NOERROR;
}

void TuneKIVDialog::GetBdAndFill() { }

int TuneKIVDialog::ReadAnalogMeasurements()
{
    int i, j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;

    for (i = 0; i < 3; i++)
    {
        sumU[i] = 0;
        sumI[i] = 0;
        sumPHI[i] = 0;
        sumPHI[i + 3] = 0;
    }

    for (i = 0; i < m_filterSteps; i++)
    {
        // получение текущих аналоговых сигналов от модуля
        if (Commands::GetBd(BT_BASE, &m_Bda_in, sizeof(m_Bda_in)) != NOERROR)
        {
            QMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
            return GENERALERROR;
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                sumU[j] += m_Bda_in.IUefNat_filt[j];
                sumI[j] += m_Bda_in.IUefNat_filt[j + 3];
            }

            for (j = 0; j < 6; j++)
            {
                sumPHI[j] += m_Bda_in.phi_next_f[j];
//                if(j==4)
//                {
//                    samples[inc] = Bda_in.phi_next_f[j];
//                    inc++;
//                }
            }

            sumFreq += m_Bda_in.Frequency;

            QThread::msleep(500);
        }
    }

    for (i = 0; i < 3; i++)
    {
        m_Bda_in.IUefNat_filt[i] = sumU[i] / m_filterSteps;
        m_Bda_in.IUefNat_filt[i + 3] = sumI[i] / m_filterSteps;
    }

    for (i = 0; i < 6; i++)
    {
        m_Bda_in.phi_next_f[i] = sumPHI[i] / m_filterSteps;
    }

    m_Bda_in.Frequency = sumFreq / m_filterSteps;

    return NOERROR;
}

int TuneKIVDialog::ReadAnalogTemp20()
{
    int i, j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;
    for (i = 0; i < m_filterSteps; i++)
    {
        // получение текущих аналоговых сигналов от модуля
        if (Commands::GetBd(BT_BASE, &m_Bda_block20, sizeof(m_Bda_block20)) != NOERROR)
        {
            QMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
            return GENERALERROR;
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                sumU[j] += m_Bda_block20.IUefNat_filt[j];
                sumI[j] += m_Bda_block20.IUefNat_filt[j + 3];
            }

            for (j = 0; j < 6; j++)
            {
                sumPHI[j] += m_Bda_block20.phi_next_f[j];
            }

            sumFreq += m_Bda_block20.Frequency;

            QThread::msleep(500);
        }
    }

    for (i = 0; i < 3; i++)
    {
        m_Bda_block20.IUefNat_filt[i] = sumU[i] / m_filterSteps;
        m_Bda_block20.IUefNat_filt[i + 3] = sumI[i] / m_filterSteps;
    }

    for (i = 0; i < 6; i++)
    {
        m_Bda_block20.phi_next_f[i] = sumPHI[i] / m_filterSteps;
    }

    m_Bda_block20.Frequency = sumFreq / m_filterSteps;

    return NOERROR;
}

int TuneKIVDialog::ReadAnalogTemp60()
{
    int i, j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;
    for (i = 0; i < m_filterSteps; i++)
    {
        // получение текущих аналоговых сигналов от модуля
        if (Commands::GetBd(BT_BASE, &m_Bda_block60, sizeof(m_Bda_block60)) != NOERROR)
        {
            QMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
            return GENERALERROR;
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                sumU[j] += m_Bda_block60.IUefNat_filt[j];
                sumI[j] += m_Bda_block60.IUefNat_filt[j + 3];
            }

            for (j = 0; j < 6; j++)
            {
                sumPHI[j] += m_Bda_block60.phi_next_f[j];
            }

            sumFreq += m_Bda_block60.Frequency;

            QThread::msleep(500);
        }
    }

    for (i = 0; i < 3; i++)
    {
        m_Bda_block60.IUefNat_filt[i] = sumU[i] / m_filterSteps;
        m_Bda_block60.IUefNat_filt[i + 3] = sumI[i] / m_filterSteps;
    }

    for (i = 0; i < 6; i++)
    {
        m_Bda_block60.phi_next_f[i] = sumPHI[i] / m_filterSteps;
    }

    m_Bda_block60.Frequency = sumFreq / m_filterSteps;

    return NOERROR;
}

int TuneKIVDialog::ReadAnalogTempMinus20()
{
    int i, j;
    float sumU[3];
    float sumI[3];
    float sumPHI[6];
    float sumFreq = 0.0;
    for (i = 0; i < m_filterSteps; i++)
    {
        // получение текущих аналоговых сигналов от модуля
        if (Commands::GetBd(BT_BASE, &m_Bda_blockMinus20, sizeof(m_Bda_blockMinus20)) != NOERROR)
        {
            QMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
            return GENERALERROR;
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                sumU[j] += m_Bda_blockMinus20.IUefNat_filt[j];
                sumI[j] += m_Bda_blockMinus20.IUefNat_filt[j + 3];
            }

            for (j = 0; j < 6; j++)
            {
                sumPHI[j] += m_Bda_blockMinus20.phi_next_f[j];
            }

            sumFreq += m_Bda_blockMinus20.Frequency;

            QThread::msleep(500);
        }
    }

    for (i = 0; i < 3; i++)
    {
        m_Bda_blockMinus20.IUefNat_filt[i] = sumU[i] / m_filterSteps;
        m_Bda_blockMinus20.IUefNat_filt[i + 3] = sumI[i] / m_filterSteps;
    }

    for (i = 0; i < 6; i++)
    {
        m_Bda_blockMinus20.phi_next_f[i] = sumPHI[i] / m_filterSteps;
    }

    m_Bda_blockMinus20.Frequency = sumFreq / m_filterSteps;

    return NOERROR;
}

int TuneKIVDialog::SaveUeff()
{
//        // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
//        for (int i=0; i<6; i++)
//            IUefNat_filt_old[i] = Bda_block.IUefNat_filt[i];
    return NOERROR;
}

int TuneKIVDialog::LoadWorkConfig()
{
    // пишем ранее запомненный конфигурационный блок
    memcpy(&CKIV->Bci_block, &m_Bci_block_work, sizeof(ConfigKIV::Bci));
    if (Commands::WriteFile(CM_CONFIGFILE, S2ConfigForTune) != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

void TuneKIVDialog::SetExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == nullptr)
        return;
    for (int i = 0; i < 3; ++i)
    {
        QDoubleSpinBox *spb = this->findChild<QDoubleSpinBox *>("spb7371" + QString::number(i));
        if (spb != nullptr)
            m_RealData.u[i] = spb->value();
        spb = this->findChild<QDoubleSpinBox *>("spb7371" + QString::number(i + 3));
        if (spb != nullptr)
            m_RealData.i[i] = spb->value();
        spb = this->findChild<QDoubleSpinBox *>("spb7371" + QString::number(i + 6));
        if (spb != nullptr)
            m_RealData.d[i] = spb->value();
    }
    QDoubleSpinBox *spb = this->findChild<QDoubleSpinBox *>("spb73719");
    if (spb != nullptr)
        m_RealData.f[0] = spb->value();
    StdFunc::ClearCancel();
    dlg->close();
}

void TuneKIVDialog::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("dlg7371");
    if (dlg == nullptr)
        return;
    StdFunc::Cancel();
    dlg->close();
}

void TuneKIVDialog::CancelTune() { StdFunc::Cancel(); }

void TuneKIVDialog::RefreshAnalogValues(int bdnum)
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

int TuneKIVDialog::TuneTempCor()
{

    int i;
    QString tmps;
    WDFunc::LE_read_data(ask, "N", tmps);
    m_filterSteps = tmps.toInt();
    ask->close();
    // float sum = 0.0;
    SaveWorkConfig();

    if (Commands::GetBac(BT_MEZONIN, &m_Bac_block, sizeof(Bac)) != NOERROR)
    {
        WARNMSG("Ошибка при приёме данных");
        return GENERALERROR;
    }

    for (i = 0; i < 3; i++)
    {
        m_Bac_block.TKPsi_a[i] = 0;
        m_Bac_block.TKPsi_b[i] = 0;
        m_Bac_block.TKUa[i] = 0;
        m_Bac_block.TKUb[i] = 0;
    }
    if (Commands::WriteBac(BT_MEZONIN, &m_Bac_block, sizeof(Bac)) != NOERROR)
    {
        WARNMSG("Ошибка при отправке данных");
        return GENERALERROR;
    }
    // обновление коэффициентов в соответствующих полях на экране
    //    FillBac(0);
    m_VModel->updateModel();
    m_Bac_newblock = m_Bac_block;
    WaitNSeconds(5);
    Start7_3_4_11();
    // Average();

    if (Show125mA() == GENERALERROR)
        return GENERALERROR;
    else
    {
        if (AskTemp20() == GENERALERROR)
            return GENERALERROR;
        else
        {

            Enter20Data();

            if (ReadAnalogTemp20() != NOERROR)
                return GENERALERROR;

            if (Commands::GetBd(0, &m_Bd_block20, sizeof(m_Bd_block20)) != NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return GENERALERROR;
            }
        }

        if (AskTemp60() == GENERALERROR)
            return GENERALERROR;
        else
        {

            Enter60Data();

            if (ReadAnalogTemp60() != NOERROR)
                return GENERALERROR;

            if (Commands::GetBd(0, &m_Bd_block_plus60, sizeof(m_Bd_block_plus60)) != NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return GENERALERROR;
            }
        }

        if (AskTempMinus20() == GENERALERROR)
            return GENERALERROR;
        else
        {

            Enterminus20Data();

            if (ReadAnalogTempMinus20() != NOERROR)
                return GENERALERROR;

            if (Commands::GetBd(0, &m_Bd_block_minus20, sizeof(m_Bd_block_minus20)) != NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return GENERALERROR;
            }
        }
    }

    // FillBd1(this);
    CalcTempCorCoefs();
    FillNewBac();
    WaitNSeconds(5);

    if (Commands::WriteBac(BT_MEZONIN, &m_Bac_newblock, sizeof(m_Bac_newblock)) == NOERROR)
    {
        QMessageBox::information(this, "Настройка", "Настройка завершена");
        WaitNSeconds(5);
        LoadWorkConfig();
        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneKIVDialog::Show125mA()
{

    if (QMessageBox::question(this, "Настройка",
            "Задайте ток источника равным 125 мА"
            " и нажмите OK",
            nullptr, "Ok", "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneKIVDialog::AskTemp20()
{

    if (QMessageBox::question(this, "Настройка",
            "Установите в камере температуру 20±2°С"
            " и через 30 минут нажмите OK",
            nullptr, "Ok", "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneKIVDialog::AskTempMinus20()
{

    if (QMessageBox::question(this, "Настройка",
            "Установите в камере температуру минус 20±2°С"
            " и через 30 минут нажмите OK",
            nullptr, "Ok", "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneKIVDialog::AskTemp60()
{

    if (QMessageBox::question(this, "Настройка",
            "Установите в камере температуру 60±2°С"
            " и через 30 минут нажмите OK",
            nullptr, "Ok", "Close"))
    {

        return NOERROR;
    }
    else
        return GENERALERROR;
}

int TuneKIVDialog::Average()
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Количество усреднений");
    ledit = new QLineEdit;
    ledit->setObjectName("N");
    QPushButton *pb = new QPushButton;
    ask = new QDialog();
    ask->setAttribute(Qt::WA_DeleteOnClose);

    glyout->addWidget(lbl, 0, 1, 1, 1);
    glyout->addWidget(ledit, 1, 1, 1, 1);
    pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(ReadN()));
    glyout->addWidget(pb, 2, 1, 1, 1);
    vlyout->addLayout(glyout);
    ask->setLayout(vlyout);
    ask->exec();

    return NOERROR;
}

void TuneKIVDialog::CalcTempCorCoefs()
{
    int i;
    float deltaTplus, deltaTminus, dUplus[3], dUminus[3], deltaPhiplus[3], deltaPhiminus[3];

    deltaTplus = m_Bd_block_plus60.Tmk - m_Bd_block20.Tmk;
    deltaTminus = m_Bd_block_minus20.Tmk - m_Bd_block20.Tmk;

    for (i = 0; i < 3; i++)
    {
        dUplus[i] = ((m_Bda_block60.IUefNat_filt[i] * m_Uet[i]) / (m_Bda_block20.IUefNat_filt[i] * m_Uet60[i])) - 1;
        dUminus[i]
            = ((m_Bda_blockMinus20.IUefNat_filt[i] * m_Uet[i]) / (m_Bda_block20.IUefNat_filt[i] * m_UetMinus20[i])) - 1;
        m_Bac_newblock.TKUa[i] = ((dUminus[i] * deltaTplus * deltaTplus) - (dUplus[i] * deltaTminus * deltaTminus))
            / (deltaTplus * deltaTminus * (deltaTplus - deltaTminus));
        m_Bac_newblock.TKUb[i] = ((dUplus[i] * deltaTminus) - (dUminus[i] * deltaTplus))
            / (deltaTplus * deltaTminus * (deltaTplus - deltaTminus));
    }

    for (i = 0; i < 6; i++)
    {
        deltaPhiplus[i] = (m_Bda_block60.phi_next_f[i] - m_Bda_block20.phi_next_f[i]) - (m_PHIet60[i] - m_PHIet[i]);
        deltaPhiminus[i]
            = (m_Bda_blockMinus20.phi_next_f[i] - m_Bda_block20.phi_next_f[i]) - (m_PHIetMinus20[i] - m_PHIet[i]);
        m_Bac_newblock.TKPsi_a[i]
            = ((deltaPhiminus[i] * deltaTplus * deltaTplus) - (deltaPhiplus[i] * deltaTminus * deltaTminus))
            / (deltaTplus * deltaTminus * (deltaTplus - deltaTminus));
        m_Bac_newblock.TKPsi_b[i] = ((deltaPhiplus[i] * deltaTminus) - (deltaPhiminus[i] * deltaTplus))
            / (deltaTplus * deltaTminus * (deltaTplus - deltaTminus));
    }
} */

void TuneKIVDialog::GenerateReport()
{
    /*    // данные в таблицу уже получены или из файла, или в процессе работы
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
            if((RealData.d[0]>0 && ReportHeader.PhiloadA.toFloat() < 0) || (RealData.d[0]<0 &&
       ReportHeader.PhiloadA.toFloat() > 0))
            {
               ReportHeader.OffsetPhiloadA = QString::number(RealData.d[0] + ReportHeader.PhiloadA.toFloat(), 'f', 3);
               RealData.d[0] = -RealData.d[0];
            }
            else
               ReportHeader.OffsetPhiloadA = QString::number(RealData.d[0] - ReportHeader.PhiloadA.toFloat(), 'f', 3);

            if((RealData.d[1]>0 && ReportHeader.PhiloadB.toFloat() < 0) || (RealData.d[1]<0 &&
       ReportHeader.PhiloadB.toFloat() > 0))
            {
               ReportHeader.OffsetPhiloadB = QString::number(RealData.d[1] + ReportHeader.PhiloadB.toFloat(), 'f', 3);
               RealData.d[1] = -RealData.d[1];
            }
            else
               ReportHeader.OffsetPhiloadB = QString::number(RealData.d[1] - ReportHeader.PhiloadB.toFloat(), 'f', 3);

            if((RealData.d[2]>0 && ReportHeader.PhiloadC.toFloat() < 0) || (RealData.d[2]<0 &&
       ReportHeader.PhiloadC.toFloat() > 0))
            {
               ReportHeader.OffsetPhiloadC = QString::number(RealData.d[2] + ReportHeader.PhiloadC.toFloat(), 'f', 3);
               RealData.d[2] = -RealData.d[2];
            }
            else
               ReportHeader.OffsetPhiloadC = QString::number(RealData.d[2] - ReportHeader.PhiloadC.toFloat(), 'f', 3);

            ReportHeader.OffsetPhiUAB = QString::number(RealData.dpsiU[0] - ReportHeader.PhiUAB.toFloat(), 'f', 3);
            ReportHeader.OffsetPhiUBC = QString::number(RealData.dpsiU[1] - ReportHeader.PhiUBC.toFloat(), 'f', 3);

            report->dataManager()->setReportVariable("FreqMIP", QString::number(RealData.f[0], 'f', 3));
            report->dataManager()->setReportVariable("UA_MIP."+QString::number(i), QString::number(RealData.u[0], 'f',
       3)); report->dataManager()->setReportVariable("UB_MIP."+QString::number(i), QString::number(RealData.u[1], 'f',
       3)); report->dataManager()->setReportVariable("UC_MIP."+QString::number(i), QString::number(RealData.u[2], 'f',
       3)); report->dataManager()->setReportVariable("IA_MIP."+QString::number(i), QString::number(RealData.i[0], 'f',
       3)); report->dataManager()->setReportVariable("IB_MIP."+QString::number(i), QString::number(RealData.i[1], 'f',
       3)); report->dataManager()->setReportVariable("IC_MIP."+QString::number(i), QString::number(RealData.i[2], 'f',
       3)); report->dataManager()->setReportVariable("PhiLA_MIP."+QString::number(i), QString::number(RealData.d[0],
       'f', 3)); report->dataManager()->setReportVariable("PhiLB_MIP."+QString::number(i),
       QString::number(RealData.d[1], 'f', 3));
       report->dataManager()->setReportVariable("PhiLC_MIP."+QString::number(i), QString::number(RealData.d[2], 'f',
       3)); report->dataManager()->setReportVariable("PhiUab_MIP."+QString::number(i),
       QString::number(RealData.dpsiU[0], 'f', 3));
       report->dataManager()->setReportVariable("PhiUbc_MIP."+QString::number(i), QString::number(RealData.dpsiU[1],
       'f', 3)); report->dataManager()->setReportVariable("Freq."+QString::number(i),
       QString::number(Bda_block.Frequency, 'f', 3)); report->dataManager()->setReportVariable("UA."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[0], 'f', 3));
            report->dataManager()->setReportVariable("UB."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[1], 'f', 3));
       report->dataManager()->setReportVariable("UC."+QString::number(i), QString::number(Bda_block.IUefNat_filt[2],
       'f', 3)); report->dataManager()->setReportVariable("IA."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[3], 'f', 3));
       report->dataManager()->setReportVariable("IB."+QString::number(i), QString::number(Bda_block.IUefNat_filt[4],
       'f', 3)); report->dataManager()->setReportVariable("IC."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[5], 'f', 3));
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

        if (QMessageBox::question(this,"Сохранить","Сохранить протокол поверки?"))
        {
            QString filename = Files::ChooseFileForSave(this, "*.pdf", "pdf");
            if (!filename.isEmpty())
            {
                report->designReport();
                report->printToPDF(filename);
        //        report->previewReport();
              //  report->designReport();
                QMessageBox::information(this, "Успешно!", "Записано успешно!");
            }
            else
                QMessageBox::information(this, "Отменено", "Действие отменено");
        }
        delete report; */
}
