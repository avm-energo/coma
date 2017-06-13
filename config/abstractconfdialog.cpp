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
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConf()));
    if (pc.Emul)
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    pb->setIcon(QIcon(":/pic/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadConfFromFile()));
    wdgtlyout->addWidget(pb, 1, 0, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon(":/pic/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveConfToFile()));
    wdgtlyout->addWidget(pb, 1, 1, 1, 1);
    pb = new QPushButton("Задать конфигурацию по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SIGNAL(LoadDefConf()));
    wdgtlyout->addWidget(pb, 2, 0, 1, 2);
    wdgt->setLayout(wdgtlyout);
    return wdgt;
}

void AbstractConfDialog::PrereadConf()
{
    if ((pc.ModuleBsi.Hth & HTH_CONFIG) || (pc.Emul)) // если в модуле нет конфигурации, заполнить поля по умолчанию
        emit LoadDefConf();
    else // иначе заполнить значениями из модуля
        ReadConf();
}

// по имени виджета взять его номер

int AbstractConfDialog::GetChNumFromObjectName(QString ObjectName)
{
    QStringList ObjectNameSl = ObjectName.split(".");
    int ChNum;
    bool ok;
    if (ObjectNameSl.size()>1)
        ChNum = ObjectNameSl.at(1).toInt(&ok);
    else
        return GENERALERROR;
    if (!ok)
        return GENERALERROR;
    return ChNum;
}