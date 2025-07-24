#include "dialogs/interfaceemudialog.h"

#include <gen/error.h>
#include <gen/settings.h>
#include <gen/stdfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/lefunc.h>
#include <widgets/pbfunc.h>
#include <widgets/tvfunc.h>

#include <QCoreApplication>
#include <QMessageBox>
#include <QStandardItemModel>

InterfaceEmuDialog::InterfaceEmuDialog(QWidget *parent) : AbstractInterfaceDialog(parent) { }

InterfaceEmuDialog::~InterfaceEmuDialog() noexcept { }

void InterfaceEmuDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    m_tableView = TVFunc::New(this, "", nullptr);
    lyout->addWidget(m_tableView);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(10);
    hlyout->addWidget(PBFunc::New(this, "newrspb", "Добавить", this, &InterfaceEmuDialog::addInterface));
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    connect(m_tableView, &QTableView::doubleClicked, this, &InterfaceEmuDialog::setInterface);
    AbstractInterfaceDialog::setupUI();
}

bool InterfaceEmuDialog::updateModel()
{
    QStringList rslist;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString rsname = "EMU-" + QString::number(i);
        rslist << QString(Settings::get(rsname, ""));
    }
    QStringList sl { "Имя", "typeB", "typeM" };
    QStandardItemModel *mdl = static_cast<QStandardItemModel *>(m_tableView->model());
    if (mdl == nullptr)
        mdl = new QStandardItemModel(this);
    else
        mdl->clear();
    mdl->setHorizontalHeaderLabels(sl);
    for (const auto &item : std::as_const(rslist))
    {
        QString key = QCoreApplication::applicationName();
        key += "\\" + item;
        QList<QStandardItem *> items {
            new QStandardItem(item),                                       //
            new QStandardItem(QString(Settings::get(key + "/typeb", ""))), //
            new QStandardItem(QString(Settings::get(key + "/typem", ""))), //
        };
        mdl->appendRow(items);
    }
    m_tableView->setModel(mdl);
    m_tableView->resizeColumnsToContents();
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
    lyout->addWidget(LBLFunc::New(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(LEFunc::New(dlg, "namele"), count++, 1, 1, 1);
    lyout->addWidget(LBLFunc::New(dlg, "typeB:"), count, 0, 1, 1, Qt::AlignLeft);
    auto lineEdit = LEFunc::New(dlg, "typeble");
    lineEdit->setInputMask("HH;");
    lyout->addWidget(lineEdit, count++, 1, 1, 1, Qt::AlignLeft);

    lyout->addWidget(LBLFunc::New(dlg, "typeM:"), count, 0, 1, 1, Qt::AlignLeft);
    lineEdit = LEFunc::New(dlg, "typemle");
    lineEdit->setInputMask("HH;");
    lyout->addWidget(lineEdit, count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(PBFunc::New(dlg, "acceptpb", "Сохранить", this, &InterfaceEmuDialog::acceptedInterface));
    hlyout->addWidget(PBFunc::New(dlg, "cancelpb", "Отмена", [dlg] { dlg->close(); }));

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
    QString name = LEFunc::Data(dlg, "namele");
    // rotateSettings("EMU-", name);
    QString key = QCoreApplication::applicationName();
    key += "\\" + name;
    Settings::set("typeb", LEFunc::Data(dlg, "typeble"));
    Settings::set("typem", LEFunc::Data(dlg, "typemle"));

    if (!updateModel())
        qCritical() << Error::GeneralError;
    dlg->close();
}
