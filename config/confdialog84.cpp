#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include "../widgets/emessagebox.h"
#include "../widgets/ecombobox.h"
#include "../widgets/wd_func.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "confdialog84.h"


ConfDialog84::ConfDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    C84 = new Config84(S2Config);
     setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    PrereadConf();
#endif
}

ConfDialog84::~ConfDialog84()
{
}

void ConfDialog84::Fill()
{

}

void ConfDialog84::FillBack()
{
    int i,index;
    QString tmps;
    bool tmpb;

}

void ConfDialog84::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QVBoxLayout *vlyout3 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *cp1 = new QWidget;
    QWidget *analog = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    analog->setStyleSheet(tmps);

    hlyout->addWidget(WDFunc::NewLBL(this, "Тип контролируемого оборудования:"), 0);
    QStringList cbl = QStringList() << "1ф трансформатор/АТ" << "3ф трансформатор/АТ" << "1ф реактор" << "3ф реактор";
    EComboBox *cb = WDFunc::NewCB(this, "eq_typecb", cbl, ACONFWCLR);
    cb->setEditable(true);
    cb->setMinimumWidth(70);
    hlyout->addWidget(cb);
    vlyout1->addLayout(hlyout);

    QGroupBox *gb = new QGroupBox("Аналоговые параметры");
    //gb->updateGeometry();

    //gb->setLayout(vlyout3);
    analog->setLayout(vlyout3);

    gb = new QGroupBox("Осциллограммы");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Запуск осциллограммы:"));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", ACONFWCLR));
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Прочие");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Кол-во точек оцифровки:"));
    cbl = QStringList() << "64" << "80" << "128" << "256";
    hlyout->addWidget(WDFunc::NewCB(this, "npointscb", cbl, ACONFWCLR));
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени фильтрации:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "nfiltrspb", 1, 1000, 0, ACONFWCLR));
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени гармоник:"));
    hlyout->addWidget(WDFunc::NewSPB(this, "nhfiltrspb", 1, 100, 0, ACONFWCLR));
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    cp1->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(cp1,"Общие");
    ConfTW->addTab(analog,"Аналоговые");
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog84::CheckConf()
{
}




void ConfDialog84::SetDefConf()
{
    C84->SetDefConf();
}
