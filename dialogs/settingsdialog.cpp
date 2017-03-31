#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QString>
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "../publicclass.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Настройки");
    SetupUI();
    Fill();
}

void SettingsDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Показывать окно ошибок при их появлении");
    hlyout->addWidget(lbl);
    QCheckBox *cb = new QCheckBox;
    cb->setObjectName("showerrcb");
    hlyout->addWidget(cb);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Задержка появления окна ошибок, с");
    hlyout->addWidget(lbl);
    s_tqSpinBox *spb = new s_tqSpinBox;
    spb->setObjectName("errdelayspb");
    spb->setDecimals(0);
    spb->setMinimum(1);
    spb->setMaximum(10);
    hlyout->addWidget(spb);
    vlyout->addLayout(hlyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    WDFunc::SetSPBData(this, "errdelayspb", pc.ErrWindowDelay);
    WDFunc::SetChBData(this, "showerrcb", pc.ShowErrWindow);
}

void SettingsDialog::AcceptSettings()
{
    double erwindelay = static_cast<double>(pc.ErrWindowDelay);
    WDFunc::SPBData(this, "errdelayspb", erwindelay);
    WDFunc::ChBData(this, "showerrcb", pc.ShowErrWindow);
    this->close();
}
