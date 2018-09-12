#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include "confdialog87.h"
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

ConfDialog87::ConfDialog87(QVector<S2::DataRec> &S2Config, bool BaseBoard, QWidget *parent) :
    AbstractConfDialog(parent)
{
    //RangeInMins = {4.0, 0.0, 0.0, -5.0, 0.0};
    //RangeInMaxs = {20.0, 20.0, 5.0, 5.0, 5.0};
    this->S2Config = &S2Config;
    C87 = new Config87(S2Config, BaseBoard);
    Params.InTypes = QStringList() << "Не исп." << "мА" << "В";
    Params.RangeTypes = QStringList() << "(4..20) мА" << "(0..20) мА" << "(0..5) мА" << \
                                         "(-5..5) В" << "(0..5) В" << "Произвольный мА" << "Произвольный В";
    Params.NumCh = CHR87_NUMCH;
    SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    PrereadConf();
#endif
}

