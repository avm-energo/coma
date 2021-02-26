#include "confkivdialog.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/s2.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QVBoxLayout>

ConfKIVDialog::ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent) : AbstractConfDialog(parent)
{

    CKIV = ckiv;
    // S2Config = S2Config;
    //    CKIV = new ConfigKIV(ckiv);
    //    Conf = new ConfDialog(ckiv->S2Config(), this);
    //    ConfKxx = new ConfKxxDialog(ckiv->S2Config(), this);
}

ConfKIVDialog::~ConfKIVDialog()
{
    delete CKIV;
}

void ConfKIVDialog::Fill()
{

    CKIV->MainConfig()->Fill();
    CKIV->KxxConfig()->Fill();
    //    Conf->Fill();
    //    ConfKxx->Fill();

    WDFunc::SetSPBData(this, "Unom", CKIV->Bci_block.Unom);
    WDFunc::SetSPBData(this, "Umin", CKIV->Bci_block.Umin);
    WDFunc::SetSPBData(this, "Imin", CKIV->Bci_block.Imin);

    for (int i = 0; i < 3; i++)
    {

        WDFunc::SetSPBData(this, "Tg_pasp." + QString::number(i), CKIV->Bci_block.Tg_pasp[i]);
        WDFunc::SetSPBData(this, "C_pasp." + QString::number(i), CKIV->Bci_block.C_pasp[i]);
    }

    WDFunc::SetSPBData(this, "dС_pred", CKIV->Bci_block.dC_pred);
    WDFunc::SetSPBData(this, "dС_alarm", CKIV->Bci_block.dC_alarm);
    WDFunc::SetSPBData(this, "dTg_pred", CKIV->Bci_block.dTg_pred);
    WDFunc::SetSPBData(this, "dTg_alarm", CKIV->Bci_block.dTg_alarm);
    WDFunc::SetSPBData(this, "dIunb_pred", CKIV->Bci_block.dIunb_pred);
    WDFunc::SetSPBData(this, "dIunb_alarm", CKIV->Bci_block.dIunb_alarm);
    WDFunc::SetSPBData(this, "GdС", CKIV->Bci_block.GdC);
    WDFunc::SetSPBData(this, "GdTg", CKIV->Bci_block.GdTg);
    WDFunc::SetSPBData(this, "GdIunb", CKIV->Bci_block.GdIunb);
    WDFunc::SetSPBData(this, "NFiltr", CKIV->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "T_Data_Rec", CKIV->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "U2nom", CKIV->Bci_block.Unom_1);
    WDFunc::SetSPBData(this, "Ulow", CKIV->Bci_block.LowU);
    WDFunc::SetSPBData(this, "Tevent_pred", CKIV->Bci_block.Tevent_pred);
    WDFunc::SetSPBData(this, "Tevent_alarm", CKIV->Bci_block.Tevent_alarm);

    if (CKIV->Bci_block.IsU)
        WDFunc::SetChBData(this, "IsU", true);
    else
        WDFunc::SetChBData(this, "IsU", false);

    if (CKIV->Bci_block.IsIunb)
        WDFunc::SetChBData(this, "IsIunb", true);
    else
        WDFunc::SetChBData(this, "IsIunb", false);
}

