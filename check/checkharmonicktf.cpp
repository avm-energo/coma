
#include "checkharmonicktf.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QLayout>

CheckHarmonicKTF::CheckHarmonicKTF()
{
    // setAttribute(Qt::WA_DeleteOnClose);
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(Colors::ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(Colors::UCONFCLR) + ";}";
}

QWidget *CheckHarmonicKTF::BdIWidget(QWidget *parent, int value , int min, int max)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox;
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Данные по гармоническому составу тока");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    int j, k, s;
    for (i = 0; i < 3; ++i)
    {
        j = 0;
        s = 0;
        k = 0;
        for (int h = min; h < max; h++)
        {
            if (k > 0)
                s = s + 2;
            k = s + 1;
            glyout->addWidget(WDFunc::NewLBL(parent, phase[i] + "Гм. " + QString::number(h)), s, i, 1, 1);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number((value + j) + i * 61), ValuesFormat,
                                  h + "-я гармоника тока в фазе" + phase[i] + "в % от 1-й", true),
                k, i, 1, 1);
            j++;
        }
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

QWidget *CheckHarmonicKTF::BdUWidget(QWidget *parent, int value, int min, int max)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox;
    ffont.setFamily("Times");
    ffont.setPointSize(11);
    gb->setFont(ffont);

    gb = new QGroupBox("Данные по гармоническому составу напряжения");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;

    int j, k, s;
    for (i = 0; i < 3; ++i)
    {
        j = 0;
        s = 0;
        k = 0;
        for (int h = min; h < max; h++)
        {
            if (k > 0)
                s = s + 2;
            k = s + 1;
            glyout->addWidget(WDFunc::NewLBL(parent, phase[i] + "Гм. " + QString::number(h)), s, i, 1, 1);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number((value + j) + i * 61), ValuesFormat,
                                  h + "-я гармоника напряжения в фазе" + phase[i] + "в % от 1-й", true),
                k, i, 1, 1);
            j++;
        }
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

void CheckHarmonicKTF::FillBd5(QWidget *parent)
{
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 61; i++)
        {
            WDFunc::SetLBLText(parent, QString::number(1028 + i + 61 * j),
                WDFunc::StringValueWithCheck(Bd_block5.UI_Harm[j][i], 3, true));
        }
    }
}
void CheckHarmonicKTF::FillBd7(QWidget *parent)
{
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 61; i++)
        {

            WDFunc::SetLBLText(parent, QString::number(1428 + i + 61 * j),
                WDFunc::StringValueWithCheck(Bd_block7.UI_Harm[j][i], 3, true));
        }
    }
}

void CheckHarmonicKTF::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetLBLText(parent, Name, Value);
}
