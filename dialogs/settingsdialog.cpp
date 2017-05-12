#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
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


    hlyout = new QHBoxLayout;
    lbl = new QLabel("IP-адрес МИП:");
    hlyout->addWidget(lbl);
    QLineEdit *le = new QLineEdit;
    le->setObjectName("mip1");
    hlyout->addWidget(le);
    lbl = new QLabel(".");
    hlyout->addWidget(lbl);
    le = new QLineEdit;
    le->setObjectName("mip2");
    hlyout->addWidget(le);
    lbl = new QLabel(".");
    hlyout->addWidget(lbl);
    le = new QLineEdit;
    le->setObjectName("mip3");
    hlyout->addWidget(le);
    lbl = new QLabel(".");
    hlyout->addWidget(lbl);
    le = new QLineEdit;
    le->setObjectName("mip4");
    hlyout->addWidget(le);
    vlyout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    lbl = new QLabel("ASDU:");
    hlyout->addWidget(lbl);
    spb = new s_tqSpinBox;
    spb->setDecimals(0);
    spb->setMinimum(1);
    spb->setMaximum(65534);
    spb->setValue(pc.MIPASDU);
    spb->setObjectName("asduspb");
    hlyout->addWidget(spb);
    hlyout->addStretch(90);
    vlyout->addLayout(hlyout);

    QString restring = "^[0-2]{0,1}[0-9]{1,2}$";
    QStringList tmpsl = pc.MIPIP.split(".");
    if (tmpsl.size()<4)
    {
        for (int i = tmpsl.size(); i < 4; i++)
            tmpsl.append("");
    }
    WDFunc::SetLEData(this,"mip1",tmpsl.at(0),restring);
    WDFunc::SetLEData(this,"mip2",tmpsl.at(1),restring);
    WDFunc::SetLEData(this,"mip3",tmpsl.at(2),restring);
    WDFunc::SetLEData(this,"mip4",tmpsl.at(3),restring);

    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    WDFunc::SetSPBData(this, "errdelayspb", pc.ErrWindowDelay);
    WDFunc::SetChBData(this, "showerrcb", pc.ShowErrWindow);
    QString restring = "^[0-2]{0,1}[0-9]{1,2}$";
    QStringList tmpsl = pc.MIPIP.split(".");
    if (tmpsl.size()<4)
    {
        for (int i = tmpsl.size(); i < 4; i++)
            tmpsl.append("");
    }
    WDFunc::SetLEData(this,"mip1",tmpsl.at(0),restring);
    WDFunc::SetLEData(this,"mip2",tmpsl.at(1),restring);
    WDFunc::SetLEData(this,"mip3",tmpsl.at(2),restring);
    WDFunc::SetLEData(this,"mip4",tmpsl.at(3),restring);
    WDFunc::SetSPBData(this, "asduspb", pc.MIPASDU);
}

void SettingsDialog::AcceptSettings()
{
    double erwindelay = static_cast<double>(pc.ErrWindowDelay);
    WDFunc::SPBData(this, "errdelayspb", erwindelay);
    WDFunc::ChBData(this, "showerrcb", pc.ShowErrWindow);
    pc.MIPIP.clear();
    QString tmps;
    for (int i = 1; i < 5; ++i)
    {
        WDFunc::LEData(this, "mip"+QString::number(i), tmps);
        pc.MIPIP += tmps+".";
    }
    pc.MIPIP.chop(1); // последнюю точку убираем
    double mipasdu = static_cast<double>(pc.MIPASDU);
    WDFunc::SPBData(this, "asduspb", mipasdu);
    this->close();
}
