#include "interfaceemudialog.h"

#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>
#include <QStandardItemModel>
InterfaceEmuDialog::InterfaceEmuDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
}

void InterfaceEmuDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    tableView = WDFunc::NewQTV(this, "", nullptr);
    lyout->addWidget(tableView);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(10);
    hlyout->addWidget(WDFunc::NewPB(this, "newrspb", "Добавить", this, &InterfaceEmuDialog::addInterface));
    hlyout->addWidget(WDFunc::NewPB(this, "", tr("Удалить"), this, [this] {
        auto name = tableView->currentIndex().siblingAtColumn(0).data().toString();
        removeDevice(name);
        updateModel();
    }));
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    connect(tableView, &QTableView::doubleClicked, this, &InterfaceEmuDialog::setInterface);
    AbstractInterfaceDialog::setupUI();
}

bool InterfaceEmuDialog::updateModel()
{
    QStringList rslist;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        auto sets = std::unique_ptr<QSettings>(new QSettings);
        QString rsname = "EMU-" + QString::number(i);
        rslist << sets->value(rsname, "").toString();
    }
    QStringList sl { "Имя", "typeB", "typeM" };
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
            new QStandardItem(item),                                //
            new QStandardItem(sets->value("typeb", "").toString()), //
            new QStandardItem(sets->value("typem", "").toString()), //
        };
        mdl->appendRow(items);
    }
    tableView->setModel(mdl);
    tableView->resizeColumnsToContents();
    return true;
}

void InterfaceEmuDialog::setInterface(QModelIndex index)
{
    auto *mdl = index.model();
    int row = index.row();

    QString name = mdl->data(mdl->index(row, 0)).toString();
    EmulatorSettings settings;

    QString str = mdl->data(mdl->index(row, 1)).toString();
    settings.typeB = str.toUInt(nullptr, 16);
    str = mdl->data(mdl->index(row, 2)).toString();
    settings.typeM = str.toUInt(nullptr, 16);

    ConnectStruct st { name, settings };
    emit accepted(st);
}

void InterfaceEmuDialog::addInterface()
{
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("emudlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL2(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE2(dlg, "namele"), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL2(dlg, "typeB:"), count, 0, 1, 1, Qt::AlignLeft);
    auto lineEdit = WDFunc::NewLE2(dlg, "typeble");
    lineEdit->setInputMask("HH;");
    lyout->addWidget(lineEdit, count++, 1, 1, 1, Qt::AlignLeft);

    lyout->addWidget(WDFunc::NewLBL2(dlg, "typeM:"), count, 0, 1, 1, Qt::AlignLeft);
    lineEdit = WDFunc::NewLE2(dlg, "typemle");
    lineEdit->setInputMask("HH;");
    lyout->addWidget(lineEdit, count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(dlg, "acceptpb", "Сохранить", this, &InterfaceEmuDialog::acceptedInterface));
    hlyout->addWidget(WDFunc::NewPB(dlg, "cancelpb", "Отмена", [dlg] { dlg->close(); }));

    lyout->addLayout(hlyout, count, 0, 1, 2, Qt::AlignCenter);
    dlg->setLayout(lyout);
    dlg->adjustSize();
    dlg->exec();
}

void InterfaceEmuDialog::acceptedInterface()
{
    QDialog *dlg = this->findChild<QDialog *>("emudlg");
    if (dlg == nullptr)
        return;
    QString name = WDFunc::LEData(dlg, "namele");
    // check if there's such name in registry
    if (isKeyExist("EMU-", name))
    {
        QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
        return;
    }
    rotateSettings("EMU-", name);
    QString key = QCoreApplication::applicationName();
    key += "\\" + name;
    auto settings = std::unique_ptr<QSettings>(new QSettings(QCoreApplication::organizationName(), key));
    settings->setValue("typeb", WDFunc::LEData(dlg, "typeble"));
    settings->setValue("typem", WDFunc::LEData(dlg, "typemle"));

    if (!updateModel())
        qCritical() << Error::GeneralError;
    dlg->close();
}
