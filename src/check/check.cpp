#include "check.h"

#include "../gen/colors.h"
#include "../widgets/wd_func.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStringListModel>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

Check::Check() { }

QWidget *Check::Bd0W(QWidget *parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. Tmk, °С:"), 0, 0);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "Bd00", valuesLBLSS, "Температура кристалла микроконтроллера, °С"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. VBAT, В:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "Bd01", valuesLBLSS, "Напряжение аккумуляторной батареи, В"), 0, 3);

    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet("QWidget {background-color: " + QString(UCONFCLR) + ";}");
    return w;
}

void Check::FillBd0(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bd00", WDFunc::StringValueWithCheck(Bd_block0.Tmk));
    WDFunc::SetLBLText(parent, "Bd01", WDFunc::StringValueWithCheck(Bd_block0.Vbat));
}
