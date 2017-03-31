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
#include <QMessageBox>
#include <QCoreApplication>
#include <QFileDialog>
#include "../canal.h"
#include "../widgets/messagebox.h"
#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqspinbox.h"
#include "confdialog_80.h"

confdialog_80::confdialog_80(QWidget *parent) :
    QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    econf = new config_80;
    GeneralConf = new confdialog(&(econf->Bci_block.mainblk));
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    lyout->addWidget(ConfTW);
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QPushButton *pb1 = new QPushButton("Прочитать из модуля");
    connect(pb1,SIGNAL(clicked()),this,SLOT(GetBci()));
    if (pc.Emul)
        pb1->setEnabled(false);
    wdgtlyout->addWidget(pb1, 0, 0, 1, 1);
    QPushButton *pb = new QPushButton("Записать в модуль");
    pb->setObjectName("WriteConfPB");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConfDataToModule()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadConf()));
    wdgtlyout->addWidget(pb, 0, 2, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveConf()));
    wdgtlyout->addWidget(pb, 0, 3, 1, 1);
    pb = new QPushButton("Задать конфигурацию по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefConf()));
    wdgtlyout->addWidget(pb, 1, 0, 1, 2);
    pb = new QPushButton("Перейти на новую конфигурацию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetNewConf()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 1, 2, 1, 2);
    wdgt->setLayout(wdgtlyout);
    lyout->addWidget(wdgt);
    setLayout(lyout);
    SetupUI();
    if ((pc.ModuleBsi.Hth & HTH_CONFIG) || (pc.Emul)) // если установлен признак отсутствия конфигурации
        SetDefConf();
    else
        GetBci();
}

confdialog_80::~confdialog_80()
{
    delete GeneralConf;
}

void confdialog_80::GetBci()
{
    cn->Send(CN_GF,Canal::BT_NONE,NULL,0,1,econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
    {
        FillConfData();
        MessageBox2::information(this, "OK", "Прочитано успешно");
    }
}

void confdialog_80::FillConfData()
{
    int i;
    GeneralConf->Fill(); // заполнение общих полей
    SetComboBox("eq_typecb", QString::number(econf->Bci_block.eq_type));
    SetCheckBox("oscchb.0", (econf->Bci_block.ddosc & 0x0001));
    SetCheckBox("oscchb.1", (econf->Bci_block.ddosc & 0x0002));
    SetCheckBox("oscchb.2", (econf->Bci_block.ddosc & 0x0004));
    SetSpinBox("npointspb", econf->Bci_block.npoints);
    SetSpinBox("nhfiltrspb", econf->Bci_block.nhfiltr);
    switch (pc.ModuleBsi.MTypeM)
    {
    case MTM_81: // 2 напряжения, 0 токов
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        SetComboBox("unom.2", QString::number(econf->Bci_block.unom2));
        SetSpinBox("thr.1", econf->Bci_block.duosc);
       break;
    }
    case MTM_82:
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        for (i = 3; i < 6; i++)
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        SetSpinBox("thr.1", econf->Bci_block.duosc);
        SetSpinBox("thr.2", econf->Bci_block.diosc);
        break;
    }
    case MTM_83:
    {
        for (i = 0; i < 6; i++)
        {
            SetSpinBox("inom.1."+QString::number(i), econf->Bci_block.inom1[i]);
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        }
        SetSpinBox("thr.2", econf->Bci_block.diosc);
       break;
    }
    default:
        break;
    }
    SetSpinBox("thr.3", econf->Bci_block.duimin);
}

