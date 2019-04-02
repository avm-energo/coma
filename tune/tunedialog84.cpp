#include <QTime>
#include <QtMath>
#include <QInputDialog>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QCoreApplication>
#include "tunedialog84.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/maindef.h"
#include "../check/check84.h"
#include "../gen/maindef.h"
#include "../gen/modulebsi.h"
#include "../gen/files.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

TuneDialog84::TuneDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent) : EAbstractTuneDialog(parent)
{
    this->S2ConfigForTune = &S2Config;

    C84 = new Config84(*S2ConfigForTune);
    //ReportModel = new QStandardItemModel;
    //ViewModel = new QStandardItemModel;

    //SetBac(&Bac_block, BoardTypes::BT_NONE, sizeof(Bac_block));

    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}


void TuneDialog84::SetupUI()
{
}
