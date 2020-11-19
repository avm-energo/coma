#include "confkivdialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

ConfKIVDialog::ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent) : AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    setStyleSheet(tmps);
    //    this->S2Config = ckiv;
    CKIV = ckiv;
    S2Config = ckiv->getS2Config();
    Conf = new ConfDialog(S2Config, Board::GetInstance().typeB(), Board::GetInstance().typeM(), this);
    ConfKxx = new ConfKxxDialog(S2Config, this);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    PrereadConf();
}

ConfKIVDialog::~ConfKIVDialog()
{
    delete CKIV;
}

void ConfKIVDialog::Fill()
{
    int i;

    Conf->Fill();
    ConfKxx->Fill();

    WDFunc::SetSPBData(this, "Unom", CKIV->Bci_block.Unom);
    WDFunc::SetSPBData(this, "Umin", CKIV->Bci_block.Umin);
    WDFunc::SetSPBData(this, "Imin", CKIV->Bci_block.Imin);

    for (i = 0; i < 3; i++)
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
    int i;

    Conf->FillBack();
    ConfKxx->FillBack();

    WDFunc::SPBData(this, "Unom", CKIV->Bci_block.Unom);
    WDFunc::SPBData(this, "Umin", CKIV->Bci_block.Umin);
    WDFunc::SPBData(this, "Imin", CKIV->Bci_block.Imin);

    for (i = 0; i < 3; i++)
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
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}");
    QFont font;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    QString phase[3] = { "Фаза A:", "Фаза B:", "Фаза C:" };
    //    QString paramcolor = Colors::MAINWINCLR;
    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Unom", 0, 10000, 0, Colors::MAINWINCLR), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "U2nom", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);
    row++;

    for (int i = 0; i < 3; i++)
    {
        gridlyout->addWidget(WDFunc::NewLBL(this, phase[i]), row, 2 + i, 1, 1, Qt::AlignTop);
    }
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Паспортные значения ёмкости вводов, пФ:"), row, 1, 1, 1, Qt::AlignTop);
    for (int i = 0; i < 3; i++)
    {

        gridlyout->addWidget(WDFunc::NewSPB(this, "C_pasp." + QString::number(i), 0, 20000, 1, Colors::MAINWINCLR), row,
            2 + i, 1, 1, Qt::AlignTop);
    }
    row++;

    gridlyout->addWidget(WDFunc::NewLBL(this, "Паспортные значения tg вводов, %:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        gridlyout->addWidget(
            WDFunc::NewSPB(this, "Tg_pasp." + QString::number(i), 0, 10, 2, Colors::MAINWINCLR), row, 2 + i, 1, 1);
    }
    row++;

    for (int i = 0; i < 10; i++)
    {
        gridlyout->addWidget(WDFunc::NewLBL(this, ""), row, 1, 1, 1);
        row++;
    }

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
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}");
    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *alyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    QScrollArea *area = new QScrollArea;
    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    //    QString phase[3] = { "Фаза A:", "Фаза B:", "Фаза C:" };
    //    QString paramcolor = Colors::MAINWINCLR;
    int row = 0;
    QGroupBox *gb = new QGroupBox("Уставки сигнализации");
    gb->setFont(font);
    //    vlyout1 = new QVBoxLayout;
    //    vlyout2 = new QVBoxLayout;
    //    gridlyout = new QGridLayout;
    //    row = 0;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dС_pred", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dС_alarm", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dTg_pred", 0, 1000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dTg_alarm", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this,
                             "Уставка предупредительной сигнализации по "
                             "изменению небаланса токов, %:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dIunb_pred", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению небаланса токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "dIunb_alarm", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    alyout->addWidget(gb);

    gb = new QGroupBox("Уставки контроля минимума тока и напряжения");
    gb->setFont(font);
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Уставка контроля минимума напряжения (в % от номинального):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Umin", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума тока (в % от Imax):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    alyout->addWidget(gb);
    area->setLayout(alyout);
    //    Ust->setLayout(vlyout1);
    //    area2->setWidget(Ust);
    lyout->addWidget(area);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::remainsWidget()
{
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}");
    QGroupBox *gb = new QGroupBox("Гистерезис");
    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    //    vlyout2 = new QVBoxLayout;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    int row = 0;

    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dC, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "GdС", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dTg, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "GdTg", 0, 10000, 3, Colors::MAINWINCLR), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по небалансу токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "GdIunb", 0, 10000, 1, Colors::MAINWINCLR), row, 2, 1, 3);

    //    vlyout2->addLayout(gridlyout);
    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    //............................................................

    gb = new QGroupBox("Сигнализации событий");
    gb->setFont(font);
    //    vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Низкое напряжение для сигнализации:"), row, 0, 1, 1, Qt::AlignLeft);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Ulow", 0, 10000, 1, Colors::MAINWINCLR), row, 1, 1, 1);

    gridlyout->addWidget(WDFunc::NewChB(this, "IsU", "Сигнализация по наличию входного напряжения"), row, 2, 1, 1);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Задержка на формирование предупредительных событий:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Tevent_pred", 0, 10000, 1, Colors::MAINWINCLR), row, 1, 1, 1);

    gridlyout->addWidget(WDFunc::NewChB(this, "IsIunb", "Сигнализация по небалансу токов"), row, 2, 1, 1);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Задержка на формирование аварийных событий:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "Tevent_alarm", 0, 10000, 1, Colors::MAINWINCLR), row, 1, 1, 1);

    //    vlyout2->addLayout(gridlyout);
    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("");
    gb->setFont(font);
    //    vlyout2 = new QVBoxLayout;
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "NFiltr", 0, 10000, 0, Colors::MAINWINCLR), row, 1, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, Colors::MAINWINCLR), row, 1, 1, 3);

    //    vlyout2->addLayout(gridlyout);
    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::connectionWidget()
{
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}");
    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *alyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    QScrollArea *area = new QScrollArea;
    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);
    QGridLayout *gridlyout = new QGridLayout;
    //    vlyout2 = new QVBoxLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    gb->setFont(font);

    gridlyout->addWidget(Conf->SetupMainBlk(this), 0, 0, 1, 1);
    gridlyout->addWidget(ConfKxx->SetupComParam(this), 0, 1, 1, 1);

    //    vlyout2->addLayout(gridlyout);
    gb->setLayout(gridlyout);
    alyout->addWidget(gb);

    gb = new QGroupBox("Настройка времени");
    //    vlyout2 = new QVBoxLayout;
    gb->setFont(font);

    vlyout->addWidget(Conf->SetupTime(this));

    gb->setLayout(vlyout);
    alyout->addWidget(gb);
    area->setLayout(alyout);
    lyout->addWidget(area);
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
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::TABCOLOR) + ";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(analogWidget(), "Аналоговые");

    ConfTW->addTab(thresholdsWidget(), "Уставки");

    ConfTW->addTab(remainsWidget(), "Остальное");
    lyout->addWidget(ConfTW);

    ConfTW->addTab(connectionWidget(), "Связь");
    ConfTW->addTab(ConfKxx->SetupModBus(this), "ModBusMaster");
    ConfTW->addTab(ConfKxx->SetupBl(this), "Общее");

    //    QWidget *wdgt = ConfButtons();
    //    lyout->addWidget(wdgt);
    lyout->addWidget(ConfButtons());
    setLayout(lyout);
}

void ConfKIVDialog::CheckConf()
{
}

void ConfKIVDialog::SetDefConf()
{
    CKIV->setDefConf();
    Conf->SetDefConf();
    ConfKxx->SetDefConf();
    Fill();
}

// void ConfKIVDialog::Start_Timer() { timerRead->start(1000); }

// void ConfKIVDialog::Stop_Timer() { timerRead->stop(); }