void confdialog_80::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    QTabWidget *ConfTW = this->findChild<QTabWidget *>("conftw");
    if (ConfTW == 0)
        return;
    ConfTW->addTab(cp1,"Общие");
    ConfTW->addTab(cp2,"104");

    QLabel *lbl = new QLabel ("Тип контролируемого оборудования:");
    hlyout->addWidget(lbl, 0);
    QStringList cbl = QStringList() << "1ф трансформатор/АТ" << "3ф трансформатор/АТ" << "1ф реактор" << "3ф реактор";
    QStringListModel *cblm = new QStringListModel;
    cblm->setStringList(cbl);
    s_tqComboBox *cb = new s_tqComboBox;
    cb->setObjectName("eq_typecb");
    cb->setModel(cblm);
    cb->setMinimumWidth(70);
    tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    cb->setStyleSheet(tmps);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    hlyout->addWidget(cb,10);
    vlyout1->addLayout(hlyout);

    QGroupBox *gb = new QGroupBox("Аналоговые");
    switch (pc.ModuleBsi.MTypeM)
    {
    case MTM_81: // 6U0I
    {
        vlyout2->addWidget(UNom(1));
        vlyout2->addWidget(UNom(2));
        vlyout2->addWidget(Threshold("Уставка скачка напряжения для запуска осциллографирования, %", 1));
        break;
    }
    case MTM_82: // 3U3I
    {
        vlyout2->addWidget(UNom(1));
        vlyout2->addWidget(INom(3), 10);
        vlyout2->addWidget(INom(4), 10);
        vlyout2->addWidget(Threshold("Уставка скачка напряжения для запуска осциллографирования, %", 1));
        vlyout2->addWidget(Threshold("Уставка скачка тока для запуска осциллографирования, %", 2));
        break;
    }
    case MTM_83: // 0U6I
    {
        vlyout2->addWidget(INom(1), 10);
        vlyout2->addWidget(INom(2), 10);
        vlyout2->addWidget(INom(3), 10);
        vlyout2->addWidget(INom(4), 10);
        vlyout2->addWidget(Threshold("Уставка скачка тока для запуска осциллографирования, %", 2));
        break;
    }
    default:
        break;
    }
    vlyout2->addWidget(Threshold("Уставка порога мин. уровня для определения частоты, %", 3));
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Осциллограммы");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Запуск осциллограммы:");
    hlyout->addWidget(lbl);
    s_tqCheckBox *chb = new s_tqCheckBox;
    chb->setObjectName("oscchb.0");
    chb->setText("по команде Ц");
    connect(chb,SIGNAL(statechanged(int)),this,SLOT(SetOsc(int)));
    hlyout->addWidget(chb);
    chb = new s_tqCheckBox;
    chb->setObjectName("oscchb.1");
    chb->setText("по дискр. входу PD1");
    connect(chb,SIGNAL(statechanged(int)),this,SLOT(SetOsc(int)));
    hlyout->addWidget(chb);
    chb = new s_tqCheckBox;
    chb->setObjectName("oscchb.2");
    chb->setText("по резкому изменению");
    connect(chb,SIGNAL(statechanged(int)),this,SLOT(SetOsc(int)));
    hlyout->addWidget(chb);
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Прочие");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Кол-во точек оцифровки:");
    hlyout->addWidget(lbl);
    cbl = QStringList() << "64" << "80" << "128" << "256";
    cblm = new QStringListModel;
    cblm->setStringList(cbl);
    cb = new s_tqComboBox;
    cb->setObjectName("npointscb");
    cb->setModel(cblm);
    connect(cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetNPoints(QString)));
    hlyout->addWidget(cb);
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Постоянная времени фильтрации:");
    hlyout->addWidget(lbl);
    QSpinBox *spb = new QSpinBox;
    spb->setObjectName("nfiltrspb");
    spb->setMinimum(1);
    spb->setMaximum(1000);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNFiltr(int)));
    hlyout->addWidget(spb);
    vlyout2->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Постоянная времени гармоник:");
    hlyout->addWidget(lbl);
    spb = new QSpinBox;
    spb->setObjectName("nhfiltrspb");
    spb->setMinimum(1);
    spb->setMaximum(10);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetNHFiltr(int)));
    hlyout->addWidget(spb);
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout1);
    vlyout1->addWidget(gb);

    cp1->setLayout(vlyout1);
    vlyout1 = new QVBoxLayout;
    vlyout1->addWidget(GeneralConf->Widget104());
    cp2->setLayout(vlyout1);
}

