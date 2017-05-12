#include "abstractconfdialog.h"
#include <QPushButton>
#include <QCoreApplication>
#include <QGridLayout>
#include <QGroupBox>
#include "../widgets/s_tqspinbox.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../canal.h"

AbstractConfDialog::AbstractConfDialog(QWidget *parent) : QDialog(parent)
{
}

QWidget *AbstractConfDialog::Widget104()
{
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    QGridLayout *glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    QLabel *lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    s_tqSpinBox *dspbls = WDFunc::NewSPB(w, "spb.1", 0, 65535, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса:");
    glyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.2", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1:");
    glyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.3", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2:");
    glyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.4", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3:");
    glyout->addWidget(lbl,4,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.5", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl,5,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.6", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl,6,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(w, "spb.7", 0, 255, 1, 0, ACONFGCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    lbl=new QLabel("c");
    glyout->addWidget(lbl,6,2,1,1);
    lbl = new QLabel("Тип синхр. времени:");
    glyout->addWidget(lbl,7,0,1,1,Qt::AlignRight);
    QStringList cbl = QStringList() << "SNTP+PPS" << "SNTP";
    s_tqComboBox *cb = WDFunc::NewCB(w, "spb.8", cbl, ACONFGCLR);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cb, 7, 1, 1, 2);
    gb->setLayout(glyout);
    vlyout->addWidget(gb);
    w->setLayout(vlyout);
    return w;
}

void AbstractConfDialog::ReadConf()
{
    cn->Send(CN_GF,Canal::BT_NONE,NULL,0,1,S2Config);
    if (cn->result == NOERROR)
        emit NewConfLoaded();
    else
        MessageBox2::error(this, "ошибка", "Ошибка чтения конфигурации из модуля");
}

void AbstractConfDialog::WriteConf()
{
    if (CheckConf())
    {
        ERMSG("В конфигурации есть ошибки. Проверьте и исправьте");
        return;
    }
    cn->Send(CN_WF, Canal::BT_NONE, NULL, 0, 1, S2Config);
    if (cn->result == NOERROR)
    {
        emit BsiIsNeedToBeAcquiredAndChecked();
        MessageBox2::information(this, "Внимание", "Запись конфигурации и переход прошли успешно!");
    }
}

void AbstractConfDialog::SaveConfToFile()
{
    QByteArray *ba = new QByteArray;
    ba->resize(MAXBYTEARRAY);
    pc.StoreDataMem(&(ba->data()[0]), S2Config, 0x0001); // 0x101 - номер файла конфигурации
    quint32 BaLength = static_cast<quint8>(ba->data()[4]);
    BaLength += static_cast<quint8>(ba->data()[5])*256;
    BaLength += static_cast<quint8>(ba->data()[6])*65536;
    BaLength += static_cast<quint8>(ba->data()[7])*16777216;
    BaLength += sizeof(publicclass::FileHeader); // FileHeader
    int res = pc.SaveFile("Config files (*.cf)", &(ba->data()[0]), BaLength);
    switch (res)
    {
    case NOERROR:
        MessageBox2::information(this, "Внимание", "Записано успешно!");
        break;
    case ER_FILEWRITE:
        ERMSG("Ошибка при записи файла!");
        break;
    case ER_FILENAMEEMP:
        ERMSG("Пустое имя файла!");
        break;
    case ER_FILEOPEN:
        ERMSG("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void AbstractConfDialog::LoadConfFromFile()
{
    QByteArray ba;
    ba = pc.LoadFile("Config files (*.cf)");
    if (pc.RestoreDataMem(&(ba.data()[0]), ba.size(), S2Config))
    {
        WARNMSG("Ошибка при разборе файла конфигурации");
        return;
    }
    emit NewConfLoaded();
    MessageBox2::information(this, "Успешно", "Загрузка прошла успешно!");
}

QWidget *AbstractConfDialog::ConfButtons()
{
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QPushButton *pb = new QPushButton("Прочитать из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadConf()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 0, 1, 1);
    pb = new QPushButton("Записать в модуль");
    pb->setObjectName("WriteConfPB");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConfDataToModule()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadConfFromFile()));
    wdgtlyout->addWidget(pb, 1, 0, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveConfToFile()));
    wdgtlyout->addWidget(pb, 1, 1, 1, 1);
    pb = new QPushButton("Задать конфигурацию по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SIGNAL(LoadDefConf()));
    wdgtlyout->addWidget(pb, 2, 0, 1, 2);
    wdgt->setLayout(wdgtlyout);
    return wdgt;
}