void ConfKIVDialog::FillBack()
{

    CKIV->MainConfig()->FillBack();
    CKIV->KxxConfig()->FillBack();
    //    Conf->FillBack();
    //    ConfKxx->FillBack();

    WDFunc::SPBData(this, "Unom", CKIV->Bci_block.Unom);
    WDFunc::SPBData(this, "Umin", CKIV->Bci_block.Umin);
    WDFunc::SPBData(this, "Imin", CKIV->Bci_block.Imin);

    for (int i = 0; i < 3; i++)
    {

        WDFunc::SPBData(this, "Tg_pasp." + QString::number(i), CKIV->Bci_block.Tg_pasp[i]);
        WDFunc::SPBData(this, "C_pasp." + QString::number(i), CKIV->Bci_block.C_pasp[i]);
    }

    WDFunc::SPBData(this, "dС_pred", CKIV->Bci_block.dC_pred);
    WDFunc::SPBData(this, "dС_alarm", CKIV->Bci_block.dC_alarm);
    WDFunc::SPBData(this, "dTg_pred", CKIV->Bci_block.dTg_pred);
    WDFunc::SPBData(this, "dTg_alarm", CKIV->Bci_block.dTg_alarm);
    WDFunc::SPBData(this, "dIunb_pred", CKIV->Bci_block.dIunb_pred);
    WDFunc::SPBData(this, "dIunb_alarm", CKIV->Bci_block.dIunb_alarm);
    WDFunc::SPBData(this, "GdС", CKIV->Bci_block.GdC);
    WDFunc::SPBData(this, "GdTg", CKIV->Bci_block.GdTg);
    WDFunc::SPBData(this, "GdIunb", CKIV->Bci_block.GdIunb);
    WDFunc::SPBData(this, "NFiltr", CKIV->Bci_block.NFiltr);
    WDFunc::SPBData(this, "T_Data_Rec", CKIV->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "U2nom", CKIV->Bci_block.Unom_1);
    WDFunc::SPBData(this, "Ulow", CKIV->Bci_block.LowU);
    WDFunc::SPBData(this, "Tevent_pred", CKIV->Bci_block.Tevent_pred);
    WDFunc::SPBData(this, "Tevent_alarm", CKIV->Bci_block.Tevent_alarm);

    WDFunc::ChBData(this, "IsU", Variable);
    if (Variable)
        CKIV->Bci_block.IsU = 1;
    else
        CKIV->Bci_block.IsU = 0;

    WDFunc::ChBData(this, "IsIunb", Variable);
    if (Variable)
        CKIV->Bci_block.IsIunb = 1;
    else
        CKIV->Bci_block.IsIunb = 0;
}