QWidget *confdialog_80::UNom(int numunom)
{
    QWidget *w = new QWidget;
    QString NumUNomStr = QString::number(numunom);
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl=new QLabel("Класс напряжения "+NumUNomStr+"-й группы, кВ:");
    gb2lyout->addWidget(lbl);
    QStringList cbl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
    QStringListModel *cblm = new QStringListModel;
    cblm->setStringList(cbl);
    s_tqComboBox *cb = new s_tqComboBox;
    cb->setObjectName("unom."+NumUNomStr);
    cb->setModel(cblm);
    cb->setEditable(true);
    QString tmps = "QComboBox {background-color: "+QString(ACONFGCLR)+";}";
    cb->setStyleSheet(tmps);
    connect(cb,SIGNAL(textChanged(int)),this,SLOT(SetVoltageClass(int)));
    gb2lyout->addWidget(cb);
    w->setLayout(gb2lyout);
    return w;
}

// 1 - первичный ток первой группы, 2 - вторичный ток первой группы, 3,4 - то же по второй группе

QWidget *confdialog_80::INom(int numinom)
{
    QWidget *w = new QWidget;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QString NumGroup = (numinom < 3) ? "1" : "2";
    QString Perv = (numinom%2) ? "первичные" : "вторичные";
    QString PervNum = (numinom%2) ? "1" : "2";
    QLabel *lbl=new QLabel("Номинальные "+Perv+" токи ТТ "+NumGroup+"-й группы, А: ");
    gb2lyout->addWidget(lbl, 0);
    for (int i = 0; i < 3; i++)
    {
        lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
        gb2lyout->addWidget(lbl, 1, Qt::AlignRight);
        s_tqSpinBox *dspbls = new s_tqSpinBox;
        dspbls->setObjectName("inom."+NumGroup+"."+PervNum+"."+QString::number(i)); // inom.<группа 1|2>.<тип перв/втор 1|2>.<phase>
        dspbls->setSingleStep(1);
        dspbls->setMinimum(1);
        dspbls->setMaximum(50000);
        dspbls->setDecimals(0);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetCurrent(double)));
        gb2lyout->addWidget(dspbls, 1);
    }
    w->setLayout(gb2lyout);
    return w;
}

QWidget *confdialog_80::Threshold(QString str, int numthr)
{
    QWidget *w = new QWidget;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(str);
    gb2lyout->addWidget(lbl);
    s_tqSpinBox *dspbls = new s_tqSpinBox;
    dspbls->setObjectName("thr."+QString::number(numthr));
    dspbls->setSingleStep(0.1);
    dspbls->setMinimum(0.0);
    dspbls->setMaximum(100.0);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetThreshold(double)));
    gb2lyout->addWidget(dspbls);
    w->setLayout(gb2lyout);
    return w;
}

void confdialog_80::SetOsc(int isChecked)
{
    int val;
    if ((val = GetWNum(sender()->objectName()) == 0))
    {
        DBGMSG;
        return;
    }
    quint16 tmpint = 0x0001;
    tmpint = tmpint << val;
    if (isChecked == Qt::Checked)
        econf->Bci_block.ddosc |= tmpint;
    else
        econf->Bci_block.ddosc &= ~tmpint;
}

void confdialog_80::SetThreshold(double dbl)
{
    int thr;
    if ((thr = GetWNum(sender()->objectName()) == 0))
    {
        DBGMSG;
        return;
    }
    switch (thr)
    {
    case 1: // % напряжения
    {
        econf->Bci_block.duosc = dbl;
        break;
    }
    case 2: // % тока
    {
        econf->Bci_block.diosc = dbl;
        break;
    }
    case 3: // % частоты
    {
        econf->Bci_block.duimin = dbl;
        break;
    }
    default:
        break;
    }
}

void confdialog_80::SetVoltageClass(int tmpi)
{
    int vnum;
    if ((vnum = GetWNum(sender()->objectName()) == 0))
    {
        DBGMSG;
        return;
    }
    if (vnum == 2) // вторая тройка напряжений
        econf->Bci_block.unom2 = tmpi;
    else
        econf->Bci_block.unom1 = tmpi;
}

// по имени виджета взять его номер

int confdialog_80::GetWNum(QString &ObjName)
{
    QStringList sl = ObjName.split(".");
    if (sl.size() < 1)
    {
        DBGMSG;
        return 0;
    }
    bool ok;
    int vnum = sl.at(1).toInt(&ok);
    if (!ok)
    {
        DBGMSG;
        return 0;
    }
    return vnum;
}

