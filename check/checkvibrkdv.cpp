#include "checkvibrkdv.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QLayout>

CheckVibrKDV::CheckVibrKDV()
{
    // setAttribute(Qt::WA_DeleteOnClose);
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(UCONFCLR) + ";}";
}

QWidget *CheckVibrKDV::Bd1W(QWidget *parent)
{
    int i;
    int row, column;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    //    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Среднеквадратичные значения вибросигналов, Вольт");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(60 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(63 + i), ValuesFormat,
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
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckVibrKDV::Bd2W(QWidget *parent)
{
    int i;
    int row, column;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    //    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Среднеквадратичные значения виброускорений, м/с2");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6040 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6043 + i), ValuesFormat,
                              "Среднеквадратичное значение виброускорения по каналу " + QString::number(i) + ", м/с2"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Среднеквадратичные значения виброскоростей, мм/с");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(6046 + i), ValuesFormat,
                "Среднеквадратичное значение виброскорости по каналу   " + QString::number(1 + i) + ", мм/с"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(6049 + i), ValuesFormat,
                "Среднеквадратичное значение виброскорости по каналу " + QString::number(4 + i) + ", мм/с"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Среднеквадратичные значения виброперемещений, мкм");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(6052 + i), ValuesFormat,
                "Среднеквадратичное значение виброперемещения по каналу " + QString::number(1 + i) + ", мкм"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(
            WDFunc::NewLBLT(parent, "", QString::number(6055 + i), ValuesFormat,
                "Среднеквадратичное значение виброперемещения по каналу " + QString::number(4 + i) + ", мкм"),
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
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckVibrKDV::Bd3W(QWidget *parent)
{
    int i;
    int row, column;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброускорения, м/с2/час");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6060 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброускорения по каналу"
                                  + QString::number(1 + i) + ", м/с2/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6063 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброускорения по каналу"
                                  + QString::number(4 + i) + ", м/с2/час"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброскорости, мм/с/час");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6066 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброскорости по каналу "
                                  + QString::number(1 + i) + ", мм/с/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6069 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброскорости по каналу  "
                                  + QString::number(4 + i) + ", мм/с/час"),
            row, column, 1, 1);
        column++;
    }

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    gb = new QGroupBox("Скорость изменения среднеквадратичного значения виброперемещения, мкм/час");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 0;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(1 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6052 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброперемещения по каналу "
                                  + QString::number(1 + i) + ", мкм/час"),
            row, column, 1, 1);
        column++;
    }

    column = 0;
    for (i = 0; i < 3; i++)
    {
        row = 2;

        glyout->addWidget(WDFunc::NewLBL(parent, "Датчик №" + QString::number(4 + i)), row, column, 1, 1);
        row++;
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(6055 + i), ValuesFormat,
                              "Скорость изменения среднеквадратичного значения виброперемещения по каналу "
                                  + QString::number(4 + i) + ", мкм/час"),
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
    w->setStyleSheet(WidgetFormat);
    return w;
}

void CheckVibrKDV::FillBd19(QWidget *parent)
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLBLText(parent, QString::number(60 + i), WDFunc::StringValueWithCheck(Bd_block19.VibrSCZ[i], 3));
    }
}

void CheckVibrKDV::FillBd20(QWidget *parent)
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLBLText(
            parent, QString::number(6040 + i), WDFunc::StringValueWithCheck(Bd_block20.Vibr_SCZ_A[i], 3));
        WDFunc::SetLBLText(
            parent, QString::number(6046 + i), WDFunc::StringValueWithCheck(Bd_block20.Vibr_SCZ_V[i], 3));
        WDFunc::SetLBLText(
            parent, QString::number(6052 + i), WDFunc::StringValueWithCheck(Bd_block20.Vibr_SCZ_D[i], 3));
        WDFunc::SetLBLText(
            parent, QString::number(6060 + i), WDFunc::StringValueWithCheck(Bd_block20.Vibr_SCZ_A_temp[i], 3));
        WDFunc::SetLBLText(
            parent, QString::number(6066 + i), WDFunc::StringValueWithCheck(Bd_block20.Vibr_SCZ_V_temp[i], 3));
        WDFunc::SetLBLText(
            parent, QString::number(6072 + i), WDFunc::StringValueWithCheck(Bd_block20.Vibr_SCZ_D_temp[i], 3));
    }
}

void CheckVibrKDV::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetLBLText(parent, Name, Value);
}