QWidget *ConfKIVDialog::analogWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);

    const QStringList phase { "Фаза A:", "Фаза B:", "Фаза C:" };
    //    QString paramcolor = Colors::MAINWINCLR;
    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Unom", 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "U2nom", 0, 10000, 1), row, 2, 1, 3);
    row++;

    for (int i = 0; i < phase.size(); i++)
    {
        gridlyout->addWidget(WDFunc::NewLBL2(this, phase.at(i)), row, 2 + i, 1, 1, Qt::AlignTop);
    }
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Паспортные значения ёмкости вводов, пФ:"), row, 1, 1, 1, Qt::AlignTop);
    for (int i = 0; i < 3; i++)
    {

        gridlyout->addWidget(
            WDFunc::NewSPB2(this, "C_pasp." + QString::number(i), 0, 20000, 1), row, 2 + i, 1, 1, Qt::AlignTop);
    }
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Паспортные значения tg вводов, %:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        gridlyout->addWidget(WDFunc::NewSPB2(this, "Tg_pasp." + QString::number(i), 0, 10, 2), row, 2 + i, 1, 1);
    }
    row++;

    //    for (int i = 0; i < 10; i++)
    //    {
    //        gridlyout->addWidget(WDFunc::NewLBL2(this, ""), row, 1, 1, 1);
    //        row++;
    //    }

    //    vlyout2->addLayout(gridlyout);
    //    gb->setLayout(vlyout2);
    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::thresholdsWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;

    int row = 0;
    QGroupBox *gb = new QGroupBox("Уставки сигнализации");

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dС_pred", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dС_alarm", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dTg_pred", 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dTg_alarm", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка предупредительной сигнализации по "
                             "изменению небаланса токов, %:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dIunb_pred", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению небаланса токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "dIunb_alarm", 0, 10000, 1), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Уставки контроля минимума тока и напряжения");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка контроля минимума напряжения (в % от номинального):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Umin", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка контроля минимума тока (в % от Imax):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Imin", 0, 10000, 1), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::remainsWidget()
{
    QWidget *w = new QWidget;
    QGroupBox *gb = new QGroupBox("Гистерезис");

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    int row = 0;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по dC, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "GdС", 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по dTg, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "GdTg", 0, 10000, 3), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по небалансу токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "GdIunb", 0, 10000, 1), row, 2, 1, 3);
    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    //............................................................

    gb = new QGroupBox("Сигнализации событий");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Низкое напряжение для сигнализации:"), row, 0, 1, 1, Qt::AlignLeft);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Ulow", 0, 10000, 1), row, 1, 1, 1);

    gridlyout->addWidget(
        WDFunc::NewChB2(this, "IsU", "Сигнализация по наличию входного напряжения, % от ном."), row, 2, 1, 1);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Задержка на формирование предупредительных событий, сек:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Tevent_pred", 0, 10000, 1), row, 1, 1, 1);

    gridlyout->addWidget(WDFunc::NewChB2(this, "IsIunb", "Сигнализация по небалансу токов"), row, 2, 1, 1);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Задержка на формирование аварийных событий, сек:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "Tevent_alarm", 0, 10000, 1), row, 1, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "NFiltr", 0, 10000, 0), row, 1, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, "T_Data_Rec", 0, 10000, 0), row, 1, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::connectionWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;

    QGridLayout *gridlyout = new QGridLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");

    //    gridlyout->addWidget(Conf->SetupMainBlk(this), 0, 0, 1, 1);
    //    gridlyout->addWidget(ConfKxx->SetupComParam(this), 0, 1, 1, 1);
    gridlyout->addWidget(CKIV->MainConfig()->MainWidget(this), 0, 0, 1, 1);
    gridlyout->addWidget(CKIV->KxxConfig()->ComParam(this), 0, 1, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка времени");

    //    vlyout->addWidget(Conf->SetupTime(this));
    vlyout->addWidget(CKIV->MainConfig()->TimeWidget(this));

    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

void ConfKIVDialog::SetupUI()
{
    //    QString phase[3] = { "Фаза A:", "Фаза B:", "Фаза C:" };
    // QString S;
    //    QVBoxLayout *vlyout1 = new QVBoxLayout;
    //    QVBoxLayout *vlyout2 = new QVBoxLayout;
    //    QGridLayout *gridlyout = new QGridLayout;
    // Закомментировал т.к. не используется
    //    QScrollArea *area = new QScrollArea;
    //    QScrollArea *area2 = new QScrollArea;
    //    QScrollArea *scrArea = new QScrollArea;
    //    QWidget *analog1 = new QWidget;
    //    QWidget *analog2 = new QWidget;
    //    QWidget *extraconf = new QWidget;
    //    QWidget *MEKconf = new QWidget;
    //    QWidget *Leftconf = new QWidget;
    //    QWidget *link = new QWidget;
    //    QWidget *Ust = new QWidget;
    //    QWidget *time = new QWidget;
    //    QString tmps = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
    //    analog1->setStyleSheet(tmps);
    //    analog2->setStyleSheet(tmps);
    //    extraconf->setStyleSheet(tmps);
    //    time->setStyleSheet(tmps);
    //    MEKconf->setStyleSheet(tmps);
    //    Leftconf->setStyleSheet(tmps);
    //    link->setStyleSheet(tmps);
    //    Ust->setStyleSheet(tmps);

    //    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    //    area->setFrameShape(QFrame::NoFrame);
    //    area->setWidgetResizable(true);

    //    area2->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    //    area2->setFrameShape(QFrame::NoFrame);
    //    area2->setWidgetResizable(true);

    //    scrArea->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    //    scrArea->setFrameShape(QFrame::NoFrame);
    //    scrArea->setWidgetResizable(true);

    //    QString paramcolor = Colors::MAINWINCLR;
    //    QFont font;

    //............................................................

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    //  QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::TABCOLOR) + ";}";
    //   ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(analogWidget(), "Аналоговые");

    ConfTW->addTab(thresholdsWidget(), "Уставки");

    ConfTW->addTab(remainsWidget(), "Остальное");

    ConfTW->addTab(connectionWidget(), "Связь");
    //    ConfTW->addTab(ConfKxx->SetupModBus(this), "ModBusMaster");
    //    ConfTW->addTab(ConfKxx->SetupBl(this), "Общее");
    ConfTW->addTab(CKIV->KxxConfig()->ModbusWidget(this), "ModBusMaster");
    ConfTW->addTab(CKIV->KxxConfig()->VariousWidget(this), "Общее");

    //    QWidget *wdgt = ConfButtons();
    //    lyout->addWidget(wdgt);

    lyout->addWidget(ConfTW);
    lyout->addWidget(ConfButtons());
    setLayout(lyout);
}

void ConfKIVDialog::CheckConf()
{
}

void ConfKIVDialog::SetDefConf()
{
    CKIV->setDefConf();
    //    Conf->SetDefConf();
    //    ConfKxx->SetDefConf();
    Fill();
}

// void ConfKIVDialog::Start_Timer()
//{
//    timerRead->start(1000);
//}

// void ConfKIVDialog::Stop_Timer()
//{
//    timerRead->stop();
//}
