#include "interfaceserialdialog.h"

#include "../gen/error.h"
#include "../interfaces/settingstypes.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <memory>
InterfaceSerialDialog::InterfaceSerialDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
}

void InterfaceSerialDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    tableView = WDFunc::NewQTV(this, "", nullptr);
    lyout->addWidget(tableView);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(10);
    hlyout->addWidget(WDFunc::NewPB(this, "newrspb", "Добавить", this, &InterfaceSerialDialog::addInterface));
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Удалить"), this, [this] {
        auto name = tableView->currentIndex().siblingAtColumn(0).data().toString();
        removeDevice(name);
        updateModel();
    }));
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    connect(tableView, &QTableView::doubleClicked, this, &InterfaceSerialDialog::setInterface);
    AbstractInterfaceDialog::setupUI();
}

void InterfaceSerialDialog::setInterface(QModelIndex index)
{
    auto *mdl = index.model();
    int row = index.row();

    QString name = mdl->data(mdl->index(row, 0)).toString();
    SerialPortSettings settings;
    settings.Port = mdl->data(mdl->index(row, 1)).toString();
    settings.Baud = mdl->data(mdl->index(row, 2)).toUInt();
    settings.Parity = mdl->data(mdl->index(row, 3)).toString();
    settings.Stop = mdl->data(mdl->index(row, 4)).toString();
    settings.Address = mdl->data(mdl->index(row, 5)).toUInt();
    if (!settings.isValid())
        return;
    ConnectStruct st { name, settings };
    emit accepted(st);
}

void InterfaceSerialDialog::addInterface()
{
    QStringList ports;
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : portlist)
        ports << info.portName();
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("rsdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE2(dlg, "namele"), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB2(dlg, "portcb", ports), count++, 1, 1, 1);
    QStringList sl { "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB2(dlg, "speedcb", sl), count++, 1, 1, 1);
    sl = QStringList({ "Нет", "Нечет", "Чет" });
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB2(dlg, "paritycb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList({ "1", "2" });
    lyout->addWidget(WDFunc::NewCB2(dlg, "stopbitcb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewSPB2(dlg, "addressspb", 1, 255, 0), count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(dlg, "acceptpb", "Сохранить", this, &InterfaceSerialDialog::acceptedInterface));
    hlyout->addWidget(WDFunc::NewPB(dlg, "cancelpb", "Отмена", [dlg] { dlg->close(); }));

    lyout->addLayout(hlyout, count, 0, 1, 2, Qt::AlignCenter);
    dlg->setLayout(lyout);
    dlg->adjustSize();
    dlg->exec();
}

bool InterfaceSerialDialog::updateModel()
{
    QStringList rslist;
    QSettings sets;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString rsname = "RS485-" + QString::number(i);
        if (sets.contains(rsname))
            rslist << sets.value(rsname, "").toString();
    }
    QStringList sl { "Имя", "Порт", "Скорость", "Четность", "Стоп бит", "Адрес" };
    QStandardItemModel *mdl = static_cast<QStandardItemModel *>(tableView->model());
    if (mdl == nullptr)
        mdl = new QStandardItemModel(this);
    else
        mdl->clear();
    mdl->setHorizontalHeaderLabels(sl);
    for (const auto &item : qAsConst(rslist))
    {
        QString key = QCoreApplication::applicationName();
        key += "\\" + item;
        auto sets = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));
        QList<QStandardItem *> items {
            new QStandardItem(item),                                 //
            new QStandardItem(sets->value("port", "").toString()),   //
            new QStandardItem(sets->value("speed", "").toString()),  //
            new QStandardItem(sets->value("parity", "").toString()), //
            new QStandardItem(sets->value("stop", "").toString()),   //
            new QStandardItem(sets->value("address", "").toString()) //
        };
        mdl->appendRow(items);
    }
    tableView->setModel(mdl);
    tableView->resizeColumnsToContents();
    return true;
}

void InterfaceSerialDialog::acceptedInterface()
{
    QDialog *dlg = this->findChild<QDialog *>("rsdlg");
    if (dlg == nullptr)
        return;
    QString name = WDFunc::LEData(dlg, "namele");
    // check if there's such name in registry
    if (isKeyExist("RS485-", name))
    {
        QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
        return;
    }
    rotateSettings("RS485-", name);
    QString key = QCoreApplication::applicationName();
    key += "\\" + name;
    auto settings = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));
    settings->setValue("port", WDFunc::CBData(dlg, "portcb"));
    settings->setValue("speed", WDFunc::CBData(dlg, "speedcb"));
    settings->setValue("parity", WDFunc::CBData(dlg, "paritycb"));
    settings->setValue("stop", WDFunc::CBData(dlg, "stopbitcb"));
    int spbdata;
    WDFunc::SPBData(dlg, "addressspb", spbdata);
    settings->setValue("address", QString::number(spbdata));
    if (!updateModel())
        qCritical() << Error::GeneralError;
    dlg->close();
}
