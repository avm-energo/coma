#include <QVBoxLayout>
#include "connectdialog.h"
#include "../gen/maindef.h"
#include "../widgets/wd_func.h"

ConnectDialog::ConnectDialog()
{
    QByteArray ba;
    QStringList intersl = QStringList() << "USB" << "Ethernet" << "RS485";
    setMinimumWidth(150);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Выберите интерфейс связи"));
    lyout->addWidget(WDFunc::NewCB(this, "intercb", intersl));
    WDFunc::CBConnect(this, "intercb", WDFunc::CT_TEXTCHANGED,this,SLOT(ParseInter(QString)));
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, &QPushButton::clicked,this,&ConnectDialog::SetInterface);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, &QPushButton::clicked,this,&ConnectDialog::Cancelled);
    connect(pb, SIGNAL(clicked(bool)),this, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

void ConnectDialog::ParseString(QString str)
{
    QStringList sl = Str.split(" ");

    if (sl.size() < 3)
    {
        EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        ERMSG("Некорректная запись в файле");
        return;
    }
    if ((sl.at(1) == "ETH") && (MainInterface == I_ETHERNET))
    {
       FullName = sl.at(0);
       AdrBaseStation = sl.at(2).toUShort();
       IPtemp = Str.split(" ").last();
    }
    else if ((sl.at(1) == "MODBUS") && (MainInterface == I_RS485))
    {
        if(sl.size() > 6)
        {
            bool ok;
            FullName = sl.at(0);
            Settings.Baud =  sl.at(2).toUInt(&ok);
            if (!ok)
            {
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                ERMSG("Некорректная запись в файле");
                return;
            }
            Settings.Parity =  sl.at(3);
            Settings.Stop =  sl.at(4);
            Settings.Address =  sl.at(5).toUInt(&ok);
            if (!ok)
            {
                ERMSG("Некорректная запись в файле");
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                return;
            }
            Settings.Port =  sl.at(6);
        }
        else
        {
            EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        }
    }
}

void ConnectDialog::ParseInter()
{
    QString tmps = WDFunc::CBData(this, "intercb");
    if (tmps == "USB")
        MainInterface = I_USB;
    else if (tmps == "Ethernet")
        MainInterface = I_ETHERNET;
    else if (tmps == "RS485")
        MainInterface = I_RS485;
    else
        MainInterface = I_UNKNOWN;
}

void ConnectDialog::SetInterface(QString inttype)
{
    QByteArray ba;
    int res;
    QString Str;

    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QStringListModel *tmpmodel = new QStringListModel;

    if(MainInterface == I_USB)
    {
        USBsl = cn->DevicesFound();
        if (USBsl.size() == 0)
        {
            lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
            Error::ShowErMsg(CN_NOPORTSERROR);
        }
        tmpmodel->deleteLater();
        tmpmodel->setStringList(USBsl);
        QComboBox *portscb = new QComboBox;
        connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPortSlot(QString)));
        portscb->setModel(tmpmodel);
        lyout->addWidget(portscb);
    }
    else
    {
        if(!HaveAlreadyRed)
        {
            sl.clear();
            res= Files::LoadFromFile(Files::ChooseFileForOpen(this, "IP files (*.txt)"), ba);
            if (res != Files::ER_NOERROR)
            {
               WARNMSG("Ошибка при загрузке файла");
               return;
            }
            Str = ba;
            sl.append(Str.split("\r\n"));
            HaveAlreadyRed = 1;
        }
        if (sl.size() == 0)
        {
            lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
            Error::ShowErMsg(CN_NOPORTSERROR);
            ERMSG("Ошибка, устройства не найдены");
        }
        tmpmodel = new QStringListModel;
        tmpmodel->setStringList(sl);

        QComboBox *portscb = new QComboBox;
        connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(ParseString(QString)));
        portscb->setModel(tmpmodel);
        lyout->addWidget(portscb);
    }
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)),this,SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}
