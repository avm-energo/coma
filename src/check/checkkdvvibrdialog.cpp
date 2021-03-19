#include "checkkdvvibrdialog.h"

#include "../gen/datamanager.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

CheckKDVVibrDialog::CheckKDVVibrDialog(QWidget *parent) : AbstractCheckDialog(parent)
{
    m_BdUIList
        = { /*{ "Вибрации", Bd1W() },*/ { "Среднеквадратичные значения", Bd2W() }, { "Скорости изменения", Bd3W() } };
    m_BdUIList.first().widget->setUpdatesEnabled();

    Timer->setInterval(ANMEASINT);
}

void CheckKDVVibrDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKDVVibrDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

// Bd1W shouldnt be here, only for AVM/AVTUK-Debug
UWidget *CheckKDVVibrDialog::Bd1W()
{
    int i;
    int row, column;
    UWidget *w = new UWidget();
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QGroupBox *gb = new QGroupBox("Среднеквадратичные значения вибросигналов, Вольт");

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(60 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(63 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 60, 6 } });
    return w;
}

UWidget *CheckKDVVibrDialog::Bd2W()
{
    int i;
    int row, column;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;

    QGridLayout *glyout = new QGridLayout;

    //...................................

    QGroupBox *gb = new QGroupBox("Среднеквадратичные значения виброускорений, м/с2");

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6040 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6043 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    gb->setLayout(glyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Среднеквадратичные значения виброскоростей, мм/с");

    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6046 + i), ValuesFormat,
                "Среднеквадратичное значение виброскорости по каналу   " + QString::number(1 + i) + ", мм/с"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6049 + i), ValuesFormat,
                "Среднеквадратичное значение виброскорости по каналу " + QString::number(4 + i) + ", мм/с"),
            row, column, 1, 1);
        column++;
    }

    gb->setLayout(glyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Среднеквадратичные значения виброперемещений, мкм");

    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6052 + i), ValuesFormat,
                "Среднеквадратичное значение виброперемещения по каналу " + QString::number(1 + i) + ", мкм"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(this, "", QString::number(6055 + i), ValuesFormat,
                "Среднеквадратичное значение виброперемещения по каналу " + QString::number(4 + i) + ", мкм"),
            row, column, 1, 1);
        column++;
    }

    gb->setLayout(glyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 6040, 36 } });
    return w;
}

UWidget *CheckKDVVibrDialog::Bd3W()
{
    int i;
    int row, column;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;

    QGridLayout *glyout = new QGridLayout;

    //...................................

    QGroupBox *gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброускорения, м/с2/час");

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6060 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброускорения по каналу"
                                  + QString::number(1 + i) + ", м/с2/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6063 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброускорения по каналу"
                                  + QString::number(4 + i) + ", м/с2/час"),
            row, column, 1, 1);
        column++;
    }

    gb->setLayout(glyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброскорости, мм/с/час");

    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6066 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброскорости по каналу "
                                  + QString::number(1 + i) + ", мм/с/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6069 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброскорости по каналу  "
                                  + QString::number(4 + i) + ", мм/с/час"),
            row, column, 1, 1);
        column++;
    }

    gb->setLayout(glyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброперемещения, мкм/час");

    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6052 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброперемещения по каналу "
                                  + QString::number(1 + i) + ", мкм/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL2(this, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number(6055 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброперемещения по каналу "
                                  + QString::number(4 + i) + ", мкм/час"),
            row, column, 1, 1);
        column++;
    }

    gb->setLayout(glyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 6040, 36 } });
    return w;
}
