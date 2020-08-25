#include "checkkdv.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QLayout>
#include <QScrollArea>
#include <QScrollBar>

CheckKDV::CheckKDV()
{
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(UCONFCLR) + ";}";
}

QWidget *CheckKDV::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "A", "B", "C" };
    QFont font;

    QGroupBox *gb = new QGroupBox("Входные сигналы");
    gb->setFont(font);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "Частота, Гц"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "Frequency", ValuesFormat, "Частота, Гц"), 1, 0, 1, 1);

    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "IUefNat_filt ф." + phase[i] + ", В"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "IUefNat_filt" + QString::number(i), ValuesFormat,
                              "Истинные действующие значения сигналов, В"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "IUefNat_filt ф." + phase[i] + ", мА"), 2, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "IUefNat_filt" + QString::number(i + 3), ValuesFormat,
                              "Истинные действующие значения сигналов, мА"),
            3, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "UefNatLin_filt ф." + phase[i] + ", мА"), 4, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", "UefNatLin_filt" + QString::number(i), ValuesFormat, "Линейное напряжение, В"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "phi_U ф." + phase[i] + ", Град"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "phi_next_f" + QString::number(i), ValuesFormat,
                              "Углы сдвига сигналов по 1-ой гармонике относительно Ua, Град"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "phi_I ф." + phase[i] + ", Град"), 6, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "phi_next_f" + QString::number(i + 3), ValuesFormat,
                              "Углы сдвига сигналов по 1-ой гармонике относительно Ua, Град"),
            7, i + 3, 1, 1);
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKDV::Bd2W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "A", "B", "C", "ср." };
    QFont font;
    int i;

    QGroupBox *gb = new QGroupBox("Первичные");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    for (i = 0; i < 4; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Gr1IUefNat_filt ф." + phase[i]), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1000 + i), ValuesFormat,
                              "Истинные действующие значения сигналов"),
            1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Gr1IUeff_filtered ф." + phase[i]), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1004 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз по 1-й гармонике"),
            3, i, 1, 1);
    }

    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "KrF_Gr1 ф." + phase[i]), 4, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(1008 + i), ValuesFormat, "Крест-фактор"), 5, i, 1, 1);
    }

    for (i = 0; i < 4; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Gr2IUefNat_filt ф." + phase[i]), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1100 + i), ValuesFormat,
                              "Истинные действующие значения сигналов"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Gr2IUeff_filtered ф." + phase[i]), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1104 + i), ValuesFormat,
                              "Действующие значения сигналов трех фаз по 1-й гармонике"),
            9, i, 1, 1);
    }

    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "KrF_Gr2 ф." + phase[i]), 10, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(1108 + i), ValuesFormat, "Крест-фактор"), 11, i, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKDV::Bd3W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "A", "B", "C", "ср." };
    QFont font;
    int i;

    QGroupBox *gb = new QGroupBox("Качество");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "U0_Gr1"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1020", ValuesFormat, "Сигнал нулевой последовательности"), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1_Gr1"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1021", ValuesFormat, "Сигнал прямой последовательности"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2_Gr1"), 0, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1022", ValuesFormat, "Сигнал обратной последовательности"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim0_Gr1"), 0, 3, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1023", ValuesFormat, "Коэфф. несимметрии по нулевой посл."), 1, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim2_Gr1"), 0, 4, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1024", ValuesFormat, "Коэфф. несимметрии по обратной посл."), 1, 4, 1, 1);

    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "THD_Gr1 ф." + phase[i]), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "THD_Gr1" + QString::number(i), ValuesFormat,
                              "Общий коэфф. гармонических искажений"),
            3, i, 1, 1);
    }

    glyout->addWidget(WDFunc::NewLBL(parent, "U0_Gr2"), 4, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1120", ValuesFormat, "Сигнал нулевой последовательности"), 5, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U1_Gr2"), 4, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1121", ValuesFormat, "Сигнал прямой последовательности"), 5, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "U2_Gr2"), 4, 2, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1122", ValuesFormat, "Сигнал обратной последовательности"), 5, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim0_Gr2"), 4, 3, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1123", ValuesFormat, "Коэфф. несимметрии по нулевой посл."), 5, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Kunsim2_Gr2"), 4, 4, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "1124", ValuesFormat, "Коэфф. несимметрии по обратной посл."), 5, 4, 1, 1);

    for (i = 0; i < 3; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "THD_Gr2 ф." + phase[i]), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", "THD_Gr2" + QString::number(i), ValuesFormat,
                              "Общий коэфф. гармонических искажений"),
            7, i, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKDV::Bd4W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "A", "B", "C", "ср." };
    QFont font;
    int i;

    QGroupBox *gb = new QGroupBox("Мощности");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    for (i = 0; i < 4; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "PNatf ф." + phase[i]), 0, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2420 + i), ValuesFormat, "Истинная активная мощность"), 1, i, 1,
            1);
        glyout->addWidget(WDFunc::NewLBL(parent, "QNatf ф." + phase[i]), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2424 + i), ValuesFormat,
                              "Реактивная мощность по кажущейся полной и истинной активной"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "SNatf ф." + phase[i]), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2428 + i), ValuesFormat,
                              "Кажущаяся полная мощность по эфф. токам и напряжениям"),
            5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "CosPhiNat ф." + phase[i]), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2432 + i), ValuesFormat,
                              "Cos phi по истинной активной мощности"),
            7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Pf ф." + phase[i]), 8, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2436 + i), ValuesFormat, "Активная мощность по 1-й гарм."), 9,
            i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Qf ф." + phase[i]), 10, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2440 + i), ValuesFormat, "Реактивная мощность по 1-й гарм."),
            11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Sf ф." + phase[i]), 12, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2444 + i), ValuesFormat, "Полная мощность по по 1-й гарм."), 13,
            i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(parent, "CosPhi ф." + phase[i]), 14, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(2448 + i), ValuesFormat, "Cos phi по 1-й гарм."), 15, i, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKDV::Bd5W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "Фаза A", "Фаза B", "Фаза C" };
    QFont font;
    int j, i = 0;

    QScrollArea *area = new QScrollArea;
    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    QGroupBox *gb = new QGroupBox("Гармоники");
    QGroupBox *gbIn = new QGroupBox("Группа №1");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    gbIn->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QHBoxLayout *hlyout1 = new QHBoxLayout;

    for (j = 0; j < 3; ++j)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, phase[j]), 0, j, 1, 1, Qt::AlignCenter);
    }

    for (j = 1; j < 62; ++j)
    {
        i = 2 + 2 * (j - 1);
        glyout->addWidget(
            WDFunc::NewLBL(parent, "Bda_Gr1.Harm[0][" + QString::number(j - 1) + "]"), 1 + 2 * (j - 1), 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(3020 + j - 1), ValuesFormat, ""), i, 0, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBL(parent, "Bda_Gr1.Harm[1][" + QString::number(j - 1) + "]"), 1 + 2 * (j - 1), 1, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(3146 + j - 1), ValuesFormat, ""), i, 1, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBL(parent, "Bda_Gr1.Harm[2][" + QString::number(j - 1) + "]"), 1 + 2 * (j - 1), 2, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(3270 + j - 1), ValuesFormat, ""), i, 2, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gbIn->setLayout(vlyout2);
    hlyout1->addWidget(gbIn);

    gbIn = new QGroupBox("Группа №2");
    font.setFamily("Times");
    font.setPointSize(11);
    gbIn->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    for (j = 0; j < 3; ++j)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, phase[j]), 0, j + 3, 1, 1, Qt::AlignCenter);
    }

    for (j = 1; j < 62; ++j)
    {
        i = 2 + 2 * (j - 1);
        glyout->addWidget(
            WDFunc::NewLBL(parent, "Bda_Gr2.Harm[0][" + QString::number(j - 1) + "]"), 1 + 2 * (j - 1), 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(3420 + j - 1), ValuesFormat, ""), i, 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBL(parent, "Bda_Gr2.Harm[1][" + QString::number(j - 1) + "]"), 1 + 2 * (j - 1), 4, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(3546 + j - 1), ValuesFormat, ""), i, 4, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBL(parent, "Bda_Gr2.Harm[2][" + QString::number(j - 1) + "]"), 1 + 2 * (j - 1), 5, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(3670 + j - 1), ValuesFormat, ""), i, 5, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gbIn->setLayout(vlyout2);
    hlyout1->addWidget(gbIn);
    gb->setLayout(hlyout1);
    area->setWidget(gb);
    lyout->addWidget(area);
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    area->verticalScrollBar()->setMinimumHeight(450);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKDV::Bd6W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "A", "B", "C", "ср." };
    QFont font;

    QGroupBox *gb = new QGroupBox("Ресурс");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "Vst"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "4000", ValuesFormat, "Относительная скорость старения изоляции, час/час"), 1, 0, 1,
        1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Age"), 0, 1, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "4001", ValuesFormat, "Интегральное старение изоляции, час"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Resurs"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "4002", ValuesFormat, "Остаточный ресурс изоляции, час"), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Tpred"), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "4010", ValuesFormat,
                          "Оставшееся время работы до предупреждения при данной нагрузке"),
        1, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "Tdop"), 0, 4, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "4011", ValuesFormat, "Допустимое оставшееся время работы при данной нагрузке"), 1,
        4, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKDV::Bd7W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "A", "B", "C", "ср." };
    QFont font;
    int i;
    QScrollArea *area = new QScrollArea;
    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    QGroupBox *gb = new QGroupBox("Модель");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;

    for (i = 0; i < 20; i++)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "Tdop" + QString::number(105 + i * 5)), 2 * i, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4012 + i), ValuesFormat,
                              "Допустимое оставшееся время работы при нагрузке " + QString::number(105 + i * 5) + "%"),
            2 * i + 1, 0, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    area->setWidget(gb);
    lyout->addWidget(area);
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    area->verticalScrollBar()->setMinimumHeight(450);

    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKDV::Bd8W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = { "A", "B", "C", "ср." };
    QFont font;
    int i;

    QGroupBox *gb = new QGroupBox("Температура");
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);
    // gb->setTitle(title);
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(parent, "TempWinding"), 0, 0, 1, 1);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "4500", ValuesFormat, "Температура ННТ обмотки (расчетная)"), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "temperature"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "4501", ValuesFormat, "Температура окружающей среды"), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "TempWinding"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "4502", ValuesFormat, "Сопротивление термометра, Ом"), 1, 2, 1, 1);

    for (i = 0; i < 4; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(parent, "ExtTempWin[" + QString::number(i) + "]"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4503 + i), ValuesFormat,
                              "Температуры обмоток, измеренные встроенными датчиками(до 4шт.)"),
            3, i, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    lyout->addLayout(vlyout1);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void CheckKDV::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetLBLText(parent, Name, Value);
}

