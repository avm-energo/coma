#include "check.h"

#include "../widgets/wd_func.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStringListModel>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
// #include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/maindef.h"
#include "../gen/s2util.h"
#include "../gen/stdfunc.h"
#include "../widgets/emessagebox.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

Check::Check() { }

QWidget *Check::Bd0W(QWidget *parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                           "background-color: "
        + QString(ACONFOCLR) + "; font: bold 10px;}";
    QWidget *w = new QWidget(parent);
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(parent, "1. Tmk, °С:"), 0, 0);
    glyout->addWidget(
        WDFunc::NewLBLT(parent, "", "Bd00", ValuesFormat, "Температура кристалла микроконтроллера, °С"), 0, 1);
    glyout->addWidget(WDFunc::NewLBL(parent, "2. VBAT, В:"), 0, 2);
    glyout->addWidget(WDFunc::NewLBLT(parent, "", "Bd01", ValuesFormat, "Напряжение аккумуляторной батареи, В"), 0, 3);

    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    QString tmps = "QWidget {background-color: " + QString(UCONFCLR) + ";}";
    w->setStyleSheet(tmps);
    return w;
}

void Check::FillBd0(QWidget *parent)
{
    WDFunc::SetLBLText(parent, "Bd00", WDFunc::StringValueWithCheck(Bd_block0.Tmk));
    WDFunc::SetLBLText(parent, "Bd01", WDFunc::StringValueWithCheck(Bd_block0.Vbat));
}
