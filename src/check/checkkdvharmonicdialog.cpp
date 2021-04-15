#include "checkkdvharmonicdialog.h"

#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../module/modules.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTime>
#include <QVBoxLayout>
#include <QtMath>

CheckKDVHarmonicDialog::CheckKDVHarmonicDialog(QWidget *parent) : AbstractCheckDialog(parent)
{

//    QString tmps = "QDialog {background-color: " + QString(Colors::UCONFCLR) + ";}";
//    setStyleSheet(tmps);
//    QStringList sl;

    m_BdUIList = { { "Гармоники U 2-11", BdUWidget(3020, 2, 12) }, { "Гармоники U 12-21", BdUWidget(3030, 12, 22) },
        { "Гармоники U 22-31", BdUWidget(3040, 22, 32) }, { "Гармоники U 32-41", BdUWidget(3050, 32, 42) },
        { "Гармоники U 42-51", BdUWidget(3060, 42, 52) }, { "Гармоники U 52-62", BdUWidget(3070, 52, 63) },
        { "Гармоники I 2-11", BdIWidget(3420, 2, 12) }, { "Гармоники I 12-21", BdIWidget(3430, 12, 22) },
        { "Гармоники I 22-31", BdIWidget(3440, 22, 32) }, { "Гармоники I 32-41", BdIWidget(3450, 32, 42) },
        { "Гармоники I 42-51", BdIWidget(3460, 42, 52) }, { "Гармоники I 52-62", BdIWidget(3470, 52, 63) } };
    m_BdUIList.first().widget->setUpdatesEnabled();
    Timer->setInterval(ANMEASINT);
}

void CheckKDVHarmonicDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void CheckKDVHarmonicDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

UWidget *CheckKDVHarmonicDialog::BdIWidget(unsigned int value, int min, int max)
{
    int i;
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = { "ф.A ", "ф.B ", "ф.C " };

    //...................................

    QFont font;
    QGroupBox *gb = new QGroupBox;
    font.setFamily("Times");
    font.setPointSize(11);
    gb->setFont(font);


    gb = new QGroupBox("Данные по гармоническому составу напряжения");
    gb->setFont(font);
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
            glyout->addWidget(WDFunc::NewLBL(this, phase[i] + "Гм. " + QString::number(h)), s, i, 1, 1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", QString::number((value + j) + i * 126), ValuesFormat,
                                  QString::number(h) + "-я гармоника тока в фазе" + phase[i] + "в % от 1-й", true),
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
    w->setFloatBdQuery({ { 3020, 61 }, { 3146, 61 }, { 3272, 61 } });
    return w;
}

UWidget *CheckKDVHarmonicDialog::BdUWidget(unsigned int value, int min, int max)
{
    int i;
    UWidget *w = new UWidget;
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
            glyout->addWidget(WDFunc::NewLBL(this, phase[i] + "Гм. " + QString::number(h)), s, i, 1, 1);
            glyout->addWidget(
                WDFunc::NewLBLT(this, "", QString::number((value + j) + i * 126), ValuesFormat,
                    QString::number(h) + "-я гармоника напряжения в фазе" + phase[i] + "в % от 1-й", true),
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
    w->setFloatBdQuery({ { 3420, 61 }, { 3546, 61 }, { 3672, 61 } });
    return w;
}