void CheckKDV::FillBda_block(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Frequency", WDFunc::StringValueWithCheck(Bda_block.Frequency, 3));

    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLBLText(
            parent, "IUefNat_filt" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.IUefNat_filt[i], 3));
        WDFunc::SetLBLText(parent, "IUefNat_filt" + QString::number(i + 3),
            WDFunc::StringValueWithCheck(Bda_block.IUefNat_filt[i + 3], 3));
        WDFunc::SetLBLText(parent, "UefNatLin_filt" + QString::number(i),
            WDFunc::StringValueWithCheck(Bda_block.UefNatLin_filt[i], 3));
        WDFunc::SetLBLText(parent, "IUeff_filtered" + QString::number(i),
            WDFunc::StringValueWithCheck(Bda_block.IUeff_filtered[i], 3));
        WDFunc::SetLBLText(parent, "IUeff_filtered" + QString::number(i + 3),
            WDFunc::StringValueWithCheck(Bda_block.IUeff_filtered[i + 3], 3));
        WDFunc::SetLBLText(
            parent, "phi_next_f" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.phi_next_f[i], 3));
        WDFunc::SetLBLText(parent, "phi_next_f" + QString::number(i + 3),
            WDFunc::StringValueWithCheck(Bda_block.phi_next_f[i + 3], 3));
        WDFunc::SetLBLText(parent, "PNatf" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.PNatf[i], 3));
        WDFunc::SetLBLText(parent, "QNatf" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.QNatf[i], 3));
        WDFunc::SetLBLText(parent, "SNatf" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.SNatf[i], 3));
        WDFunc::SetLBLText(
            parent, "CosPhiNat" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.CosPhiNat[i], 3));
        WDFunc::SetLBLText(parent, "Pf" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.Pf[i], 3));
        WDFunc::SetLBLText(parent, "Qf" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.Qf[i], 3));
        WDFunc::SetLBLText(parent, "Sf" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.Sf[i], 3));
        WDFunc::SetLBLText(parent, "CosPhi" + QString::number(i), WDFunc::StringValueWithCheck(Bda_block.CosPhi[i], 3));
    }

    WDFunc::SetLBLText(parent, "Pt100_R", WDFunc::StringValueWithCheck(Bda_block.Pt100_R, 3));
}
void CheckKDV::FillBd2_Gr1(QWidget *parent)
{

    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(
            parent, QString::number(1000 + i), WDFunc::StringValueWithCheck(Bd2_Gr1.GrIUefNat_filt[i], 3));
        WDFunc::SetLBLText(
            parent, QString::number(1004 + i), WDFunc::StringValueWithCheck(Bd2_Gr1.GrIUeff_filtered[i], 3));
    }

    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(1008 + i), WDFunc::StringValueWithCheck(Bd2_Gr1.KrF[i], 3));
    }
}
void CheckKDV::FillBd3_Gr2(QWidget *parent)
{
    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(
            parent, QString::number(1100 + i), WDFunc::StringValueWithCheck(Bd3_Gr2.GrIUefNat_filt[i], 3));
        WDFunc::SetLBLText(
            parent, QString::number(1104 + i), WDFunc::StringValueWithCheck(Bd3_Gr2.GrIUeff_filtered[i], 3));
    }

    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(1108 + i), WDFunc::StringValueWithCheck(Bd3_Gr2.KrF[i], 3));
    }
}
void CheckKDV::FillBd4_U0(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "1020", WDFunc::StringValueWithCheck(Bd4_U0.U0, 3));
    WDFunc::SetLBLText(parent, "1021", WDFunc::StringValueWithCheck(Bd4_U0.U1, 3));
    WDFunc::SetLBLText(parent, "1022", WDFunc::StringValueWithCheck(Bd4_U0.U2, 3));
    WDFunc::SetLBLText(parent, "1023", WDFunc::StringValueWithCheck(Bd4_U0.Kunsim0, 3));
    WDFunc::SetLBLText(parent, "1024", WDFunc::StringValueWithCheck(Bd4_U0.Kunsim2, 3));

    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLBLText(parent, "THD_Gr1" + QString::number(i), WDFunc::StringValueWithCheck(Bd4_U0.THD[i], 3));
    }
}
void CheckKDV::FillBd5_UI_Harm(QWidget *parent)
{
    for (int i = 0; i < 61; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(3020 + i), WDFunc::StringValueWithCheck(Bd5_UI_Harm.Harm[0][i], 3));
        WDFunc::SetLBLText(parent, QString::number(3146 + i), WDFunc::StringValueWithCheck(Bd5_UI_Harm.Harm[1][i], 3));
        WDFunc::SetLBLText(parent, QString::number(3270 + i), WDFunc::StringValueWithCheck(Bd5_UI_Harm.Harm[2][i], 3));
    }
}
void CheckKDV::FillBd6_U0(QWidget *parent)
{

    WDFunc::SetLBLText(parent, "1120", WDFunc::StringValueWithCheck(Bd6_U0.U0, 3));
    WDFunc::SetLBLText(parent, "1121", WDFunc::StringValueWithCheck(Bd6_U0.U1, 3));
    WDFunc::SetLBLText(parent, "1122", WDFunc::StringValueWithCheck(Bd6_U0.U2, 3));
    WDFunc::SetLBLText(parent, "1123", WDFunc::StringValueWithCheck(Bd6_U0.Kunsim0, 3));
    WDFunc::SetLBLText(parent, "1124", WDFunc::StringValueWithCheck(Bd6_U0.Kunsim2, 3));

    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLBLText(parent, "THD_Gr2" + QString::number(i), WDFunc::StringValueWithCheck(Bd6_U0.THD[i], 3));
    }
}
void CheckKDV::FillBd7_UI_Harm(QWidget *parent)
{
    for (int i = 0; i < 61; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(3420 + i), WDFunc::StringValueWithCheck(Bd7_UI_Harm.Harm[0][i], 3));
        WDFunc::SetLBLText(parent, QString::number(3546 + i), WDFunc::StringValueWithCheck(Bd7_UI_Harm.Harm[1][i], 3));
        WDFunc::SetLBLText(parent, QString::number(3670 + i), WDFunc::StringValueWithCheck(Bd7_UI_Harm.Harm[2][i], 3));
    }
}
void CheckKDV::FillBd9_Resurs(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "4000", WDFunc::StringValueWithCheck(Bd9_Resurs.Vst, 3));
    WDFunc::SetLBLText(parent, "4001", WDFunc::StringValueWithCheck(Bd9_Resurs.Age, 3));
    WDFunc::SetLBLText(parent, "4002", WDFunc::StringValueWithCheck(Bd9_Resurs.Resurs, 3));
}
void CheckKDV::FillBd10_Power(QWidget *parent)
{
    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(2420 + i), WDFunc::StringValueWithCheck(Bd10_Power.PNatf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2424 + i), WDFunc::StringValueWithCheck(Bd10_Power.QNatf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2428 + i), WDFunc::StringValueWithCheck(Bd10_Power.SNatf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2432 + i), WDFunc::StringValueWithCheck(Bd10_Power.CosPhiNat[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2436 + i), WDFunc::StringValueWithCheck(Bd10_Power.Pf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2440 + i), WDFunc::StringValueWithCheck(Bd10_Power.Qf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2444 + i), WDFunc::StringValueWithCheck(Bd10_Power.Sf[i], 3));
        WDFunc::SetLBLText(parent, QString::number(2448 + i), WDFunc::StringValueWithCheck(Bd10_Power.CosPhi[i], 3));
    }
}
void CheckKDV::FillBd11_Model(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "4010", WDFunc::StringValueWithCheck(Bd11_Model.Tpred, 3));
    WDFunc::SetLBLText(parent, "4011", WDFunc::StringValueWithCheck(Bd11_Model.Tdop, 3));
    WDFunc::SetLBLText(parent, "4012", WDFunc::StringValueWithCheck(Bd11_Model.Tdop105, 3));
    WDFunc::SetLBLText(parent, "4013", WDFunc::StringValueWithCheck(Bd11_Model.Tdop110, 3));
    WDFunc::SetLBLText(parent, "4014", WDFunc::StringValueWithCheck(Bd11_Model.Tdop115, 3));
    WDFunc::SetLBLText(parent, "4015", WDFunc::StringValueWithCheck(Bd11_Model.Tdop120, 3));
    WDFunc::SetLBLText(parent, "4016", WDFunc::StringValueWithCheck(Bd11_Model.Tdop125, 3));
    WDFunc::SetLBLText(parent, "4017", WDFunc::StringValueWithCheck(Bd11_Model.Tdop130, 3));
    WDFunc::SetLBLText(parent, "4018", WDFunc::StringValueWithCheck(Bd11_Model.Tdop135, 3));
    WDFunc::SetLBLText(parent, "4019", WDFunc::StringValueWithCheck(Bd11_Model.Tdop140, 3));
    WDFunc::SetLBLText(parent, "4020", WDFunc::StringValueWithCheck(Bd11_Model.Tdop145, 3));
    WDFunc::SetLBLText(parent, "4021", WDFunc::StringValueWithCheck(Bd11_Model.Tdop150, 3));
    WDFunc::SetLBLText(parent, "4022", WDFunc::StringValueWithCheck(Bd11_Model.Tdop155, 3));
    WDFunc::SetLBLText(parent, "4023", WDFunc::StringValueWithCheck(Bd11_Model.Tdop160, 3));
    WDFunc::SetLBLText(parent, "4024", WDFunc::StringValueWithCheck(Bd11_Model.Tdop165, 3));
    WDFunc::SetLBLText(parent, "4025", WDFunc::StringValueWithCheck(Bd11_Model.Tdop170, 3));
    WDFunc::SetLBLText(parent, "4026", WDFunc::StringValueWithCheck(Bd11_Model.Tdop175, 3));
    WDFunc::SetLBLText(parent, "4027", WDFunc::StringValueWithCheck(Bd11_Model.Tdop180, 3));
    WDFunc::SetLBLText(parent, "4028", WDFunc::StringValueWithCheck(Bd11_Model.Tdop185, 3));
    WDFunc::SetLBLText(parent, "4029", WDFunc::StringValueWithCheck(Bd11_Model.Tdop190, 3));
    WDFunc::SetLBLText(parent, "4030", WDFunc::StringValueWithCheck(Bd11_Model.Tdop195, 3));
    WDFunc::SetLBLText(parent, "4031", WDFunc::StringValueWithCheck(Bd11_Model.Tdop200, 3));
}
void CheckKDV::FillBd13_Temp(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "4500", WDFunc::StringValueWithCheck(Bd13_Temp.TempWinding, 3));
    WDFunc::SetLBLText(parent, "4501", WDFunc::StringValueWithCheck(Bd13_Temp.temperature, 3));
    WDFunc::SetLBLText(parent, "4502", WDFunc::StringValueWithCheck(Bd13_Temp.resistance, 3));

    for (int i = 0; i < 4; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(4503 + i), WDFunc::StringValueWithCheck(Bd13_Temp.ExtTempWin[i], 3));
    }
}

QWidget *CheckKDV::BdUI(int bdnum)
{
    Q_UNUSED(bdnum);
    return nullptr;
}

void CheckKDV::SetDefaultValuesToWrite()
{
}

void CheckKDV::PrepareAnalogMeasurements()
{
}

void CheckKDV::ChooseValuesToWrite()
{
}

void CheckKDV::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}

void CheckKDV::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row);
}

void CheckKDV::RefreshAnalogValues(int bdnum)
{
    Q_UNUSED(bdnum);
}

QWidget *CheckKDV::CustomTab()
{
    QWidget *w = new QWidget;

    return w;
}
