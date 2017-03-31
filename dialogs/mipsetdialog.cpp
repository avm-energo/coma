#include "mipsetdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "../publicclass.h"

MipSetDialog::MipSetDialog(QWidget *parent) :
    QDialog(parent)
{
    SetupUI();
}

void MipSetDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("IP-адрес МИП:");
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
    lyout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    lbl = new QLabel("ASDU:");
    hlyout->addWidget(lbl);
    s_tqSpinBox *spb = new s_tqSpinBox;
    spb->setDecimals(0);
    spb->setMinimum(1);
    spb->setMaximum(65534);
    spb->setValue(pc.MIPASDU);
    spb->setObjectName("asduspb");
    hlyout->addWidget(spb);
    hlyout->addStretch(90);
    lyout->addLayout(hlyout);

    QPushButton *pb = new QPushButton("Готово");
    pb->setObjectName("dlgpb");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetMipConPar()));
    lyout->addWidget(pb);
    setLayout(lyout);
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
}

void MipSetDialog::SetMipConPar()
{
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