void confdialog_80::SetCurrent(double dbl)
{
    QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 4) // name, group, perv, phase
    {
        DBGMSG;
        return;
    }
    bool ok;
    int group, perv, phase;
    group = sl.at(1).toInt(&ok);
    if (ok)
    {
        perv = sl.at(2).toInt(&ok);
        if (ok)
        {
            phase = sl.at(3).toInt(&ok);
            if (ok)
            {
                if (perv == 1) // первичная группа
                    econf->Bci_block.inom1[(group-1)*3+phase] = dbl;
                else
                    econf->Bci_block.inom2[(group-1)*3+phase] = dbl;
                return;
            }
        }
    }
    DBGMSG;
}

void confdialog_80::SetEqType(int tmpi)
{
    econf->Bci_block.eq_type = tmpi;
}

void confdialog_80::SetNPoints(QString tmpi)
{
    econf->Bci_block.npoints = tmpi.toInt();
}

void confdialog_80::SetNFiltr(int tmpi)
{
    econf->Bci_block.nfiltr = tmpi;
}

void confdialog_80::SetNHFiltr(int tmpi)
{
    econf->Bci_block.nhfiltr = tmpi;
}

void confdialog_80::WriteConfDataToModule()
{
    cn->Send(CN_WF, Canal::BT_NONE, &econf->Bci_block, sizeof(econf->Bci_block), 2, econf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
        MessageBox2::information(this, "OK", "Операция проведена успешно!");
}

void confdialog_80::SetNewConf()
{
/*    cn->Send(CN_Cnc);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
    {
        MessageBox2::information(this, "OK", "Переведено успешно");
        emit BsiIsNeedToBeAcquiredAndChecked();
    } */
}

void confdialog_80::SetDefConf()
{
    econf->Bci_defblock.mainblk.MTypeB = pc.ModuleBsi.MTypeB; // делаем для того, чтобы типы совпадали
    econf->Bci_defblock.mainblk.MTypeM = pc.ModuleBsi.MTypeM; // делаем для того, чтобы подтипы совпадали
    memcpy(&econf->Bci_block, &econf->Bci_defblock, sizeof(config_80::Bci));
    FillConfData();
}

void confdialog_80::LoadConf()
{
    QByteArray ba;
    ba = pc.LoadFile("Config files (*.ecf)");
    if (pc.RestoreDataMem(&(ba.data()[0]), ba.size(), econf->Config))
    {
        WARNMSG("");
        return;
    }
    FillConfData();
    MessageBox2::information(this, "OK", "Загрузка прошла успешно!");
}

void confdialog_80::SaveConf()
{
    QByteArray *ba = new QByteArray;
    ba->resize(MAXBYTEARRAY);
    pc.StoreDataMem(&(ba->data()[0]), econf->Config, 0x0001);
    quint32 BaLength = static_cast<quint8>(ba->data()[4]);
    BaLength += static_cast<quint8>(ba->data()[5])*256;
    BaLength += static_cast<quint8>(ba->data()[6])*65536;
    BaLength += static_cast<quint8>(ba->data()[7])*16777216;
    BaLength += sizeof(publicclass::FileHeader); // FileHeader
    int res = pc.SaveFile("Config files (*.ecf)", &(ba->data()[0]), BaLength);
    switch (res)
    {
    case 0:
        MessageBox2::information(this, "OK", "Записано успешно!");
        break;
    case 1:
        MessageBox2::error(this, "error", "Ошибка при записи файла!");
        break;
    case 2:
        MessageBox2::error(this, "error", "Пустое имя файла!");
        break;
    case 3:
        MessageBox2::error(this, "error", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void confdialog_80::SetComboBox(QString name, QString &text)
{
    s_tqComboBox *cb = this->findChild<s_tqComboBox *>(name);
    if (cb != 0)
        cb->setCurrentText(text);
}

void confdialog_80::SetSpinBox(QString name, double value)
{
    s_tqSpinBox *dspbls = this->findChild<s_tqSpinBox *>(name);
    if (dspbls != 0)
        dspbls->setValue(value);
}

void confdialog_80::SetCheckBox(QString name, bool isChecked)
{
    s_tqCheckBox *chb = this->findChild<s_tqCheckBox *>(name);
    if (chb != 0)
        chb->setChecked(isChecked);
}
